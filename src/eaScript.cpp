#include <sstream>
#include <fstream>
#include "eaScript.h"

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

std::shared_ptr<eaScript> eaScript::FromString(std::string str)
{
	stringstream ss(str);
	return FromStream(ss);
}

std::shared_ptr<eaScript> eaScript::FromFile(std::string fileName)
{
	ifstream file(fileName, ios::binary);
	return FromStream(file);
}

std::shared_ptr<eaScript> eaScript::FromStream(std::istream& stream)
{
	auto script = std::shared_ptr<eaScript>(new eaScript());
	eaScriptReader reader(stream);

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
			auto labelBlock = label.Get<eaScriptLabelBlock>();
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

std::string eaScript::GetError()
{
	auto error = scripeLoadError;
	scripeLoadError = "";
	return error;
}

eaScriptObject eaScriptReader::ReadNoteBlock()
{
	// 判断是否为注释块
	int c = Get();

	if (c != '#')
	{
		AddError(line, pos, "内部错误，无法读取注释块");
		SkipLine();
		return nullptr;
	}

	string line;
	getline(s, line);
	return eaScriptObject::Create<eaScriptNoteBlock>(line);
}

eaScriptObject eaScriptReader::ReadTaskBlock()
{
	// 判断是否为任务块
	int c = Get();

	if (c != ':')
	{
		AddError(line, pos, "内部错误，无法读取命令块");
		SkipLine();
		return nullptr;
	}

	// 获取任务名称
	string task;
	
	while (!IsObjectEnd())
		task += Get();

	// 获取命令参数
	string argName;
	
	eaScriptTaskBlock::argList args;

	while (!Eof())
	{
		c = Get();

		// 跳过空格
		if (c == ' ')
		{
			continue;
		}

		// 换行终止
		if (c == '\r')
			break;

		// 遇到等号后开始读对象
		if (c == '=')
		{
			auto obj = ReadObject();

			if (obj == nullptr)
			{
				AddError(line, pos, "读取任务块失败");
				SkipLine();
				break;
			}
			args.emplace_back(eaScriptTaskBlock::arg{ argName , obj });
			argName = "";

			continue;
		}

		// 字符是否合法
		if ((IsDigit(c) && argName == "") ||
			(!IsDigit(c) && !IsLetter(c)))
		{
			AddError(line, pos, "检测到非法字符"s + (char) c);
			SkipToSpace();
			continue;
		}

		argName += c;
	}

	return eaScriptObject::Create<eaScriptTaskBlock>(task, args);
}

eaScriptObject eaScriptReader::ReadLabelBlock()
{
	// 判断是否为标签块
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
			AddError(line, pos, "未找到标签末端的等号");
			SkipLine();
			return nullptr;
		}

		labelName += c;
		c = Get();
	}

	// 判断末端等号数量
	int labelEndDepth = 0;

	while (c == '=')
	{
		c = Get();
		++labelEndDepth;
	}

	if (labelEndDepth != labelStartDepth)
		AddError(line, pos,
			"标签末端的等号数量不匹配，期望 "s + to_string(labelStartDepth) + "，实际 " + to_string(labelEndDepth));

	return eaScriptObject::Create<eaScriptLabelBlock>(labelName,0);
}

eaScriptObject eaScriptReader::ReadLuaBlock()
{
	// 判断是否为Lua块
	int c = Get();

	if (c != '{')
	{
		AddError(line, pos, "内部错误，无法读取命令块");
		SkipLine();
		return nullptr;
	}

	// 读取Lua代码
	c = Get();
	string code;

	while (c != '}')
	{
		if (Eof())
		{
			AddError(line, pos, "未找到Lua块末端花括号");
			SkipLine();
			return nullptr;
		}
		code += c;
		c = Get();
	}
	
	// 读到新行
	c = Get();

	if (c != '\r')
	{
		AddError(line, pos, "Lua块末端花括号后应换行");
		SkipLine();
	}

	return eaScriptObject::Create<eaScriptLuaBlock>(code);
}

