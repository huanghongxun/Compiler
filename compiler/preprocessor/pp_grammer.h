#pragma once

#include "../lexical/lexical.h"

#include "../tokens.h"
#include "pattern_parser.h"

namespace compiler::preprocessor
{
	using namespace compiler::parser;

	enum class pp_grammar_id
	{
		statement,
		include,
		system_include,
		marco_include,
		define,
		macro_parameters,
		macro_definition,
		undefine,
		ifdef,
		ifndef,
		if_,
		elif,
		else_,
		endif,
		line,
		error,
		warning,
		pragma,
		illformed,
		space,
		qualified_name,
	};

	template<typename _Position>
	struct store_position
	{
		store_position(_Position &pos) : pos(pos) {}

		template<typename _Token>
		void operator()(_Token const& token) const
		{
			pos = token.get_position();
		}

		_Position &pos;
	};

	struct store_found_eof
	{
		store_found_eof(bool &found_eof) : found_eof(found_eof) {}

		template<typename _Token>
		void operator()(_Token const& token) const
		{
			found_eof = true;
		}

		bool &found_eof;
	};

	struct store_found_directive
	{
		store_found_directive(token_id &found_directive)
			: found_directive(found_directive) {}

		template<typename _Token>
		void operator()(_Token const& token) const
		{
			found_directive = token_id(token);
		}

		token_id &found_directive;
	};

	template<typename _Position>
	struct pp_grammer : grammar<pp_grammer<_Position>>
	{
		typedef store_position<_Position> store_pos_t;
		typedef store_found_eof store_found_eof_t;
		typedef store_found_directive store_found_directive_t;

		template<typename _Scanner>
		struct definition
		{
			typedef rule<_Scanner, parser_context<>, dynamic_parser_tag> rule_type;

			rule_type pp_statement;
			rule_type pp_include, pp_marco_include, pp_system_include;
			rule_type pp_define, pp_marco_definition, pp_marco_parameters;
			rule_type pp_undef;
			rule_type pp_ifdef, pp_ifndef, pp_if, pp_else, pp_elif, pp_endif;
			rule_type pp_line, pp_error, pp_warning, pp_pragma;
			rule_type pp_illformed, pp_qualified_name;
			rule_type pp_eol;
			no_tree_rule_type ppsp;

			definition(pp_grammer const& self)
			{
				pp_statement.set_id(pp_grammar_id::statement);
				pp_include.set_id(pp_grammar_id::include);
				pp_marco_include.set_id(pp_grammar_id::marco_include);
				pp_system_include.set_id(pp_grammar_id::system_include);
				pp_define.set_id(pp_grammar_id::define);
				pp_marco_parameters.set_id(pp_grammar_id::marco_parameters);
				pp_marco_definition.set_id(pp_grammar_id::marco_definition);
				pp_undefine.set_id(pp_grammar_id::undefine);
				pp_ifdef.set_id(pp_grammar_id::ifdef);
				pp_ifndef.set_id(pp_grammar_id::ifndef);
				pp_if.set_id(pp_grammar_id::if_);
				pp_elif.set_id(pp_grammar_id::elif);
				pp_else.set_id(pp_grammar_id::else_);
				pp_endif.set_id(pp_grammar_id::endif);
				pp_line.set_id(pp_grammar_id::line);
				pp_error.set_id(pp_grammar_id::error);
				pp_warning.set_id(pp_grammar_id::warning);
				pp_pragma.set_id(pp_grammar_id::pragma);
				ppsp.set_id(pp_grammar_id::space);
				pp_qualified_name.set_id(pp_grammar_id::qualified_name);

				pp_statement = (
	//				pp_include |
	//				pp_system_include |
					pp_marco_include |
					pp_define |
					pp_undef |
					pp_ifdef |
					pp_ifndef |
					pp_if |
	//				pp_else |
					pp_elif |
	//				pp_endif |
					pp_line |
					pp_error |
					pp_warning |
					pp_pragma |
					pp_illformed
					) >> pp_eol[store_found_eoltokens_type(self.found_eoltokens)];

				pp_include =
					ch_p(token_id::PP_QHEADER)
					[store_found_directive_t(self.found_directive)];

				pp_system_include =
					ch_p(token_id::PP_HHEADER)
					[store_found_directive_t(self.found_directive)];

				pp_marco_include =
					no_node_d
					[
						ch_p(token_id::PP_INCLUDE)
						[store_found_directive_t(self.found_directive)]
					]
				>> *(any_p -
					(ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof))
					);

				pp_define =
					no_node_d
					[
						ch_p(token_id::PP_DEFINE)
						[store_found_directive_t(self.found_directive)]
				>> +ppsp
					]
				>> (ch_p(token_id::identifier) | pattern_p(token_category::keyword, token_category::token_type_mask))
					>> !marco_parameters
					>> !marco_definition
					;

				marco_parameters =
					confix_p(
						no_node_d[ch_p(token_id::left_paren) >> *ppsp],
						!list_p(
						(ch_p(token_id::identifier) | pattern_p(token_category::keyword, token_category::token_type_mask) | ch_p(token_id::ellipsis)),
							no_node_d[*ppsp >> ch_p(token_id::comma) >> *ppsp]
						),
						no_node_d[*ppsp >> ch_p(token_id::right_paren)]
					);

