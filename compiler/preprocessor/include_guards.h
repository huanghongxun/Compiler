#pragma once

#include "../tokens.h"

namespace compiler::preprocessor
{
	// looking for:
	//     #ifndef INCLUDE_GUARD
	//     #define INCLUDE_GUARD
	//     ...
	//     #endif
	//    
	// or
	//     #if !defined(INCLUDE_GUARD) or #if !defined INCLUDE_GUARD
	//     #define INCLUDE_GUARD
	//     ...
	//     #endif
	template<typename _Token>
	struct include_guards
	{
		typedef _Token token_type;
		typedef typename _Token::string_type string_type;

		include_guards()
			: state(state_beginning), detected_guards(false), current_state(true), if_depth(0)
		{

		}

		_Token &detect_guard(_Token &t)
		{
			return current_state ? state(t) : t;
		}

		bool detected(string_type& guard_name) const
		{
			if (detected_guards)
			{
				guard_name = string_type(this->guard_name);
				return true;
			}
			else
				return false;
		}

	private:
		typedef token_type& (*state_type)(token_type& t);
		state_type state;

		bool detected_guards;
		bool current_state;
		string_type guard_name;
		int if_depth;

		bool is_skippable(token_id id) const
		{
			return is_category(id, token_category::whitespace) || is_category(id, token_category::eol) || id == token_id::pound;
		}

		token_type& state_beginning(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::PP_IFNDEF)
				state = state_ifndef;
			else if (id == token_id::PP_IF)
				state = state_1a;
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// already found #ifndef, looking for a guard name.
		token_type& state_ifndef(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::identifier)
			{
				guard_name = t.get_value();
				state = state_define;
			}
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// already found if, looking for !defined
		token_type& state_if(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::not)
				state = state_not;
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// found #if !, looking for !
		token_type& state_not(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::identifier && t.get_value() == "defined")
				state = state_defined;
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// found #if !defined, looking for ( or id
		token_type& state_defined(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::left_paren)
				state = state_if_guard_name;
			else if (id == token_id::identifier)
			{
				guard_name = t.get_value();
				state = state_define;
			}
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// found #if !defined(, looking for id
		token_type& state_if_left_paren(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::identifier)
			{
				guard_name = t.get_value();
				state = state_if_identifier;
			}
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// found #if !defined(id, looking for )
		token_type& state_if_identifier(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::right_paren)
			{
				state = state_define;
			}
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// looking for #define
		token_type& state_define(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::PP_DEFINE)
				state = state_define_identifier;
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// found #define, looking for id
		token_type& state_define_identifier(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::identifier && t.get_value() == guard_name)
				state = state_inside;
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}

		// found #define id, looking for #endif
		token_type& state_endif(token_type& t)
		{
			token_id = token_id(t);
			if (id == token_id::PP_IF || id == token_id::PP_IFDEF || id == token_id::PP_IFNDEF)
				++if_depth;
			else if (id == token_id::PP_ENDIF)
				if (if_depth > 0)
					if_depth--;
				else
					state = state_eof;
			return t;
		}

		// found #endif, looking for eof
		token_type& state_eof(token_type& t)
		{
			token_id id = token_id(t);
			if (id == token_id::eof)
				detected_guards = current_state;
			else if (!is_skippable(id))
				current_state = false;
			return t;
		}
	};
}