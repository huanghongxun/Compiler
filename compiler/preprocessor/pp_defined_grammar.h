#pragma once

#include "../tokens.h"
#include "pattern_parser.h"
#include "../lexical/lexical.h"

namespace compiler::preprocessor
{
	using namespace compiler::parser;

	template <typename _Container>
	struct pp_defined_grammar : public grammar<pp_defined_grammar<_Container>>
	{
		template <typename _Scanner>
		struct definition
		{
			typedef rule<_Scanner, parser_context<>, dynamic_parser_tag> rule_type;

			rule_type defined;
			rule_type id;

			definition(pp_defined_grammar const& self)
			{
				defined =
					ch_p(token_id::identifier)
					>> ((ch_p(token_id::left_paren) >> id >> ch_p(token_id::right_paren)) | id);

				id =
					ch_p(token_id::identifier)[push_back_a(self.result_seq)] |
					pattern_p(token_category::keyword, token_category::token_type_mask | token_category::preprocessor_flag)[push_back_a(self.result_seq)] |
					pattern_p(token_category::bool_literal, token_category::token_type_mask | token_category::preprocessor_flag)[push_back_a(self.result_seq)];
			}

			rule_type const& start() const
			{
				return defined;
			}
		};

		_Container & result_seq;
	};

	template <typename _Iterator>
	struct pp_defined_grammar_gen
	{

	};
}