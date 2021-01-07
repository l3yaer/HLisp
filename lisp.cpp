#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <cstring>
#include <cstdio>
#include <map>

std::string str(double n)
{
	std::ostringstream os; os << n;
	return os.str();
}

#define DEBUG_MEM 0

struct MemObj
{
	union
	{
		intptr_t addr = 0;
		void *ptr;
	};
};

class ObjectManager
{
public:
	std::map<intptr_t, size_t> mem;
	MemObj alloc(size_t size)
	{
		void *mem = malloc(size);
		MemObj obj;
		obj.addr = (intptr_t) mem;
		add(obj);
		return obj;
	}

	void free_mem(MemObj obj)
	{
		remove(obj);
		if(peek(obj) == 0 && obj.ptr != nullptr)
			free(obj.ptr);
	}

	void add(MemObj obj)
	{
		if(mem.find(obj.addr) == mem.end())
			mem[obj.addr] = 0;

		mem[obj.addr]++;
#if DEBUG_MEM
		std::cout << "obj added: " << obj.addr << " " << obj.ptr << " | ref count: " << mem[obj.addr]  << std::endl;
#endif
	}

	void remove(MemObj obj)
	{
		if(obj.addr == 0)
			return;

		mem[obj.addr]--;
#if DEBUG_MEM
		std::cout << "obj removed: " << obj.addr << " | ref count: " << mem[obj.addr]  << std::endl;
#endif
		if(mem[obj.addr] > 0)
			return;
		mem.erase(obj.addr);
	}

	size_t peek(MemObj obj)
	{
		if(mem.find(obj.addr) == mem.end())
			return -1;
		return mem[obj.addr];
	}
};

static ObjectManager object_manager;

enum Type
{
	SYMBOL,
	NUMBER,
	LIST,
	FUNC,
	LAMBDA,
	NIL
};

struct Env;
struct Value;
struct String
{
	MemObj mem;
	char *value = nullptr;
	size_t size = 0;
};

String create_string(const std::string &string)
{
	String new_string;
	new_string.size = string.size();
	new_string.mem = object_manager.alloc(sizeof(char) * (new_string.size + 1));
	new_string.value = (char *) new_string.mem.ptr;
	snprintf(new_string.value, new_string.size + 1, "%s", string.c_str());
	return new_string;
}

void destroy_string(String *string)
{
	object_manager.free_mem(string->mem);
#if DEBUG_MEM
	if(object_manager.peek(string->mem) == 0 && string->mem.addr)
		std::cout << "obj freed: " << string->mem.addr << std::endl;
#endif
	string->size = 0;
	string->value = nullptr;
}

void string_copy(String *dest, const String *src)
{
	destroy_string(dest);
	dest->size = src->size;
	dest->mem = src->mem;
	object_manager.add(dest->mem);
	dest->value = src->value;
}

String substring(const String *src, size_t start, size_t end)
{
	String new_string;
	if(end <= start)
		return new_string;

	new_string.size = end - start;
	new_string.mem = src->mem;
	object_manager.add(new_string.mem);
	new_string.value = src->value + start;
	return new_string;
}

struct Array
{
	MemObj mem;
	Value *content = nullptr;
	size_t size = 0;
	size_t capacity = 0;
};

void destroy_array(Array *array)
{
	object_manager.free_mem(array->mem);
#if DEBUG_MEM
	if(object_manager.peek(array->mem) == 0 && array->mem.addr)
		std::cout << "obj freed: " << array->mem.addr << std::endl;
#endif
	array->content = nullptr;
	array->size = 0;
	array->capacity = 0;
}

void array_copy(Array *dest, const Array *src);
Array array_make_from_vector(const std::vector<Value> &array);

struct Value
{
	typedef Value (*Func)(const Array &, Env *);
	typedef std::vector<Value>::const_iterator ValueIter;
	typedef std::map<std::string, Value> Map;

	Type type;
	union
	{
		String string;
		Array list;
		Func func;
		double number;
	};
	Env *env;

