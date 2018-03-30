#pragma once

#include <memory>
#include <typeindex>
using namespace std;

class object_t
{
	class bad_cast : exception
	{

	};

	struct base
	{
		virtual unique_ptr<base> clone() const = 0;
		virtual const void* get_memory() const = 0;
		virtual size_t get_memory_size() const = 0;
	};

	template<typename T>
	struct derived : base
	{
		T value;

		template<typename U>
		derived(U&& value) : value(forward<U>(value)) {}

		unique_ptr<base> clone() const override
		{
			return unique_ptr<base>(new derived<T>(value));
		}

		const void* get_memory() const override
		{
			return &value;
		}

		size_t get_memory_size() const override
		{
			return sizeof(T);
		}
	};

	unique_ptr<base> clone() const
	{
		if (!empty())
			return ptr->clone();
		else
			return nullptr;
	}

	unique_ptr<base> ptr;
	type_index index;

public:
	object_t();
	object_t(const object_t &that);
	object_t(object_t &&that);

	template<typename U, class = typename enable_if<!is_same<typename decay<U>::type, object_t>::value, U>::type>
	object_t(U&& value) : ptr(new derived<typename decay<U>::type>(forward<U>(value))), index(type_index(typeid(typename decay<U>::type))) {}

	bool empty() const;

	template<typename U>
	bool is() const
	{
		return index == type_index(typeid(U));
	}

	type_index type() const
	{
		return index;
	}

	template<typename U>
	U cast() const
	{
		if (!is<U>())
		{
			throw bad_cast();
		}
		auto d = dynamic_cast<derived<U>*> (ptr.get());
		return d->value;
	}

	long long cast_to_integral() const
	{
		if (is<char>())
			return cast<char>();
		else if (is<short>())
			return cast<short>();
		else if (is<int>())
			return cast<int>();
		else if (is<long>())
			return cast<long>();
		else if (is<long long>())
			return cast<long long>();
		else
			throw bad_cast();
	}

	object_t& operator= (const object_t& b)
	{
		if (ptr != b.ptr)
		{
			ptr = b.clone();
			index = b.index;
		}
		return *this;
	}

	const void* get_raw_memory() const;

	size_t get_raw_memory_size() const;
};

