#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

class Object
{
public:
	Object() : ref_count(0) {}
	virtual ~Object(){}

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
	ObjectPtr() : object(nullptr) {}
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

	operator bool () const
	{
		return object != nullptr;
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

class Value;
class Env;
class ValueList;
typedef ObjectPtr<Value> ValuePtr;
typedef ObjectPtr<Env> EnvPtr;
#define CREATE_VALUE(x) ValuePtr(new Value(x))
#define NIL_VALUE ValuePtr(new Value())
#define IS_NIL(v) (v)->type == Value::TYPE::NIL
using ValueListIter = std::vector<ValuePtr>::iterator;
typedef ObjectPtr<ValueList> ValueListPtr;
typedef std::function<ValuePtr(EnvPtr)> Native;
typedef std::unordered_map<std::string, ValuePtr> BindsMap;

struct Arg
{
	std::string name;
	bool varg = false;
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
		ValueListPtr list;
		Native native;
	};

	Value() : type(TYPE::NIL), Object() {}
	Value(int x) : number(x), type(TYPE::INTEGER), Object() {}
	Value(float x) : real(x), type(TYPE::REAL), Object() {}
	Value(ValueListPtr l) : list(l), type(TYPE::LIST), Object() {}
	Value(const char *s) : string(std::string(s)), type(TYPE::STRING), Object() {}
	Value(Native l) : native(l), type(TYPE::NATIVE), Object() {}
	~Value() {};
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
		case TYPE::NATIVE:
			native = v.native;
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

class ValueList : public Object
{
public:
	ValueList() : Object() {}
	ValueList(std::initializer_list<ValuePtr> v) : content(v.begin(), v.end()), Object() {}
	ValueList(ValueListIter begin, ValueListIter end) : content(begin, end), Object() {}

	std::vector<ValuePtr> *operator->()
	{
		return &content;
	}

	std::vector<ValuePtr> *raw()
	{
		return &content;
	}

	ValueListIter begin()
	{
		return content.begin();
	}

	ValueListIter end()
	{
		return content.end();
	}

private:
	std::vector<ValuePtr> content;
};

class Env : public Object
{
public:
	Env() : parent(EnvPtr()) , Object() {}

	Env(EnvPtr parent) : parent(parent), Object() {}

	BindsMap binds;
	EnvPtr parent;

	ValuePtr get_value(const std::string &symbol)
	{
		BindsMap::iterator it = binds.find(symbol);
		if(it != binds.end())
			return it->second;
		if(parent)
			return parent->get_value(symbol);
		return NIL_VALUE;
	}

	void set_value(const std::string &symbol, const ValuePtr &value)
	{
		binds[symbol] = value;
	}
};

std::map<std::string, Native> library;

ValuePtr eval(ValueListPtr s, EnvPtr env);

ValuePtr eval(ValuePtr c, EnvPtr env)
{
	if(c->type == Value::TYPE::LIST)
		return eval(c->list, env);
	return c;
}

ValuePtr eval(ValueListPtr s, EnvPtr env)
{
	assert(s->begin() != s->end());
	ValuePtr f_name = eval(*s->begin(), env);
	const auto f_it = env->get_value(f_name->as_string());
	assert(!IS_NIL(f_it));
	ValueListIter pos = s->begin();
	ValueListPtr l(new ValueList{++pos, s->end()});
	EnvPtr f_env(new Env(env));
	f_env->set_value("args", CREATE_VALUE(l));
	return f_it->native(f_env);
}

int main()
{
	EnvPtr root(new Env());
	Native print_f = [](EnvPtr env) {
		const ValuePtr args = env->get_value("args");
		ValueListPtr argl = args->list;
		ValueListIter it = argl->begin();
		std::string s;
		while(it != argl->end()) s += eval(*it++, env)->as_string();
		std::cout << s << std::endl;
		return NIL_VALUE;
	};
	root->set_value("print", CREATE_VALUE(print_f));

	Native sum_f = [](EnvPtr env) {
		const ValuePtr args = env->get_value("args");
		ValueListPtr argl = args->list;
		ValueListIter it = argl->begin();
		ValuePtr v = CREATE_VALUE(0);
		while(it != argl->end()) v->number += eval(*it++, env)->number;
		return v;
	};
	root->set_value("+", CREATE_VALUE(sum_f));

	ValueListPtr sum(new ValueList{
		CREATE_VALUE("+"),
		CREATE_VALUE(1),
		CREATE_VALUE(2)
		});

	ValueListPtr prog(new ValueList{
		CREATE_VALUE("print"),
		CREATE_VALUE("1 + 2 = "),
		CREATE_VALUE(sum)
		});

	eval(prog, root);

	return 0;
}
