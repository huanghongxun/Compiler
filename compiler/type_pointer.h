#pragma once

#include "type_base.h"

namespace compiler
{
	using namespace std;

	class type_pointer : public type_base
	{
		bool m_is_const;
		type_base_ptr base_type;
	public:
		type_pointer(type_base_ptr base_type, bool is_const);

		bool is_assignable_from(type_base_ptr b) const override;
		bool can_be_cast_to(type_base_ptr to_type) const override;
		bool is_primitive() const override;
		bool is_pointer() const override;
		bool is_const() const;
		type_base_ptr get_base_type();
		type_base_ptr implicit_cast_with(type_base_ptr a, type_base_ptr b) const;
		size_t size() const override;
	};

	type_representation wrap_pointer(type_representation type);

	type_representation unwrap_pointer(type_representation type);
}