				pp_marco_definition =
					no_node_d[*ppsp] >> *(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_undef =
					no_node_d
					[
						ch_p(token_id::PP_UNDEF)
						[store_found_directive_t(self.found_directive)]
				>> +ppsp
					] >> (ch_p(token_id::identifier) | pattern_p(token_category::keyword, token_category::token_type_mask))
					;

				pp_ifdef =
					no_node_d
					[
						ch_p(token_id::PP_IFDEF)
						[store_found_directive_t(self.found_directive)]
				>> +ppsp
					]
				>> pp_qualified_name;

				pp_ifndef =
					no_node_d
					[
						ch_p(token_id::PP_IFNDEF)
						[store_found_directive_t(self.found_directive)]
				>> +ppsp
					]
				>> pp_qualified_name;

				pp_if =
					no_node_d
					[
						ch_p(token_id::PP_IF)
						[store_found_directive_t(self.found_directive)]
				>> *ppsp
					]
				>> +(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_else =
					no_node_d
					[
						ch_p(token_id::PP_ELSE)
						[store_found_directive_t(self.found_directive)]
					];

				pp_elif =
					no_node_d
					[
						ch_p(token_id::PP_ELIF)
						[store_found_directive_t(self.found_directive)]
				>> *ppsp
					]
				>> +(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_endif =
					no_node_d
					[
						ch_p(token_id::PP_ENDIF)
						[store_found_directive_t(self.found_directive)]
					];

				pp_line =
					no_node_d
					[
						ch_p(token_id::PP_LINE)
						[store_found_directive_t(self.found_directive)]
				>> *ppsp
					]
				>> +(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_illformed =
					no_node_d[ch_p(token_id::pound) >> *ppsp]
					>> (any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)))
					>> no_node_d[*(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)))];

				pp_error =
					no_node_d
					[
						ch_p(token_id::PP_ERROR)
						[store_found_directive_t(self.found_directive)]
				>> *ppsp
					]
				>> *(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_warning =
					no_node_d
					[
						ch_p(token_id::PP_WARNING)
						[store_found_directive_t(self.found_directive)]
				>> *ppsp
					]
				>> *(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_pragma =
					no_node_d
					[
						ch_p(token_id::PP_PRAGMA)
						[store_found_directive_t(self.found_directive)]
				>> *ppsp
					]
				>> *(any_p - (ch_p(token_id::new_line) | ch_p(token_id::line_comment) | ch_p(token_id::eof)));

				pp_qualified_name =
					ch_p(token_id::colon_colon) ||
					(no_node_d[*ppsp] >> list_p((ch_p(token_id::identifier) | pattern_p(token_category::keyword, token_category::token_type_mask)), no_node_d[*ppsp] >> ch_p(token_id::colon_colon) >> no_node_d[*ppsp]));
			
				ppsp = ch_p(token_id::space) | ch_p(token_id::block_comment);

				pp_eol =
					no_node_d
					[
						*ppsp
						>> (ch_p(token_id::new_line)[store_pos_t(self.pos_of_newline)]
							| ch_p(token_id::line_comment)[store_pos_t(self.pos_of_newline)]
							| ch_p(token_id::eof)[store_pos_t(self.pos_of_newline)][store_found_eof_t(self.found_eof)]
							)
					];


			}

			rule_type const& root() const
			{
				return pp_statement;
			}
		};

		pp_grammar(pp_grammar_rule_ids &rule_ids, _Position &pos_of_new_line,
			bool &found_eof, token_id found_directive)
			: rule_ids(rule_ids), pos_of_new_line(pos_of_new_line),
			found_eof(found_eof), found_directive(found_directive) {}
	};

	string get_directive_name(token_id id)
	{
		switch (id)
		{
			case token_id::PP_QHEADER:
			case token_id::PP_HHEADER:
			case token_id::PP_INCLUDE:
				return "#include";
			case token_id::PP_DEFINE:
				return "#define";
			case token_id::PP_UNDEF:
				return "#undef";
			case token_id::PP_IFDEF:
				return "#ifdef";
			case token_id::PP_IFNDEF:
				return "#ifndef";
			case token_id::PP_IF:
				return "#if";
			case token_id::PP_ELSE:
				return "#else";
			case token_id::PP_ELIF:
				return "#elif";
			case token_id::PP_ENDIF:
				return "#endif";
			case token_id::PP_LINE:
				return "#line";
			case token_id::PP_ERROR:
				return "#error";
			case token_id::PP_WARNING:
				return "#warning";
			case token_id::PP_PRAGMA:
				return "#pragma";
			default:
				return "#unknown";
		}
	}

	template<typename _LexIterator, typename _TokenContainer>
	struct cpp_grammar_gen
	{
		typedef _LexIterator iterator_type;
		typedef node_val_data_factory<> node_factory_type;

		static tree_parse_info<iterator_type, node_factory_type>
			parse_pp_grammar(iterator_type const& first, iterator_type const& last,
				position_type const& act_pos, bool &found_eof, token_type &found_directive, _TokenContainer &found_eoltokens)
		{
			static pp_grammar<typename _To

			found_eof = false;
			found_directive = token_id::eof;



			if (!s.match && found_directive != token_id::eof)
			{
				throw;
			}

			found
		}
	};
}