#pragma once
#include <istream>
#include <map>
#include <string>
#include <vector>

/*
块基类
*/
class easBlock
{
};

/*
对象
*/
class easObject
{
	std::shared_ptr<void> ptr;
	size_t typeHash;

	easObject(std::shared_ptr<void> ptr, size_t typeHash) :ptr(ptr), typeHash(typeHash)
	{
	}
public:
	easObject(nullptr_t ptr) :ptr(ptr), typeHash(typeid(void).hash_code())
	{
	}

	template<class T, class ...Args> static easObject Create(Args&& ... args)
	{
		auto ptr = (std::shared_ptr<void>) std::make_shared<T>(args...);
		return easObject(ptr, typeid(T).hash_code());
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
using easArray = std::vector<easObject>;

/*
Lua对象
*/
class easLuaObject : public std::string
{
public:
	easLuaObject(const std::string str) :std::string(str) {}
};

/*
数字对象
*/
using easNumber = double;

/*
枚举对象
*/
class easEnum : public std::string
{
public:
	easEnum(const std::string str) :std::string(str) {}
};

/*
字符串对象
*/
struct easString
{
private:
	std::string str;

public:
	easString(const std::string& strWithLua) :str(strWithLua)
	{
	}

	std::string GetStr() const;
};

/*
任务块
*/
class easTaskBlock : public easBlock
{
public:
	struct arg
	{
		std::string name;
		easObject obj;
	};
	using argList = std::vector<easTaskBlock::arg>;

	const std::string task;
	const argList args;

	easTaskBlock(std::string task, argList args) :task(task), args(args)
	{
	}
};

/*
注释块
*/
class easNoteBlock : public easBlock
{
public:
	const std::string note;

	easNoteBlock(std::string note) :note(note)
	{
	}
};

/*
标签块
*/
class easLabelBlock : public easBlock
{
public:
	const std::string label;
	long long pos;

	easLabelBlock(std::string label, long long pos) :label(label), pos(pos)
	{
	}
};

/*
Lua块
*/
class easLuaBlock : public easBlock
{
public:
	const std::string code;

	easLuaBlock(std::string code) :code(code)
	{
	}
};

/*
文本块
*/
class easTextBlock : public easBlock
{
public:	
	const easString text;

	easTextBlock(std::string text) :text(text)
	{
	}
};

class easScriptReader
{
	std::istream& s;

	int line = 1;
	int pos = 0;

	bool IsObjectEnd()
	{
		char c = Peek();
		return c == ' ' || c == ',' || c == ']' || c == '\r' || c == EOF;
	}

public:
	inline bool Eof()
	{
		return s.peek() == EOF;
	}

	inline int Line()
	{
		return line;
	}

	inline int Pos()
	{
		return pos;
	}

	easScriptReader(std::istream& s) :s(s) {}

	inline int Get()
	{
		int result = s.get();

		while (result == '\n')
			result = s.get();

		if (result == '\r')
		{
			++line;
			pos = 0;
		}

		++pos;

		return result;
	}

	inline int Peek()
	{
		auto result = s.peek();
		while (result == '\n')
		{
			s.get();
			result = s.peek();
		}
		return result;
	}

	inline void SkipLine()
	{
		auto str = GetLine();
	}

	inline void SkipToSpace()
	{
		char c;
		do
		{
			c = Get();
			if (c == ' ')
				break;
		} while (c > 0 && c != '\r');
	}

	inline std::string GetLine()
	{
		++line;
		pos = 0;

		std::string str;
		
		char c = Get();
		while (!Eof() && c != '\r')
		{
			str += c;
			c = Get();
		}

		return str;
	}

	easObject ReadObject();
	easObject ReadNumber();
	easObject ReadArray();
	easObject ReadString();
	easObject ReadEnum();
	easObject ReadLuaObject();

	easObject ReadTaskBlock();
	easObject ReadNoteBlock();
	easObject ReadLabelBlock();
	easObject ReadLuaBlock();
	easObject ReadTextBlock();
};

/*
脚本
*/
class easScript : public easBlock
{
	std::vector<easObject> blocks;
	std::map<std::string, long long> labels;

protected:
	easScript() = default;

public:
	const std::vector<easObject>& Blocks() const
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

	easScript(const easScript&) = delete;

	static std::shared_ptr<easScript> FromString(std::string str);
	static std::shared_ptr<easScript> FromFile(std::string fileName);
	static std::shared_ptr<easScript> FromStream(std::istream& stream);

	static std::string GetError();
};