	Value(Type type = SYMBOL) : type(type), string(String()), env(nullptr) {}
	Value(Type type, const std::string &val) : type(type), string(create_string(val)), env(nullptr) {}
	Value(const String &val) : type(SYMBOL), env(nullptr)
	{
		string_copy(&string, &val);
	}
	Value(const Array &val) : type(LIST), env(nullptr)
	{
		array_copy(&list, &val);
	}
	Value(Func fn) : type(FUNC), func(fn), env(nullptr) {}
	Value(double x) : type(NUMBER), number(x), env(nullptr) {}
	Value(const std::vector<Value> &list) : type(LIST), list(array_make_from_vector(list)), env(nullptr) {}
	Value(const Value &val) : type(NIL)
	{
		copy_from(val);
	}
	~Value()
	{
		clear();
	}

	Value operator=(const Value &val)
	{
		clear();
		copy_from(val);
		return *this;
	}

	double number_value() const
	{
		if(type == NUMBER)
			return number;
		if(type == SYMBOL)
			return atof(string.value);
		return 0.0f;
	}

	std::string string_value() const
	{
		switch(type)
		{
		case SYMBOL:
			return std::string(string.value).substr(0, string.size);
		case NUMBER:
			return str(number);
		case FUNC:
			return "<FUNC>";
		case LIST:
			return print_list();
		case LAMBDA:
			return "<LAMBDA>";
		}

		return "<TYPE ERROR>";
	}
private:
	std::string print_list() const
	{
		std::string s("(");
		for(int i = 0; i < list.size; ++i)
		{
			s += list.content[i].string_value();
			if(i < list.size - 1)
				s += " ";
		}
		s += ")";
		return s;
	}

	void copy_from(const Value &val)
	{
		env = val.env;
		switch(val.type)
		{
		case SYMBOL:
			if(val.type != type)
				string = String();
			string_copy(&string, &val.string);
			break;
		case NUMBER:
			number = val.number;
			break;
		case FUNC:
			func = val.func;
			break;
		case LIST:
		case LAMBDA:
			if(val.type != type)
				list = Array();
			array_copy(&list, &val.list);
			break;
		}
		type = val.type;
	}

	void clear()
	{
		if(type == SYMBOL)
			destroy_string(&string);
		else if(type == LAMBDA || type == LIST)
			destroy_array(&list);
	}
};

#define value_make_list(x) Value(std::vector<Value>(x))

void array_copy(Array *dest, const Array *src)
{
	destroy_array(dest);
	dest->content = src->content;
	dest->size = src->size;
	dest->mem = src->mem;
	object_manager.add(dest->mem);
	dest->capacity = src->capacity;
}

void array_add(Array *array, Value item)
{
	if(object_manager.peek(array->mem) > 1)
	{
		object_manager.remove(array->mem);
		size_t new_capacity = array->capacity + (array->size >= array->capacity ? 128 : 0);
		Value *new_content = (Value *) malloc(sizeof(Value) * new_capacity);
		if(new_content == nullptr)
			return;

		memcpy(new_content, array->content, sizeof(Value) * array->capacity);
		array->capacity = new_capacity;
		array->content = new_content;
		array->mem.addr = (intptr_t) new_content;
		object_manager.add(array->mem);
	}
	else if(array->size >= array->capacity)
	{
		object_manager.remove(array->mem);
		size_t new_capacity = array->capacity + 128;
		Value *new_content = (Value *) realloc(array->content, sizeof(Value) * new_capacity);

		if(new_content == nullptr)
			return;

		array->capacity = new_capacity;
		array->content = new_content;
		array->mem.addr = (intptr_t) new_content;
		object_manager.add(array->mem);
	}

	array->content[array->size++] = item;
}

Array array_make_from_vector(const std::vector<Value> &array)
{
	Array new_array;
	for(auto item : array)
		array_add(&new_array, item);
	return new_array;
}

void array_for_each(Array *array, void (*fn) (Value*))
{
	for(size_t i = 0; i < array->size; ++i)
		fn(&array->content[i]);
}

Array array_slice(const Array *src, size_t start, size_t end)
{
	Array new_array;
	new_array.content = src->content + start;
	new_array.size = end - start;
	new_array.capacity = src->capacity - start;
	new_array.mem = src->mem;
	object_manager.add(new_array.mem);
	return new_array;
}

