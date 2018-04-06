#include "syntax_tree_evaluation.h"
#include "syntax_descriptors.h"
#include "unit_assert.h"
#include "type_primitive.h"
#include "string_utils.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace compiler;
using namespace compiler::syntax;

template<typename T>
object_t eval_binary_common(const string &op, const object_t &lhs, const object_t &rhs)
{
	T l = lhs.cast<T>(), r = rhs.cast<T>();
	if (op == "*")
		return l * r;
	else if (op == "+")
		return l + r;
	else if (op == "-")
		return l - r;
	else if (op == "/")
		return l / r;
	else if (op == "==")
		return l == r;
	else if (op == "!=")
		return l != r;
	else if (op == ">")
		return l > r;
	else if (op == "<")
		return l < r;
	else if (op == "<=")
		return l <= r;
	else if (op == ">=")
		return l >= r;
	else
		return object_t();
}

template<typename T>
object_t eval_binary_integral(const string &op, const object_t &lhs, const object_t &rhs)
{
	T l = lhs.cast<T>(), r = rhs.cast<T>();
	auto last_res = eval_binary_common<T>(op, lhs, rhs);
	if (!last_res.empty())
		return last_res;
	else if (op == "<<")
		return l << r;
	else if (op == ">>")
		return l >> r;
	else if (op == "&&")
		return l && r;
	else if (op == "||")
		return l || r;
	else if (op == "^")
		return l ^ r;
	else if (op == "|")
		return l | r;
	else if (op == "&")
		return l & r;
	else if (op == "%")
		return l % r;
	else
		return object_t();
}

object_t eval_binary_bool(const string &op, const object_t &lhs, const object_t &rhs)
{
	bool l = lhs.cast<bool>(), r = rhs.cast<bool>();
	if (op == "&&")
		return l && r;
	else if (op == "||")
		return l || r;
	else if (op == "^")
		return l ^ r;
	else if (op == "|")
		return l | r;
	else if (op == "&")
		return l & r;
	else
		return object_t();
}

template<typename T>
object_t build_unary_common(const string &op, const object_t &obj)
{
	T val = obj.cast<T>();
	if (op == "-")
		return -val;
	else
		return object_t();
}

template<typename T>
object_t build_unary_integral(const string &op, const object_t &obj)
{
	T val = obj.cast<T>();
	auto res = build_unary_common<T>(op, obj);
	if (!res.empty())
		return res;
	else  if (op == "!")
		return !val;
	else if (op == "~")
		return ~val;
	else
		return object_t();
}

object_t build_unary_bool(const string &op, const object_t &obj)
{
	bool val = obj.cast<bool>();
	if (op == "!")
		return !val;
	else
		return object_t();
}

template<typename T>
object_t cast_primitive0(object_t obj, type_base_ptr from_type)
{
	string from = from_type->id;
	if (from == type_int->id)
		return (T)obj.cast<int>();
	else if (from == type_float->id)
		return (T)obj.cast<float>();
	else if (from == type_double->id)
		return (T)obj.cast<double>();
	else if (from == type_long_double->id)
		return (T)obj.cast<long double>();
	else if (from == type_long->id)
		return (T)obj.cast<long>();
	else if (from == type_long_long->id)
		return (T)obj.cast<long long>();
	else if (from == type_bool->id)
		return (T)obj.cast<bool>();
	else if (from == type_char->id)
		return (T)obj.cast<char>();
	else if (from == type_short->id)
		return (T)obj.cast<short>();
	else
		throw std::runtime_error(string_format("primitive cast does not support type %s", from.c_str()));
}

object_t cast_primitive(object_t from, type_base_ptr from_type, type_base_ptr to_type)
{
	string to = to_type->id;
	if (to == type_int->id)
		return cast_primitive0<int>(from, from_type);
	else if (to == type_float->id)
		return cast_primitive0<float>(from, from_type);
	else if (to == type_double->id)
		return cast_primitive0<double>(from, from_type);
	else if (to == type_long_double->id)
		return cast_primitive0<long double>(from, from_type);
	else if (to == type_long->id)
		return cast_primitive0<long>(from, from_type);
	else if (to == type_long_long->id)
		return cast_primitive0<long long>(from, from_type);
	else if (to == type_bool->id)
		return cast_primitive0<bool>(from, from_type);
	else if (to == type_char->id)
		return cast_primitive0<char>(from, from_type);
	else if (to == type_short->id)
		return cast_primitive0<short>(from, from_type);
	else
		throw std::runtime_error(string_format("primitive cast does not support type %s", to.c_str()));
}

