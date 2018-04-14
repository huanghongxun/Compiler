#pragma once

#include <type_traits>
#include "nil_t.h"

namespace compiler::parser
{
	struct plain_parser_category {};
	struct binary_parser_category : plain_parser_category {};
	struct unary_parser_category : plain_parser_category {};
	struct action_parser_category : plain_parser_category {};

	template<typename _Parser, typename _Action>
	struct action;

	template <typename _MatchPolicy, typename T>
	struct match_result
	{
		typedef typename _MatchPolicy::template result<T> type;
	};

	template<typename _Parser, typename _Scanner>
	struct parser_result
	{
		typedef typename std::remove_reference<_Parser>::type parser_type;
		typedef typename parser_type::template result<_Scanner> type;
	};

	template<typename _Derived>
	struct parser
	{
		typedef _Derived actual_type;
		typedef plain_parser_category parse_category_type;

		_Derived& actual()
		{
			return *static_cast<_Derived*>(this);
		}

		_Derived const& actual() const
		{
			return *static_cast<_Derived const*>(this);
		}

		template<typename _Action>
		action<_Derived, _Action> operator[](_Action const& actor) const
		{
			return action<_Derived, _Action>(actual(), actor);
		}

		template <typename _Scanner>
		using result = typename match_result<_Scanner, nil_t>::type;
	};

	struct parser_id
	{
		parser_id() : p(nullptr) {}
		parser_id(void const* p) : p(p) {}
		parser_id(size_t l) : l(l) {}

		bool operator==(parser_id const& x) const
		{
			return p == x.p;
		}

		bool operator!=(parser_id const& x) const
		{
			return p != x.p;
		}

		bool operator<(parser_id const& x) const
		{
			return p < x.p;
		}

		size_t id() const
		{
			return l;
		}
	private:
		void const* p;
		size_t l;
	};

	struct parser_tag_base {};
	struct parser_address_tag : parser_tag_base
	{
		parser_id id() const
		{
			return parser_id(reinterpret_cast<size_t>(this));
		}
	};

	template<size_t N>
	struct parser_tag : parser_tag_base
	{
		static parser_id id()
		{
			return parser_id(N);
		}
	};

	struct dynamic_parser_tag : public parser_tag_base
	{
		dynamic_parser_tag() : tag(size_t(0)) {}

		parser_id id() const
		{
			return tag.id() ? tag : parser_id(reinterpret_cast<size_t>(this));
		}

		void set_id(parser_id id)
		{
			tag = id;
		}

	private:
		parser_id tag;
	};

	template<typename _Iterator = char const*>
	struct parse_info
	{
		_Iterator last;
		bool hit, full;
		size_t length;

		parse_info(_Iterator const& last = _Iterator(),
			bool hit = false, bool full = false, size_t length = 0)
			: last(last), hit(hit), full(full), length(length) {}

		template<typename _ParseInfo>
		parse_info(_ParseInfo const& p)
			: last(p.last), hit(p.hit), full(p.full), length(p.length) {}
	};

	template<typename _Context>
	struct parser_context_linker : public _Context
	{
		template<typename _Parser>
		parser_context_linker(_Parser const& p) : _Context(p) {}

		template<typename _Parser, typename _Scanner>
		void pre_parse(_Parser const& p, _Scanner const& scan)
		{
			_Context::pre_parse(p, scan);
		}

		template<typename _Result, typename _Parser, typename _Scanner>
		_Result& post_parse(_Result &hit,_Parser const& p, _Scanner const& scan)
		{
			_Context::post_parse(hit, p, scan);
		}
	};

	template<typename _Attribute = nil_t>
	struct parser_context
	{
		typedef parser_context_linker<parser_context> context_linker_type;

		template<typename _Parser>
		parser_context(_Parser const&) {}

		template<typename _Parser, typename _Scanner>
		void pre_parse(_Parser const&, _Scanner const&) {}

		template<typename _Result, typename _Parser, typename _Scanner>
		_Result& post_parse(_Result& hit, _Parser const&, _Scanner const&)
		{
			return hit;
		}
	};

	template<typename _Scanner, typename _Context, typename _Result, typename _Parser, typename _InferredScanner>
	_Result parse_context(_InferredScanner const& scan, _Parser const& p)
	{
		_Scanner scan_wrap(scan);
		_Context context_wrap(p);
		context_wrap.pre_parse(p, scan_wrap);
		auto hit = p.parse_main(scan);
		return context_wrap.post_parse(hit, p, scan_wrap);
	}

	template<typename _Scanner>
	struct parser_scanner_linker : public _Scanner
	{
		parser_scanner_linker(_Scanner const& scan)
			: _Scanner(scan) {}
	};
}