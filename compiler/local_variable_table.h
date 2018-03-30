#pragma once

#include <memory>
#include <vector>
#include "string_utils.h"
using namespace std;

namespace compiler
{

	class local_variable_table
	{
		size_t size;
		shared_ptr<void> pool;
	public:
		local_variable_table(size_t size);
		~local_variable_table();

		template<typename T>
		void set(int index, const T* const val)
		{
			set(index, val, sizeof(T));
		}

		virtual void set(int index, const void *const val, size_t sz)
		{
			if (index < 0 || index + sz > size)
				throw out_of_range(string_format("Invalid local variable index %d", index));
			memcpy(reinterpret_cast<char*>(pool.get()) + index, val, sz);
		}

		template<typename T>
		T* get(int index)
		{
			size_t sz = sizeof(T);
			if (index < 0 || index + sz > size)
				throw out_of_range(string_format("Invalid local variable index %d", index));
			return reinterpret_cast<T*>(reinterpret_cast<char*>(pool.get()) + index);
		}


		
	};
}

