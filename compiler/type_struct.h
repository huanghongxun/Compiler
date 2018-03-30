#pragma once

#include <vector>
#include <utility>
#include <memory>
#include "type_base.h"

namespace compiler
{
	using namespace std;

	class type_struct : public type_base
	{
		size_t sz;
		vector<pair<size_t, shared_ptr<type_base>>> fields;

	public:
		type_struct(const string &id);
		const vector<pair<size_t, shared_ptr<type_base>>> &get_fields() const;
		void add_field(type_base_ptr type);

		bool is_assignable_from(type_base_ptr b) const override;
		bool can_be_cast_to(type_base_ptr to_type) const override;
		bool is_primitive() const override;
		size_t size() const override;
	};
}
