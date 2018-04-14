#pragma once

#include <limits>
#include <locale>
#include "../parser.h"

namespace compiler::parser
{
	template <typename _Scanner>
	bool parse_sign(_Scanner const& scan, size_t& count)
	{
		count = 0;
		bool neg = *scan == '-';
		if (neg || (*scan == '+'))
		{
			++scan;
			++count;
			return neg;
		}
		return false;
	}

	struct sign_parser : public parser<sign_parser>
	{
		typedef sign_parser self_type;

		template <typename _Scanner>
		using result = typename match_result<_Scanner, bool>::type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (!scan.at_end())
			{
				size_t len;
				auto save = scan.now();
				bool neg = parse_sign(scan, len);
				if (len)
					return scan.create_match(1, neg, save, scan.now());
			}
			return scan.no_match();
		}
	};

	sign_parser const sign_p = sign_parser();

	template<int radix>
	struct radix_traits;

	template<>
	struct radix_traits<2>
	{
		template<typename _Char, typename T>
		static bool digit(_Char ch, T& val)
		{
			val = ch - '0';
			return '0' <= ch && ch <= '7';
		}
	};

	template<>
	struct radix_traits<8>
	{
		template<typename _Char, typename T>
		static bool digit(_Char ch, T& val)
		{
			val = ch - '0';
			return '0' <= ch && ch <= '7';
		}
	};

	template<>
	struct radix_traits<10>
	{
		template<typename _Char, typename T>
		static bool digit(_Char ch, T& val)
		{
			val = ch - '0';
			return '0' <= ch && ch <= '9';
		}
	};

	template<>
	struct radix_traits<16>
	{
		template<typename _Char, typename T>
		static bool digit(_Char ch, T& val)
		{
			ch = std::tolower(ch, std::locale());
			if ('a' <= ch && ch <= 'f')
				val = ch - 'a' + 10;
			else if ('0' <= ch && ch <= '9')
				val = ch - '0';
			else
				return false;
			return true;
		}
	};

	template<typename T, int radix>
	struct positive_accumulator
	{
		bool operator()(T& n, T digit)
		{
			if (std::numeric_limits<T>::is_specialized)
			{
				static T const max = std::numeric_limits<T>::max();
				static T const max_div_radix = max / radix;

				if (n > max_div_radix)
					return false;
				n *= radix;
				if (n > max - digit)
					return false;
				n += digit;
				return true;
			}
			else
			{
				n *= radix;
				n += digit;
				return true;
			}
		}
	};

	template<typename T, int radix>
	struct negative_accumulator
	{
		bool operator()(T& n, T digit)
		{
			if (std::numeric_limits<T>::is_specialized)
			{
				static T const min = std::numeric_limits<T>::min();
				static T const min_div_radix = min / radix;

				if (n < min_div_radix)
					return false;
				n *= radix;
				if (n < min + digit)
					return false;
				n -= digit;
				return true;
			}
			else
			{
				n *= radix;
				n -= digit;
				return true;
			}
		}
	};

	template<int max_digits>
	constexpr bool allow_more_digits(size_t l)
	{
		return l < max_digits;
	}

	template<>
	constexpr bool allow_more_digits<-1>(size_t)
	{
		return true;
	}

	template<int radix, unsigned min_digits, unsigned max_digits, typename _Accu, typename _Scanner, typename T>
	bool parse_int(_Scanner const& scan, T& n, size_t& count, _Accu accumulator)
	{
		size_t i = 0;
		T digit;
		while (!scan.at_end() && radix_traits<radix>::digit(*scan, digit) && allow_more_digits<max_digits>(i))
		{
			if (!accumulator(n, digit))
				return false;
			++i; ++scan; ++count;
		}
		return i >= min_digits;
	}

	template<typename T = unsigned, int radix = 10, unsigned min_digits = 1, int max_digits = -1>
	struct uint_parser : parser<uint_parser<T, radix, min_digits, max_digits>>
	{
		typedef uint_parser<T, radix, min_digits, max_digits> self_type;

		template <typename _Scanner>
		using result = typename match_result<_Scanner, T>::type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (!scan.at_end())
			{
				T n = 0;
				size_t count = 0;
				auto save = scan.now();
				if (parse_int<radix, min_digits, max_digits>(scan, n, count, positive_accumulator<T, radix>()))
					return scan.create_match(count, n, save, scan.now());
			}
			return scan.no_match();
		}
	};

	template<typename T = unsigned, int radix = 10, unsigned min_digits = 1, int max_digits = -1>
	struct int_parser : parser<int_parser<T, radix, min_digits, max_digits>>
	{
		typedef int_parser<T, radix, min_digits, max_digits> self_type;

		template <typename _Scanner>
		using result = typename match_result<_Scanner, T>::type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (!scan.at_end())
			{
				T n = 0;
				size_t count = 0;
				auto save = scan.now();
				bool neg = parse_sign(scan, count), res;
				if (neg)
					res = parse_int<radix, min_digits, max_digits>(scan, n, count, negative_accumulator<T, radix>());
				else
					res = parse_int<radix, min_digits, max_digits>(scan, n, count, positive_accumulator<T, radix>());
				if (res)
					return scan.create_match(count, n, save, scan.now());
			}
			return scan.no_match();
		}
	};

	template<typename T>
	struct ureal_parser_policies
	{
		static bool const allow_leading_dot = true;
		static bool const allow_trailing_dot = true;
		static bool const expect_dot = false;

		template<typename _Scanner>
		static typename match_result<_Scanner, nil_t>::type
			parse_sign(_Scanner &scan)
		{
			return scan.no_match();
		}

		template<typename _Scanner>
		static typename parser_result<uint_parser<T>, _Scanner>::type
			parse_n(_Scanner &scan)
		{
			return uint_parser<T>().parse(scan);
		}

		template<typename _Scanner>
		static typename parser_result<char_literal<>, _Scanner>::type
			parse_dot(_Scanner &scan)
		{
			return ch_p('.').parse(scan);
		}

		template<typename _Scanner>
		static typename parser_result<uint_parser<T>, _Scanner>::type
			parse_frac_n(_Scanner &scan)
		{
			return uint_parser<>().parse(scan);
		}

		template<typename _Scanner>
		static typename parser_result<char_literal<>, _Scanner>::type
			parse_exp(_Scanner &scan)
		{
			return as_lower_d['e'].parse(scan);
		}

		template<typename _Scanner>
		static typename parser_result<int_parser<>, _Scanner>::type
			parse_exp_n(_Scanner &scan)
		{
			return int_parser<>().parse(scan);
		}
	};

	template<typename T>
	struct real_parser_policies : public ureal_parser_policies<T>
	{
		template<typename _Scanner>
		static typename parser_result<sign_parser, _Scanner>::type
			parse_sign(_Scanner& scan)
		{
			return sign_p.parse(scan);
		}
	};

	template<typename _Result, typename T, typename _Policy>
	struct real_parser_impl
	{
		typedef real_parser_impl<_Result, T, _Policy> self_type;

		template<typename _Scanner>
		_Result parse_main(_Scanner const& scan) const
		{
			typedef typename parser_result<sign_parser, _Scanner>::type sign_type;
			typedef typename parser_result<char_literal<>, _Scanner>::type e_type;

			if (scan.at_end())
				return scan.no_match();

			auto save = scan.now();

			sign_type sign = _Policy::parse_sign(scan);
			size_t count = sign ? sign.length() : 0;
			bool neg = false;
			if (sign.has_valid_attribute())
				neg = sign.value();

			_Result n_match = _Policy::parse_n(scan);
			T n = n_match.has_valid_attribute() ? n_match.value() : T(0);
			bool got_a_number = n_match;
			e_type e_hit;

			if (!got_a_number && !_Policy::allow_leading_dot)
				return scan.no_match();
			else
				count += n_match.length();

			if (neg)
				n = -n;

			if (_Policy::parse_dot(scan))
			{
				if (auto hit = _Policy::parse_frac_n(scan))
				{
					hit.value(hit.value() * std::pow(T(10), T(-hit.length())));
					if (neg)
						n -= hit.value();
					else
						n += hit.value();
					count += hit.length() + 1;
				}
				else if (!got_a_number || !_Policy::allow_trailing_dot)
					return scan.no_match();

				e_hit = _Policy::parse_exp(scan);
			}
			else
			{
				if (!got_a_number) return scan.no_match();

				e_hit = _Policy::parse_exp(scan);
				if (_Policy::expect_dot && !e_hit)
					return scan.no_match();
			}

			if (e_hit)
			{
				if (auto e_n_hit = _Policy::parse_exp_n(scan))
				{
					n *= std::pow(T(10), T(e_n_hit.value()));
					count += e_n_hit.length() + e_hit.length();
				}
				else
				{
					return scan.no_match();
				}
			}

			return scan.create_match(count, n, save, scan.now());
		}

		template<typename _Scanner>
		static _Result parse(_Scanner const& scan)
		{
			static self_type s;
			return implicit_lexeme_parse<_Result>(s, scan, scan);
		}
	};

	template<typename T, typename _Policy>
	struct real_parser : public parser<real_parser<T, _Policy>>
	{
		typedef real_parser<T, _Policy> self_type;

		template <typename _Scanner>
		using result = typename match_result<_Scanner, T>::type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef typename parser_result<self_type, _Scanner>::type result_type;
			return real_parser_impl<result_type, T, _Policy>::parse(scan);
		}
	};

	int_parser<int> const int_p = int_parser<int>();
	uint_parser<unsigned> const uint_p = uint_parser<unsigned>();
	uint_parser<unsigned, 2> const bin_p = uint_parser<unsigned, 2>();
	uint_parser<unsigned, 8> const oct_p = uint_parser<unsigned, 8>();
	uint_parser<unsigned, 16> const hex_p = uint_parser<unsigned, 16>();

	real_parser<double, ureal_parser_policies<double>> const ureal_p
		= real_parser<double, ureal_parser_policies<double>>();

	real_parser<double, real_parser_policies<double>> const real_p
		= real_parser<double, real_parser_policies<double>>();
}