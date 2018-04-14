#pragma once

#include <optional>
#include "nil_t.h"

namespace compiler::parser
{
	template<typename T = nil_t>
	struct match
	{
		match() : len(-1) {}
		match(int len) : len(len) {}
		match(int len, std::optional<T> const& val) : len(len), val(val) {}

		template<typename T2>
		match(match<T2> const& other)
			: len(other.length()), val()
		{

		}

		bool has_valid_attribute() const
		{
			return val.has_value();
		}

		template<typename T2>
		match& operator=(match<T2> const& other)
		{
			len = other.length();
			return *this;
		}

		template<typename _Match>
		void concat(_Match const& other)
		{
			len += other.length();
		}

		template<typename _Value>
		void value(_Value const& val)
		{
			this->val = val;
		}

		operator bool() const
		{
			return len >= 0;
		}

		int length() const
		{
			return len;
		}

		bool operator!() const
		{
			return len < 0;
		}

		T const& value() const
		{
			return *val;
		}
	private:
		int len;
		std::optional<T> val;
	};

	template<>
	struct match<nil_t>
	{
		match() : len(-1) {}
		match(int len) : len(len) {}
		match(int len, nil_t) : len(len) {}

		template<typename T2>
		match(match<T2> const& other)
			: len(other.length()) {}

		bool has_valid_attribute() const
		{
			return false;
		}

		template<typename T2>
		match& operator=(match<T2> const& other)
		{
			len = other.length();
			return *this;
		}

		template<typename _Match>
		void concat(_Match const& other)
		{
			len += other.length();
		}

		template<typename _Value>
		void value(_Value const& val)
		{
			this->val.reset(val);
		}

		operator bool() const
		{
			return len >= 0;
		}

		int length() const
		{
			return len;
		}

		bool operator!() const
		{
			return len < 0;
		}

		nil_t value() const
		{
			return nil_t();
		}
	private:
		int len;
	};
}