object_t compiler::evaulate_expression(AST ast)
{
	if (ast->desc.type() == typeid(descriptor_binary_operator))
	{
		auto desc = any_cast<descriptor_binary_operator>(ast->desc);

		assert_eq(ast->children.size(), 2);

		auto lhs = evaulate_expression(ast->children[0]);
		auto rhs = evaulate_expression(ast->children[1]);

		auto type_name = ast->children[0]->type.base_type->id;
		assert_eq(ast->children[0]->type.base_type->id, ast->children[1]->type.base_type->id);


		object_t res;
		if (type_name == type_int->id)
			res = eval_binary_integral<int>(desc.op, lhs, rhs);
		else if (type_name == type_short->id)
			res = eval_binary_integral<short>(desc.op, lhs, rhs);
		else if (type_name == type_long->id)
			res = eval_binary_integral<long>(desc.op, lhs, rhs);
		else if (type_name == type_long_long->id)
			res = eval_binary_integral<long long>(desc.op, lhs, rhs);
		else if (type_name == type_float->id)
			res = eval_binary_common<float>(desc.op, lhs, rhs);
		else if (type_name == type_double->id)
			res = eval_binary_common<double>(desc.op, lhs, rhs);
		else if (type_name == type_long_double->id)
			res = eval_binary_common<long double>(desc.op, lhs, rhs);
		else if (type_name == type_bool->id)
			res = eval_binary_bool(desc.op, lhs, rhs);

		if (res.empty())
			throw expression_evaulation_error();

		return res;
	}
	else if (ast->desc.type() == typeid(descriptor_unary_operator))
	{
		auto desc = any_cast<descriptor_unary_operator>(ast->desc);

		assert_eq(ast->children.size(), 1);

		object_t val = evaulate_expression(ast->children[0]);

		auto type_name = ast->children[0]->type.base_type->id;

		object_t res;
		if (type_name == type_int->id)
			res = build_unary_integral<int>(desc.op, val);
		else if (type_name == type_short->id)
			res = build_unary_integral<short>(desc.op, val);
		else if (type_name == type_long->id)
			res = build_unary_integral<long>(desc.op, val);
		else if (type_name == type_long_long->id)
			res = build_unary_integral<long long>(desc.op, val);
		else if (type_name == type_bool->id)
			res = build_unary_bool(desc.op, val);
		else if (type_name == type_float->id)
			res = build_unary_common<float>(desc.op, val);
		else if (type_name == type_double->id)
			res = build_unary_common<double>(desc.op, val);
		else if (type_name == type_long_double->id)
			res = build_unary_common<long double>(desc.op, val);

		if (res.empty())
			throw expression_evaulation_error();

		return res;
	}
	else if (ast->desc.type() == typeid(descriptor_constant))
	{
		auto desc = any_cast<descriptor_constant>(ast->desc);

		if (boost::starts_with(desc.value, "'")) // a char
		{
			auto chars = parse_constant_string(desc.value.substr(1, desc.value.size() - 2));
			if (chars.size() != 1)
				throw expression_evaulation_error();
			return chars[0];
		}
		else if (desc.value == "true")
		{
			return 1;
		}
		else if (desc.value == "false")
		{
			return 0;
		}
		else
		{
			try
			{
				boost::to_lower(desc.value);
				if (desc.value.find("e") != string::npos)
				{
					vector<string> strs;
					boost::split(strs, desc.value, boost::is_any_of("e"));
					assert_eq(strs.size(), 2);
					int e = boost::lexical_cast<int>(strs[1]);
					double m = boost::lexical_cast<double>(strs[0]);
					return m * pow(10, e);
				}
				else if (desc.value.find(".") == string::npos)
				{
					if (boost::starts_with(desc.value, "0x"))
					{
						if (boost::ends_with(desc.value, "ll"))
							return boost::lexical_cast<lexical_hex<long long>>(desc.value.substr(0, desc.value.size() - 2));
						else if (boost::ends_with(desc.value, "l"))
							return boost::lexical_cast<lexical_hex<long>>(desc.value.substr(0, desc.value.size() - 1));
						else
							return boost::lexical_cast<lexical_hex<int>>(desc.value.substr(0, desc.value.size()));
					}
					else if (boost::starts_with(desc.value, "0"))
					{
						if (boost::ends_with(desc.value, "ll"))
							return boost::lexical_cast<lexical_oct<long long>>(desc.value.substr(0, desc.value.size() - 2));
						else if (boost::ends_with(desc.value, "l"))
							return boost::lexical_cast<lexical_oct<long>>(desc.value.substr(0, desc.value.size() - 1));
						else
							return boost::lexical_cast<lexical_oct<int>>(desc.value.substr(0, desc.value.size()));
					}
					else
					{
						if (boost::ends_with(desc.value, "ll"))
							return boost::lexical_cast<long long>(desc.value.substr(0, desc.value.size() - 2));
						else if (boost::ends_with(desc.value, "l"))
							return boost::lexical_cast<long>(desc.value.substr(0, desc.value.size() - 1));
						else
							return boost::lexical_cast<int>(desc.value.substr(0, desc.value.size()));
					}
				}
				else
				{
					if (boost::ends_with(desc.value, "l"))
						return boost::lexical_cast<long double>(desc.value.substr(0, desc.value.size() - 1));
					else if (boost::ends_with(desc.value, "f"))
						return boost::lexical_cast<float>(desc.value.substr(0, desc.value.size() - 1));
					else
						return boost::lexical_cast<double>(desc.value.substr(0, desc.value.size()));
				}
			}
			catch (boost::bad_lexical_cast&)
			{
				throw expression_evaulation_error();
			}
		}
		throw expression_evaulation_error();
	}
	else if (ast->desc.type() == typeid(descriptor_primitive_cast))
	{
		auto desc = any_cast<descriptor_primitive_cast>(ast->desc);
		assert_eq(ast->children.size(), 1);
		auto res = evaulate_expression(ast->children[0]);
		return cast_primitive(res, desc.from_type.base_type, desc.to_type.base_type);
	}
	else
		throw expression_evaulation_error();
}