const Value false_symbol(SYMBOL, "false");
const Value true_symbol(SYMBOL, "true");
const Value nil(SYMBOL, "nil");

Value array_get(const Array *array, size_t index)
{
	if(index >= array->size)
		return nil;

	return array->content[index];
}
struct Env
{
	typedef std::map<std::string, Value> Map;
	Env(Env *outer = nullptr) : outer(outer) {}

	Env(const Array &params, const Array &args, Env *outer = nullptr) : outer(outer)
	{
		for(size_t i = 0; i < params.size; ++i)
			map[array_get(&params, i).string_value()] = array_get(&args, i);
	}

	Map &find(const std::string &name)
	{
		if(map.find(name) != map.end())
			return map;
		else if(outer != nullptr)
			return outer->find(name);

		std::cout << name << ": NOT FOUND" << std::endl;
		exit(1);
	}

	Value &operator[](const std::string &name)
	{
		return map[name];
	}
private:
	Map map;
	Env *outer;
};

Value eval(Value x, Env *env);

Value fn_add(const Array &val, Env *env)
{
	double acc = val.content[0].number_value();
	for(int i = 1; i < val.size; ++i)
		acc += val.content[i].number_value();
	return Value(acc);
}

Value fn_sub(const Array &val, Env *env)
{
	double acc = val.content[0].number_value();
	for(int i = 1; i < val.size; ++i)
		acc -= val.content[i].number_value();
	return Value(acc);
}

Value fn_mul(const Array &val, Env *env)
{
	double acc = val.content[0].number_value();
	for(int i = 1; i < val.size; ++i)
		acc *= val.content[i].number_value();
	return Value(acc);
}

Value fn_div(const Array &val, Env *env)
{
	double acc = val.content[0].number_value();
	for(int i = 1; i < val.size; ++i)
		acc /= val.content[i].number_value();
	return Value(acc);
}

Value fn_greater(const Array &val, Env *env)
{
	double n = val.content[0].number_value();
	for(int i = 1; i < val.size; ++i)
		if(n <= val.content[i].number_value())
			return false_symbol;
	return true_symbol;
}

Value fn_less(const Array &val, Env *env)
{
	double n = val.content[0].number_value();
	for(int i = 1; i < val.size; ++i)
		if(n >= val.content[i].number_value())
			return false_symbol;
	return true_symbol;
}

Value fn_nullp(const Array &val, Env *env)
{
	if(array_get(&val, 0).type == SYMBOL)
		return array_get(&val, 0).string.size > 0 ? true_symbol : false_symbol;
	if(array_get(&val, 0).type == LAMBDA || array_get(&val, 0).type == LIST)
		return array_get(&val, 0).list.size > 0 ? true_symbol : false_symbol;
	return false_symbol;
}

Value fn_size(const Array &val, Env *env)
{
	if(array_get(&val, 0).type == SYMBOL)
		return array_get(&val, 0).string.size > 0 ? true_symbol : false_symbol;
	if(array_get(&val, 0).type == LAMBDA || array_get(&val, 0).type == LIST)
		return Value(array_get(&val, 0).list.size);
	return nil;
}

Value fn_car(const Array &val, Env *env)
{
	Value first = array_get(&val, 0);
	if(first.type == SYMBOL)
		return Value(substring(&first.string, 0, 1));
	if(first.type == LAMBDA || first.type == LIST)
		return first.list.content[0];
	return nil;
}

Value fn_cdr(const Array &val, Env *env)
{
	Value first = array_get(&val, 0);
	if(first.type == SYMBOL)
		return Value(substring(&first.string, 1, first.string.size));
	if(first.type == LAMBDA || first.type == LIST)
		return Value(array_slice(&first.list, 1, first.list.size));
	return nil;
}

Value fn_cons(const Array &val, Env *env)
{
	Value result = value_make_list({});
	array_add(&result.list, array_get(&val, 0));
	Value second = array_get(&val, 1);
	for(int i = 0; i < second.list.size; ++i)
		array_add(&result.list, array_get(&second.list, i));
	return result;;
}

