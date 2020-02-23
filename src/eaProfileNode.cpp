#include <algorithm>
#include "eaProfileNode.h"

using namespace std;

enum PropertyValueType
{
	PropertyNumber = 0, PropertyString = 1, PropertyBoolean = 2, PropertyTable = 3, PropertyNil = 4
};

void WriteString(ostream& stream, const string& str)
{
	uint64_t size = str.size();
	stream.write(reinterpret_cast<char*>(&size), sizeof(size));
	if (size != 0)
		stream.write(str.c_str(), size);
}

void WriteNumber(ostream& stream, double num)
{
	stream.write(reinterpret_cast<char*>(&num), sizeof(num));
}

void WriteInteger(ostream& stream, int64_t num)
{
	stream.write(reinterpret_cast<char*>(&num), sizeof(num));
}

void WriteBoolean(ostream& stream, bool b)
{
	stream.write(reinterpret_cast<char*>(&b), sizeof(b));
}

void WritePropertyValue(ostream& stream, const eaPropertyValue& value);
void WriteTable(ostream& stream, const eaPropertyValue::eaTable& table)
{
	WriteInteger(stream, table.size());

	for (auto& data : table)
	{
		WritePropertyValue(stream, data.first);
		WritePropertyValue(stream, data.second);
	}
}

void WritePropertyValue(ostream& stream, const eaPropertyValue& value)
{
	if (value.IsBoolean())
	{
		WriteInteger(stream, PropertyBoolean);
		WriteBoolean(stream, value.ToBoolean());
	}
	else if (value.IsNumber())
	{
		WriteInteger(stream, PropertyNumber);
		WriteNumber(stream, value.ToNumber());
	}
	else if (value.IsString())
	{
		WriteInteger(stream, PropertyString);
		WriteString(stream, value.ToString());
	}
	else if (value.IsTable())
	{
		WriteInteger(stream, PropertyTable);
		WriteTable(stream, value.ToTable());
	}
	else
	{
		WriteInteger(stream, PropertyNil);
	}
}

string ReadString(istream& stream)
{
	uint64_t size;
	stream.read(reinterpret_cast<char*>(&size), sizeof(size));
	string str;
	str.resize(size);
	if (size != 0)
		stream.read(&str[0], size);
	return str;
}

double ReadNumber(istream& stream)
{
	double num;
	stream.read(reinterpret_cast<char*>(&num), sizeof(num));
	return num;
}

int64_t ReadInteger(istream& stream)
{
	int64_t num;
	stream.read(reinterpret_cast<char*>(&num), sizeof(num));
	return num;
}

bool ReadBoolean(istream& stream)
{
	bool b;
	stream.read(reinterpret_cast<char*>(&b), sizeof(b));
	return b;
}

eaPropertyValue ReadPropertyValue(istream& stream);
eaPropertyValue::eaTable ReadTable(istream& stream)
{
	eaPropertyValue::eaTable table;
	auto size = (size_t) ReadInteger(stream);
	for (size_t i = 0; i < size; ++i)
	{
		auto key = ReadPropertyValue(stream);
		auto value = ReadPropertyValue(stream);
		table[key] = value;
	}
	return table;
}

eaPropertyValue ReadPropertyValue(istream& stream)
{
	eaPropertyValue value;

	auto type = ReadInteger(stream);

	switch (type)
	{
	case PropertyBoolean:
		value = make_shared<bool>(ReadBoolean(stream));
		break;
	case PropertyNumber:
		value = make_shared<double>(ReadNumber(stream));
		break;
	case PropertyString:
		value = make_shared<string>(ReadString(stream));
		break;
	case PropertyTable:
		value = make_shared< eaPropertyValue::eaTable>(ReadTable(stream));
		break;
	case PropertyNil:
		value = nullptr;
		break;
	}
	return value;
}

void eaProfileNode::WriteToStream(ostream& stream)
{
	WriteInteger(stream, data.size());

	for (auto& pair : data) 
	{
		auto& name = pair.first;
		auto type = pair.second.first;

		WriteString(stream, name);
		WriteNumber(stream, type);

		if (type == DataNode)
		{
			auto node = reinterpret_pointer_cast<eaProfileNode>(pair.second.second);
			node->WriteToStream(stream);
		}
		else if (type == DataPropertyValue)
		{
			auto value = reinterpret_pointer_cast<eaPropertyValue>(pair.second.second);
			WritePropertyValue(stream, *value);
		}
	}
}

void eaProfileNode::ReadFromStream(istream& stream)
{
	auto size = (size_t)ReadInteger(stream);

	for (size_t i = 0; i < size; ++i)
	{
		auto name = ReadString(stream);
		auto type = (int)ReadNumber(stream);

		if (type == DataNode)
		{
			auto node = Set(name);
			node->ReadFromStream(stream);
		}
		else if (type == DataPropertyValue)
		{
			auto value = ReadPropertyValue(stream);
			Set(name, value);
		}
	}
}