#pragma once

#include <locale>

namespace compiler::parser
{
	template<typename _Base>
	struct skipper_iteration_policy : public _Base
	{
		typedef _Base base_type;

		template<typename _Scanner>
		void skip(_Scanner const& scan) const
		{
			while (!_Base::at_end(scan) && std::isspace(_Base::get(scan), std::locale()))
				_Base::next(scan);
		}

		template<typename _Scanner>
		void next(_Scanner const& scan) const
		{
			_Base::next(scan);
			scan.skip(scan);
		}

		template<typename _Scanner>
		bool at_end(_Scanner const& scan) const
		{
			scan.skip(scan);
			return _Base::at_end(scan);
		}
	};

	template<typename _Base>
	struct no_skipper_iteration_policy : public _Base
	{
		typedef _Base base_type;

		no_skipper_iteration_policy() : base_type() {}

		template<typename _Policy>
		no_skipper_iteration_policy(_Policy const& other) : base_type() {}

		template<typename _Scanner>
		void skip(_Scanner const&) const {}
	};

	template<typename S, typename _Scanner, typename _Base>
	inline void skipper_skip(S const& s, _Scanner const& scan, no_skipper_iteration_policy<_Base> const&)
	{
		while (true)
		{
			auto save = scan.now();
			if (!s.parse(scan))
			{
				scan.reset(save);
				break;
			}
		}
	}

	template<typename S, typename _Scanner, typename _Base>
	inline void skipper_skip(S const& s, _Scanner const& scan, skipper_iteration_policy<_Base> const&)
	{
		auto scan2 = scan.change_iteration_policy<no_skipper_iteration_policy<_Scanner::iteration_policy_type>>();
		while (true)
		{
			auto save = scan.now();
			if (!s.parse(scan2))
			{
				scan.reset(save);
				break;
			}
		}
	}

	template<typename _Parser, typename _Base>
	struct skip_parser_iteration_policy : public skipper_iteration_policy<_Base>
	{
		typedef skipper_iteration_policy<_Base> base_type;

		skip_parser_iteration_policy(_Parser const& skip_parser)
			: subject(skip_parser) {}

		template<typename _Scanner>
		void skip(_Scanner const& scan) const
		{
			skipper_skip(subject, scan, scan);
		}

		_Parser const& skipper() const
		{
			return subject;
		}

	private:
		_Parser const& subject;
	};

	template<typename R, typename _Parser, typename _Scanner, typename _Base>
	inline R implicit_lexeme_parse(_Parser const& p, _Scanner const& scan, skipper_iteration_policy<_Base> const&)
	{
		scan.skip(scan);

		R hit = p.parse_main(scan.change_iteration_policy<no_skipper_iteration_policy<typename _Scanner::iteration_policy_type>>(scan));

		return hit;
	}

	template<typename R, typename _Parser, typename _Scanner, typename _Base>
	inline R implicit_lexeme_parse(_Parser const& p, _Scanner const& scan, no_skipper_iteration_policy<_Base> const&)
	{
		return p.parse_main(scan);
	}

	template<typename R, typename _Parser, typename _Scanner>
	inline R implicit_lexeme_parse(_Parser const& p, _Scanner const& scan, iteration_policy const&)
	{
		return p.parse_main(scan);
	}
}