#include <sstream>
#include <fstream>
#include "easScript.h"

using namespace std;

string scripeLoadError;

void AddError(int line, int pos, string msg)
{
	scripeLoadError += 
		"Line: "s + to_string(line) + " pos: " + to_string(pos) + " " + msg + "\r";
}

bool IsDigit(char c)
{
	return c >= 48 && c <= 57;
}

bool IsLetter(char c)
{
	return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

std::shared_ptr<easScript> easScript::FromString(std::string str)
{
	stringstream ss(str);
	return FromStream(ss);
}

std::shared_ptr<easScript> easScript::FromFile(std::string fileName)
{
	ifstream file(fileName, ios::binary);
	return FromStream(file);
}

std::shared_ptr<easScript> easScript::FromStream(std::istream& stream)
{
	auto script = std::shared_ptr<easScript>(new easScript());
	easScriptReader reader(stream);

	while (!reader.Eof())
	{
		char c = reader.Peek();

		switch (c)
		{
		case '\r':
		case '\t':
			reader.Get();
			break;

		case '#':
			script->blocks.emplace_back(reader.ReadNoteBlock());
			break;

		case ':':
			script->blocks.emplace_back(reader.ReadTaskBlock());
			break;

		case '{':
			script->blocks.emplace_back(reader.ReadLuaBlock());
			break;

		case '=':
		{
			auto label = reader.ReadLabelBlock();
			auto labelBlock = label.Get<easLabelBlock>();
			auto pos = script->blocks.size();
			auto name = labelBlock->label;
			labelBlock->pos = pos;

			script->labels[name] = labelBlock->pos;

			script->blocks.emplace_back(label);
			break;
		}

		default:
			script->blocks.emplace_back(reader.ReadTextBlock());
		}
	}

	return script;
}

std::string easScript::GetError()
{
	auto error = scripeLoadError;
	scripeLoadError = "";
	return error;
}

easObject easScriptReader::ReadNoteBlock()
{
	// �ж��Ƿ�Ϊע�Ϳ�
	int c = Get();

	if (c != '#')
	{
		AddError(line, pos, "�ڲ������޷���ȡע�Ϳ�");
		SkipLine();
		return nullptr;
	}

	string line;
	getline(s, line);
	return easObject::Create<easNoteBlock>(line);
}

easObject easScriptReader::ReadTaskBlock()
{
	// �ж��Ƿ�Ϊ�����
	int c = Get();

	if (c != ':')
	{
		AddError(line, pos, "�ڲ������޷���ȡ�����");
		SkipLine();
		return nullptr;
	}

	// ��ȡ��������
	string task;
	
	while (!IsObjectEnd())
		task += Get();

	// ��ȡ�������
	string argName;
	
	easTaskBlock::argList args;

	while (!Eof())
	{
		c = Get();

		// �����ո�
		if (c == ' ')
		{
			continue;
		}

		// ������ֹ
		if (c == '\r')
			break;

		// �����Ⱥź�ʼ������
		if (c == '=')
		{
			auto obj = ReadObject();

			if (obj == nullptr)
			{
				AddError(line, pos, "��ȡ�����ʧ��");
				SkipLine();
				break;
			}
			args.emplace_back(easTaskBlock::arg{ argName , obj });
			argName = "";

			continue;
		}

		// �ַ��Ƿ�Ϸ�
		if ((IsDigit(c) && argName == "") ||
			(!IsDigit(c) && !IsLetter(c)))
		{
			AddError(line, pos, "��⵽�Ƿ��ַ�"s + (char) c);
			SkipToSpace();
			continue;
		}

		argName += c;
	}

	return easObject::Create<easTaskBlock>(task, args);
}

easObject easScriptReader::ReadLabelBlock()
{
	// �ж��Ƿ�Ϊ��ǩ��
	int c = Get();
	int labelStartDepth = 0;

	while (c == '=')
	{
		c = Get();
		++labelStartDepth;
	}

	string labelName;
	while (c != '=')
	{
		if (Eof() || c == '\r')
		{
			AddError(line, pos, "δ�ҵ���ǩĩ�˵ĵȺ�");
			SkipLine();
			return nullptr;
		}

		labelName += c;
		c = Get();
	}

	// �ж�ĩ�˵Ⱥ�����
	int labelEndDepth = 0;

	while (c == '=')
	{
		c = Get();
		++labelEndDepth;
	}

	if (labelEndDepth != labelStartDepth)
		AddError(line, pos,
			"��ǩĩ�˵ĵȺ�������ƥ�䣬���� "s + to_string(labelStartDepth) + "��ʵ�� " + to_string(labelEndDepth));

	return easObject::Create<easLabelBlock>(labelName,0);
}

easObject easScriptReader::ReadLuaBlock()
{
	// �ж��Ƿ�ΪLua��
	int c = Get();

	if (c != '{')
	{
		AddError(line, pos, "�ڲ������޷���ȡ�����");
		SkipLine();
		return nullptr;
	}

	// ��ȡLua����
	c = Get();
	string code;

	while (c != '}')
	{
		if (Eof())
		{
			AddError(line, pos, "δ�ҵ�Lua��ĩ�˻�����");
			SkipLine();
			return nullptr;
		}
		code += c;
		c = Get();
	}
	
	// ��������
	c = Get();

	if (c != '\r')
	{
		AddError(line, pos, "Lua��ĩ�˻����ź�Ӧ����");
		SkipLine();
	}

	return easObject::Create<easLuaBlock>(code);
}

easObject easScriptReader::ReadTextBlock()
{
	string text;

	while (!Eof())
	{
		auto rawLine = GetLine();

		// ��ȡ��
		string line;
		for (auto c : rawLine)
		{
			// ���������ո�
			if (line == "")
			{
				if (c == '\t')
					continue;
				if (c == ' ')
					continue;
			}

			line += c;
		}
		
		// ��������
		if (line == "")
			break;

		text += (text == "" ? "" : "\r") + line;
	}

	return easObject::Create<easTextBlock>(text);
}

easObject easScriptReader::ReadObject()
{
	while (Peek() == ' ')
		Get();

	int c = Peek();

	// ����
	if (IsDigit(c))
		return ReadNumber();

	// ö��
	if (IsLetter(c))
		return ReadEnum();

	// �ַ���
	if (c == '\"')
		return ReadString();

	// ����
	if (c == '[')
		return ReadArray();

	// Lua����
	if (c == '{')
		return ReadLuaObject();

	AddError(line, pos, "δ֪����");
	while (c != ' ' && c != '\r' && !Eof())
		c = Get();

	return nullptr;
}

easObject easScriptReader::ReadString()
{
	if (Get() != '"')
	{
		AddError(line, pos, "�ڲ������޷���ȡ�ַ���");
		return nullptr;
	}

	string str;

	while (Peek() != '"')
	{
		if (Eof())
		{
			AddError(line, pos, "����ķ��ʵ����ļ���β��δ�ҵ��ַ���ĩ������");
			return nullptr;
		}

		int c = Get();

		if (c == '\r')
		{
			AddError(line, pos, "δ�ҵ��ַ���ĩ������");
			return nullptr;
		}
		str += c;
	}

	Get();

	return easObject::Create<easString>(str);
}

easObject easScriptReader::ReadEnum()
{
	string str;

	while (!IsObjectEnd())
	{
		int c = Get();
		if (!IsDigit(c) && !IsLetter(c))
		{
			AddError(line, pos, "ö������ֻ���������ֺ���ĸ��ɣ�����" + c);
			return nullptr;
		}
		str += c;
	}

	return easObject::Create<easEnum>(str);
}

easObject easScriptReader::ReadLuaObject()
{
	if (Get() != '{')
	{
		AddError(line, pos, "�ڲ������޷���ȡ�����");
		return nullptr;
	}

	string str;

	while (Peek() != '}')
	{
		int c = Get();
		if (Eof())
		{
			AddError(line, pos, "����ķ��ʵ����ļ���β��δ�ҵ�Lua����ĩ�˵Ĵ�����"); 
			return nullptr;
		}
		str += c;
	}

	Get();

	return easObject::Create<easLuaObject>(str);
}

easObject easScriptReader::ReadNumber()
{
	string numStr;

	while (!IsObjectEnd())
	{
		int c = Get();
		if (!IsDigit(c) && c != '.')
		{
			AddError(line, pos, "��������ֻ���������ֺ�С������ɣ�����" + c);
			return nullptr;
		}
		numStr += c;
	}

	return easObject::Create<easNumber>(stod(numStr));
}

easObject easScriptReader::ReadArray()
{
	if (Get() != '[')
	{
		AddError(line, pos, "�ڲ������޷���ȡ����");
		return nullptr;
	}

	easArray objs;
	
	while (Peek() != ']')
	{
		auto obj = ReadObject();
		if (obj == nullptr)
		{
			AddError(line, pos, "��ȡ����ʧ��");
			return nullptr;
		}
		objs.push_back(obj);

		// ������һ������
		while (Peek() != ',' && Peek() != ']')
		{
			if (Eof())
			{
				AddError(line, pos, "δ�ҵ��������ĩ�˵ķ�����");
				return nullptr;
			}

			// ��ȡ��һ��
			Get();
		}

		if (Peek() == ',')
			Get();
	}

	Get();

	return easObject::Create<easArray>(objs);
}