eaScriptObject eaScriptReader::ReadTextBlock()
{
	string text;

	while (!Eof())
	{
		auto rawLine = GetLine();

		// 获取行
		string line;
		for (auto c : rawLine)
		{
			// 空行跳过空格
			if (line == "")
			{
				if (c == '\t')
					continue;
				if (c == ' ')
					continue;
			}

			line += c;
		}
		
		// 空行跳出
		if (line == "")
			break;

		text += (text == "" ? "" : "\r") + line;
	}

	return eaScriptObject::Create<eaScriptTextBlock>(text);
}

eaScriptObject eaScriptReader::ReadObject()
{
	while (Peek() == ' ')
		Get();

	int c = Peek();

	// 数字
	if (IsDigit(c))
		return ReadNumber();

	// 枚举
	if (IsLetter(c))
		return ReadEnum();

	// 字符串
	if (c == '\"')
		return ReadString();

	// 数组
	if (c == '[')
		return ReadArray();

	// Lua对象
	if (c == '{')
		return ReadLuaObject();

	AddError(line, pos, "未知对象");
	while (c != ' ' && c != '\r' && !Eof())
		c = Get();

	return nullptr;
}

eaScriptObject eaScriptReader::ReadString()
{
	if (Get() != '"')
	{
		AddError(line, pos, "内部错误，无法读取字符串");
		return nullptr;
	}

	string str;

	while (Peek() != '"')
	{
		if (Eof())
		{
			AddError(line, pos, "意外的访问到了文件结尾，未找到字符串末端引号");
			return nullptr;
		}

		int c = Get();

		if (c == '\r')
		{
			AddError(line, pos, "未找到字符串末端引号");
			return nullptr;
		}
		str += c;
	}

	Get();

	return eaScriptObject::Create<eaScriptString>(str);
}

eaScriptObject eaScriptReader::ReadEnum()
{
	string str;

	while (!IsObjectEnd())
	{
		int c = Get();
		if (!IsDigit(c) && !IsLetter(c))
		{
			AddError(line, pos, "枚举类型只允许由数字和字母组成，发现" + c);
			return nullptr;
		}
		str += c;
	}

	return eaScriptObject::Create<eaScriptEnum>(str);
}

eaScriptObject eaScriptReader::ReadLuaObject()
{
	if (Get() != '{')
	{
		AddError(line, pos, "内部错误，无法读取命令块");
		return nullptr;
	}

	string str;

	while (Peek() != '}')
	{
		int c = Get();
		if (Eof())
		{
			AddError(line, pos, "意外的访问到了文件结尾，未找到Lua对象末端的大括号"); 
			return nullptr;
		}
		str += c;
	}

	Get();

	return eaScriptObject::Create<eaScriptLuaObject>(str);
}

eaScriptObject eaScriptReader::ReadNumber()
{
	string numStr;

	while (!IsObjectEnd())
	{
		int c = Get();
		if (!IsDigit(c) && c != '.')
		{
			AddError(line, pos, "数字类型只允许由数字和小数点组成，发现" + c);
			return nullptr;
		}
		numStr += c;
	}

	return eaScriptObject::Create<eaScriptNumber>(stod(numStr));
}

eaScriptObject eaScriptReader::ReadArray()
{
	if (Get() != '[')
	{
		AddError(line, pos, "内部错误，无法读取数组");
		return nullptr;
	}

	eaScriptArray objs;
	
	while (Peek() != ']')
	{
		auto obj = ReadObject();
		if (obj == nullptr)
		{
			AddError(line, pos, "读取数组失败");
			return nullptr;
		}
		objs.push_back(obj);

		// 读到下一个对象
		while (Peek() != ',' && Peek() != ']')
		{
			if (Eof())
			{
				AddError(line, pos, "未找到数组对象末端的方括号");
				return nullptr;
			}

			// 获取下一个
			Get();
		}

		if (Peek() == ',')
			Get();
	}

	Get();

	return eaScriptObject::Create<eaScriptArray>(objs);
}

