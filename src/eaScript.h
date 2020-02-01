#pragma once
#include <istream>
#include <map>
#include <string>
#include <vector>

/*
块基类
*/
class eaScriptBlock
{
};

/*
对象
*/
class eaScriptObject
{
	std::shared_ptr<void> ptr;
	size_t typeHash;

	eaScriptObject(std::shared_ptr<void> ptr, size_t typeHash) :ptr(ptr), typeHash(typeHash)
	{
	}
public:
	eaScriptObject(nullptr_t ptr) :ptr(ptr), typeHash(typeid(void).hash_code())
	{
	}

	template<class T, class ...Args> static eaScriptObject Create(Args&& ... args)
	{
		auto ptr = (std::shared_ptr<void>) std::make_shared<T>(args...);
		return eaScriptObject(ptr, typeid(T).hash_code());
	}

	bool IsType(size_t typeHash) const
	{
		return this->typeHash == typeHash;
	}

	template <class T> bool IsType() const
	{
		return IsType(typeid(T).hash_code());
	}

	template <class T> T* Get()
	{
		return (T*)ptr.get();
	}

	template <class T> const T* Get() const
	{
		return (T*)ptr.get();
	}

	bool operator== (nullptr_t null) const
	{
		return ptr.get() == null;
	}
};

/*
数组对象
*/
using eaScriptArray = std::vector<eaScriptObject>;

/*
Lua对象
*/
class eaScriptLuaObject : public std::string
{
public:
	eaScriptLuaObject(const std::string str) :std::string(str) {}
};

/*
数字对象
*/
using eaScriptNumber = double;

/*
枚举对象
*/
class eaScriptEnum : public std::string
{
public:
	eaScriptEnum(const std::string str) :std::string(str) {}
};

/*
字符串对象
*/
struct eaScriptString
{
private:
	std::string str;

public:
	eaScriptString(const std::string& strWithLua) :str(strWithLua)
	{
	}

	const std::string& GetRawStr() const
	{
		return str;
	}
};

/*
任务块
*/
class eaScriptTaskBlock : public eaScriptBlock
{
public:
	struct arg
	{
		std::string name;
		eaScriptObject obj;
	};
	using argList = std::vector<eaScriptTaskBlock::arg>;

	const std::string task;
	const argList args;

	eaScriptTaskBlock(std::string task, argList args) :task(task), args(args)
	{
	}
};

/*
注释块
*/
class eaScriptNoteBlock : public eaScriptBlock
{
public:
	const std::string note;

	eaScriptNoteBlock(std::string note) :note(note)
	{
	}
};

/*
标签块
*/
class eaScriptLabelBlock : public eaScriptBlock
{
public:
	const std::string label;
	long long pos;

	eaScriptLabelBlock(std::string label, long long pos) :label(label), pos(pos)
	{
	}
};

/*
Lua块
*/
class eaScriptLuaBlock : public eaScriptBlock
{
public:
	const std::string code;

	eaScriptLuaBlock(std::string code) :code(code)
	{
	}
};

/*
文本块
*/
class eaScriptTextBlock : public eaScriptBlock
{
public:	
	const eaScriptString text;

	eaScriptTextBlock(std::string text) :text(text)
	{
	}
};

class eaScriptReader
{
	std::istream& s;

	int line = 1;
	int pos = 0;

	bool IsObjectEnd()
	{
		char c = Peek();
		return c == ' ' || c == ',' || c == ']' || c == '\n' || c == '\t' || c == EOF;
	}

public:
	inline bool Eof()
	{
		return Peek() == EOF;
	}

	inline int Line()
	{
		return line;
	}

	inline int Pos()
	{
		return pos;
	}

	eaScriptReader(std::istream& s) :s(s) {}

	int Get();
	int Peek();

	inline void SkipLine()
	{
		while (!Eof() && Get() != '\n')
			Get();
	}

	inline void SkipToNext()
	{
		while (!IsObjectEnd())
			Get();
	}

	inline std::string GetLine()
	{
		std::string str;
		
		char c = Get();
		while (!Eof() && c != '\n')
		{
			str += c;
			c = Get();
		}

		return str;
	}

	eaScriptObject ReadObject();
	eaScriptObject ReadNumber();
	eaScriptObject ReadArray();
	eaScriptObject ReadString();
	eaScriptObject ReadEnum();
	eaScriptObject ReadLuaObject();

	eaScriptObject ReadTaskBlock();
	eaScriptObject ReadNoteBlock();
	eaScriptObject ReadLabelBlock();
	eaScriptObject ReadLuaBlock();
	eaScriptObject ReadTextBlock();
};

/*
脚本
*/
class eaScript : public eaScriptBlock
{
	std::vector<eaScriptObject> blocks;
	std::map<std::string, long long> labels;

protected:
	eaScript() = default;

public:
	const std::vector<eaScriptObject>& Blocks() const
	{
		return blocks;
	}

	long long GetLabelPos(std::string name)
	{
		auto result = labels.find(name);
		if (result == labels.end())
			return -1;
		return result->second;
	}

	eaScript(const eaScript&) = delete;

	static std::shared_ptr<eaScript> FromString(std::string str);
	static std::shared_ptr<eaScript> FromFile(std::string fileName);
	static std::shared_ptr<eaScript> FromStream(std::istream& stream);

	static std::string GetError();
};
