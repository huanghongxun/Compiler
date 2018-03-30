#include "object_t.h"

object_t::object_t() : ptr(nullptr), index(type_index(typeid(void))) {}

object_t::object_t(const object_t & that) : ptr(that.clone()), index(that.index) {}

object_t::object_t(object_t && that) : ptr(move(that.ptr)), index(that.index) {}

bool object_t::empty() const
{
	return ptr == nullptr;
}

const void * object_t::get_raw_memory() const
{
	if (empty()) return nullptr;
	else return ptr->get_memory();
}

size_t object_t::get_raw_memory_size() const
{
	return empty() ? 0 : ptr->get_memory_size();
}
