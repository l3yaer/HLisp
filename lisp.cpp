#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>

class Object
{
public:
	Object() : ref_count(0) {}

	virtual ~Object()
	{
		std::cout << "dealloc" << std::endl;
	}

	const Object* get() const
	{
		++ref_count;
		return this;
	}

	int release() const
	{
		return --ref_count;
	}

	int get_ref() const
	{
		return ref_count;
	}

private:
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	mutable int ref_count;
};

template<class T>
class ObjectPtr
{
public:
	ObjectPtr() : Object(nullptr) {}
	ObjectPtr(T *object) : object(nullptr)
	{
		get(object);
	}
	ObjectPtr(const ObjectPtr &ptr) : object(nullptr)
	{
		get(ptr.object);
	}
	~ObjectPtr()
	{
		release();
	}

	const ObjectPtr& operator=(const ObjectPtr &ptr)
	{
		get(ptr.object);
		return *this;
	}

	T *operator->() const
	{
		return object;
	}

private:
	void get(T *object)
	{
		if(object != nullptr)
			object->get();
		release();
		this->object = object;
	}

	void release()
	{
		if(object != nullptr && object->release() == 0)
			delete object;
	}

	T *object;
};

class Value : public Object
{
public:
	enum TYPE
	{
		NIL,
		SYMBOL,
		INTEGER,
		REAL,
		STRING,
		LAMBDA,
		NATIVE,
		LIST
	};

	TYPE type = TYPE::NIL;

	union
	{
		long int number;
		float real;
		std::string string;
		std::vector<ObjectPtr<Value>> *list;
	};

	Value() : type(TYPE::NIL), Object() {}
	Value(int x) : number(x), type(TYPE::INTEGER), Object() {}
	Value(float x) : real(x), type(TYPE::REAL), Object() {}
	Value(std::vector<ObjectPtr<Value>> *l) : list(l), type(TYPE::LIST), Object() {}
	Value(const char *s) : string(std::string(s)), type(TYPE::STRING), Object() {}
	~Value() {
		std::cout << "value(" << as_string() << "): ";
	};
	Value(const Value&v) : type(v.type){
		switch(type)
		{
		case TYPE::INTEGER:
			number = v.number;
			break;
		case TYPE::STRING:
			string = v.string;
			break;
		case TYPE::LIST:
			list = v.list;
			break;
		}
	};

	std::string as_string() const
	{
		switch(type)
		{
		case TYPE::INTEGER:
			return std::to_string(number);
		case TYPE::NIL:
			return "NIL";
		}
		return string;
	}
};

using ValuePtr = ObjectPtr<Value>;
#define CREATE_VALUE(x) ValuePtr(new Value(x))
#define NIL_VALUE ValuePtr(new Value())
using ValueList = std::vector<ValuePtr>;
using ValueListIter = ValueList::iterator;
using BuildIn = std::function<ValuePtr(ValueListIter, ValueListIter)>;
std::map<std::string, BuildIn> library;

ValuePtr eval(ValueList *s);

ValuePtr eval(ValuePtr c)
{
	if(c->type == Value::TYPE::LIST)
		return eval(c->list);
	return c;
}

ValuePtr eval(ValueList *s)
{
	assert(s->begin() != s->end());
	ValuePtr f_name = eval(*s->begin());
	const auto f_it = library.find(f_name->as_string());
	assert(f_it != library.end());
	return f_it->second(++s->begin(), s->end());
}

int main()
{
	library["print"] = [](ValueListIter pos, ValueListIter end) {
		std::string s;
		while(pos != end) s += eval(*pos++)->as_string();
		std::cout << s << std::endl;
		return NIL_VALUE;
	};

	library["+"] = [](ValueListIter pos, ValueListIter end) {
		ValuePtr v = CREATE_VALUE(0);
		while(pos != end) v->number += eval(*pos++)->number;
		return v;
	};

	ValueList sum{
		CREATE_VALUE("+"),
		CREATE_VALUE(1),
		CREATE_VALUE(2)
	};

	ValueList prog{
		CREATE_VALUE("print"),
		CREATE_VALUE("1 + 2 = "),
		CREATE_VALUE(&sum)
	};

	eval(&prog);

	return 0;
}
