#pragma once

#include <string>
#include <memory>

namespace compiler
{
	using namespace std;

	class type_base;

	typedef shared_ptr<type_base> type_base_ptr;

	class type_base
	{
	public:
		type_base(const string &name, const string &id);

		const string id;
		const string name;

		virtual bool is_assignable_from(type_base_ptr b) const = 0;
		virtual bool can_be_cast_to(type_base_ptr to_type) const = 0;
		virtual bool is_primitive() const;
		virtual bool is_integral() const;
		virtual bool is_void() const;
		virtual bool is_pointer() const;
		virtual bool is_array() const;
		virtual type_base_ptr implicit_cast_with(type_base_ptr a, type_base_ptr b) const = 0;
		virtual size_t size() const = 0;

		virtual bool operator== (const type_base &b) const;
		virtual bool operator!= (const type_base &b) const;
	};

	struct type_representation
	{
		type_base_ptr base_type;
		bool is_const;
		bool is_static;
		bool is_built_in = false;
		bool is_lvalue = false;

		type_representation(type_base_ptr base_type, bool is_const, bool is_static);

		type_representation(type_base_ptr base_type);

		type_representation();

		string to_string();
	};
}