Value fn_append(const Array &val, Env *env)
{
	Value first = array_get(&val, 0);
	Value second = array_get(&val, 1);
	Value result(first.list);
	for(int i = 0; i < second.list.size; ++i)
		array_add(&result.list, array_get(&second.list, i));
	return result;;
}

Value fn_list(const Array &val, Env *env)
{
	return Value(val);
}

Value native_def(const Value &val, Env *env)
{
	(*env)[array_get(&val.list, 1).string_value()] = eval(array_get(&val.list, 2), env);
	return nil;
}

Value native_if(const Value &val, Env *env)
{
	if(eval(array_get(&val.list, 1), env).string_value() == false_symbol.string_value())
		return eval(array_get(&val.list, 3), env);
	else
		return eval(array_get(&val.list, 2), env);
}

Value native_quote(const Value &val, Env *env)
{
	return array_get(&val.list, 1);
}

Value fn_quit(const Array &val, Env *env)
{
	exit(0);
	return nil;
}


void create_global_env(Env &env)
{
	env["nil"] = nil;
	env["false"] = false_symbol;
	env["true"] = true_symbol;
	env["+"] = Value(&fn_add);
	env["-"] = Value(&fn_sub);
	env["*"] = Value(&fn_mul);
	env["/"] = Value(&fn_div);
	env[">"] = Value(&fn_greater);
	env["<"] = Value(&fn_less);
	env["nil?"] = Value(&fn_nullp);
	env["size"] = Value(&fn_size);
	env["car"] = Value(&fn_car);
	env["cdr"] = Value(&fn_cdr);
	env["cons"] = Value(&fn_cons);
	env["append"] = Value(&fn_append);
	env["list"] = Value(&fn_list);
	env["quit"] = Value(&fn_quit);
}

Value eval(Value x, Env *env)
{
	if(x.type == SYMBOL)
		return env->find(x.string_value())[x.string_value()];
	if(x.type == NUMBER)
		return x;
	if(x.type == LIST)
	{
		if(x.list.size == 0)
			return x;

		Value func = array_get(&x.list, 0);
		if(func.string_value() == "quote")
			return native_quote(x, env);
		if(func.string_value() == "def")
			return native_def(x, env);
		if(func.string_value() == "if")
			return native_if(x, env);

	}
	Value func(eval(array_get(&x.list, 0), env));
	Array exps;
	for(size_t i = 1; i < x.list.size; ++i)
		array_add(&exps, eval(array_get(&x.list, i), env));

	if(func.type == FUNC)
		return func.func(exps, env);

	return nil;
}

std::list<std::string> tokenize(const std::string &str)
{
	std::list<std::string> tokens;
	const char *s = str.c_str();
	while(*s)
	{
		while(*s == ' ')
			++s;
		if(*s == '(' || *s == ')')
			tokens.push_back(*s++ == '(' ? "(" : ")");
		else
		{
			const char *t = s;
			while(*t && *t != ' ' && *t != '(' && *t != ')')
				++t;
			tokens.push_back(std::string(s, t));
			s = t;
		}
	}
	return tokens;
}

bool is_digit(char c)
{
	return isdigit(static_cast<unsigned char>(c)) != 0;
}

Value atom(const std::string &token)
{
	if(is_digit(token[0]) || (token[0] == '-' && is_digit(token[1])))
	   return Value(atof(token.c_str()));
	return Value(SYMBOL, token);
}

Value read_tokens(std::list<std::string> &tokens)
{
	const std::string token(tokens.front());
	tokens.pop_front();
	if(token == "(")
	{
		Value c = value_make_list({});
		while(tokens.front() != ")")
			array_add(&c.list, read_tokens(tokens));
		tokens.pop_front();
		return c;
	} else
		return atom(token);
}

Value read_expr(const std::string expr)
{
	std::list<std::string> tokens = tokenize(expr);
	return read_tokens(tokens);
}

int main()
{
	Env global;
	create_global_env(global);
	while(1)
	{
		std::cout << "hlisp>";
		std::string line;
		std::getline(std::cin, line);
		std::cout << eval(read_expr(line), &global).string_value() << std::endl;
	}
	return 0;
}
