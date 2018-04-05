#pragma once

#include <vector>
#include "type_base.h"

namespace compiler
{
	class type_array : public type_base
	{
		size_t sz;
		type_base_ptr base_type;
		size_t dim;
	public:
		type_array(type_base_ptr base_type, size_t dim);

		bool is_assignable_from(type_base_ptr b) const override;
		bool can_be_cast_to(type_base_ptr to_type) const override;
		bool is_primitive() const override;
		bool is_pointer() const override;
		bool is_array() const override;
		type_base_ptr get_base_type();
		type_base_ptr implicit_cast_with(type_base_ptr a, type_base_ptr b) const;
		size_t size() const override;
		size_t get_this_dimension() const;
		type_base_ptr to_pointer() const;
	};

	type_base_ptr create_array_type(type_base_ptr element_type, const vector<size_t> & dim);

	type_representation to_pointer(type_representation array_type);
}
