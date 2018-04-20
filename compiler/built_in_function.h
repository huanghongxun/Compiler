#pragma once

#include <vector>
#include "object_t.h"

namespace compiler
{
	class built_in_function
	{
	public:
		virtual object_t call(const std::vector<object_t> &vec) = 0;

	};

	template<typename R, typename A>
	class built_in_function_1 : public built_in_function
	{
		R(*func)(A);

	public:
		built_in_function_1(R(*func)(A)) : func(func) {}

		object_t call(const std::vector<object_t> &vec) override
		{
			if (vec.size() != 1)
				throw invalid_argument("This function require one argument.");
			return call_fixed(vec[0]);
		}

		virtual object_t call_fixed(const object_t &a)
		{
			return func(a.cast<A>());
		}
	};

	template<typename R, typename A>
	shared_ptr<built_in_function> create_function(R(*func)(A))
	{
		return shared_ptr<built_in_function>(new built_in_function_1<R, A>(func));
	}

	template<typename R, typename A, typename Cast>
	struct built_in_function_1_cast : public built_in_function_1<R, Cast>
	{
		built_in_function_1_cast(R(*func)(A))
			: built_in_function_1([](A c) { return func((Cast)c); })
		{}
	};

	template<typename A>
	class built_in_function_1<void, A> : public built_in_function
	{
		void(*func)(A);

	public:
		built_in_function_1(void(*func)(A)) : func(func) {}

		object_t call(const std::vector<object_t> &vec) override
		{
			if (vec.size() != 1)
				throw invalid_argument("This function require one argument.");
			return call_fixed(vec[0]);
		}

		virtual object_t call_fixed(const object_t &a)
		{
			func(a.cast<A>());
			return object_t();
		}
	};

	template<typename R, typename A, typename B>
	class built_in_function_2 : public built_in_function
	{
		R(*func)(A, B);

	public:
		built_in_function_2(R(*func)(A, B)) : func(func) {}

		object_t call(const std::vector<object_t> &vec)
		{
			if (vec.size() != 2)
				throw invalid_argument("This function require 2 arguments.");
			return call_fixed(vec[0], vec[1]);
		}

		virtual object_t call_fixed(const object_t &a, const object_t &b)
		{
			if (typeid(R) == typeid(void))
			{
				func(a.cast<A>(), b.cast<B>());
				return object_t();
			}
			else
				return func(a.cast<A>(), b.cast<B>());
		}
	};

	template<typename R, typename A, typename B>
	shared_ptr<built_in_function> create_function(R(*func)(A, B))
	{
		return shared_ptr<built_in_function>(new built_in_function_2<R, A, B>(func));
	}

	template<typename A, typename B>
	class built_in_function_2<void, A, B> : public built_in_function
	{
		void(*func)(A, B);

	public:
		built_in_function_2(void(*func)(A, B)) : func(func) {}

		object_t call(const std::vector<object_t> &vec)
		{
			if (vec.size() != 2)
				throw invalid_argument("This function require 2 arguments.");
			return call_fixed(vec[0], vec[1]);
		}

		virtual object_t call_fixed(const object_t &a, const object_t &b)
		{
			func(a.cast<A>(), b.cast<B>());
			return object_t();
		}
	};

	template<typename R, typename A, typename B, typename C>
	class built_in_function_3 : public built_in_function
	{
		R(*func)(A, B, C);

	public:
		built_in_function_3(R(*func)(A, B, C)) : func(func) {}

		object_t call(const std::vector<object_t> &vec)
		{
			if (vec.size() != 3)
				throw invalid_argument("This function require 3 arguments.");
			return call_fixed(vec[0], vec[1], vec[2]);
		}

		virtual object_t call_fixed(const object_t &a, const object_t &b, const object_t &c)
		{
			if (typeid(R) == typeid(void))
			{
				func(a.cast<A>(), b.cast<B>(), c.cast<C>());
				return object_t();
			}
			else
				return func(a.cast<A>(), b.cast<B>(), c.cast<C>());
		}
	};

	template<typename R, typename A, typename B, typename C>
	shared_ptr<built_in_function> create_function(R(*func)(A, B, C))
	{
		return shared_ptr<built_in_function>(new built_in_function_3<R, A, B, C>(func));
	}

	template<typename R, typename A, typename B, typename C, typename D>
	class built_in_function_4 : public built_in_function
	{
		R(*func)(A, B, C, D);

	public:
		built_in_function_4(R(*func)(A, B, C, D)) : func(func) {}

		object_t call(const std::vector<object_t> &vec)
		{
			if (vec.size() != 4)
				throw invalid_argument("This function require 4 arguments.");
			return call_fixed(vec[0], vec[1], vec[2], vec[3]);
		}

		virtual object_t call_fixed(const object_t &a, const object_t &b, const object_t &c, const object_t &d)
		{
			if (typeid(R) == typeid(void))
			{
				func(a.cast<A>(), b.cast<B>(), c.cast<C>(), d.cast<D>());
				return object_t();
			}
			else
				return func(a.cast<A>(), b.cast<B>(), c.cast<C>(), d.cast<D>());
		}
	};
}

