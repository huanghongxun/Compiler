#pragma once

namespace compiler
{
	enum class token_category
	{
		identifier     = 0x08040000,
		parameter      = 0x08840000,
		ext_parameter  = 0x088C0000,
		keyword        = 0x10040000,
		operator_t     = 0x18040000,

		literal        = 0x20040000,
		int_literal    = 0x20840000,
		float_literal  = 0x21040000,
		string_literal = 0x21840000,
		char_literal   = 0x22040000,
		bool_literal   = 0x22840000,

		preprocessor   = 0x28040000,
		preprocessor_conditional = 0x28440000,

		unknown        = 0x50000000,
		eol            = 0x58000000,
		eof            = 0x60000000,
		whitespace     = 0x68000000,
		internal       = 0x70000000,

		token_type_mask= 0x7F800000,
		token_value_mask = 0x0007FFFF,
		preprocessor_flag= 0x00040000
	};

	constexpr int _token_id(int id, token_category type)
	{
		return id | (int)type;
	}

	enum class token_id
	{
		unknown       = 0,
		any           = _token_id(0, token_category::unknown),

		// OPERATORS
		assign        = _token_id(0x100, token_category::operator_t), // =
		comma         = _token_id(0x101, token_category::operator_t), // ,
		colon         = _token_id(0x102, token_category::operator_t), // :
		semicolon     = _token_id(0x103, token_category::operator_t), // ;
		question_mark = _token_id(0x104, token_category::operator_t), // ?
		compl = _token_id(0x105, token_category::operator_t), // ~
		left_brace    = _token_id(0x106, token_category::operator_t), // {
		right_brace   = _token_id(0x107, token_category::operator_t), // }
		left_paren    = _token_id(0x108, token_category::operator_t), // (
		right_paren   = _token_id(0x109, token_category::operator_t), // )
		left_bracket  = _token_id(0x10A, token_category::operator_t), // [
		right_bracket = _token_id(0x10B, token_category::operator_t), // ]
		ellipsis      = _token_id(0x10C, token_category::operator_t), // ...
		pound         = _token_id(0x10D, token_category::operator_t), // #
		pound_pound   = _token_id(0x10E, token_category::operator_t), // ##


		equal         = _token_id(0x110, token_category::operator_t), // ==
		greater       = _token_id(0x111, token_category::operator_t), // >
		greater_equal = _token_id(0x112, token_category::operator_t), // >=
		less          = _token_id(0x113, token_category::operator_t), // <
		less_equal    = _token_id(0x114, token_category::operator_t), // <=
		not_equal     = _token_id(0x115, token_category::operator_t), // !=

		and           = _token_id(0x120, token_category::operator_t), // & and
		bit_and       = _token_id(0x121, token_category::operator_t), // && bitand
		and_assign    = _token_id(0x123, token_category::operator_t), // &= and_eq
		or = _token_id(0x124, token_category::operator_t), // | or
		bit_or        = _token_id(0x125, token_category::operator_t), // || bitor
		or_assign     = _token_id(0x126, token_category::operator_t), // |= or_eq
		xor           = _token_id(0x127, token_category::operator_t), // ^ xor
		xor_assign    = _token_id(0x128, token_category::operator_t), // ^= xor_eq
		not = _token_id(0x129, token_category::operator_t), // ! not
		shl           = _token_id(0x12A, token_category::operator_t), // <<
		shl_assign    = _token_id(0x12B, token_category::operator_t), // <<=
		shr           = _token_id(0x12C, token_category::operator_t), // >>
		shr_assign    = _token_id(0x12D, token_category::operator_t), // >>=

		div           = _token_id(0x130, token_category::operator_t), // /
		div_assign    = _token_id(0x131, token_category::operator_t), // /=
		minus         = _token_id(0x132, token_category::operator_t), // -
		minus_assign  = _token_id(0x133, token_category::operator_t), // -=
		minus_minus   = _token_id(0x134, token_category::operator_t), // --
		percent       = _token_id(0x135, token_category::operator_t), // %
		percent_assign= _token_id(0x136, token_category::operator_t), // %=
		plus          = _token_id(0x137, token_category::operator_t), // +
		plus_assign   = _token_id(0x138, token_category::operator_t), // +=
		plus_plus     = _token_id(0x139, token_category::operator_t), // ++
		star          = _token_id(0x13A, token_category::operator_t), // *
		star_assign   = _token_id(0x13B, token_category::operator_t), // *=

		dot           = _token_id(0x140, token_category::operator_t), // .
		dot_star      = _token_id(0x141, token_category::operator_t), // .*
		arrow         = _token_id(0x142, token_category::operator_t), // ->
		arrow_star    = _token_id(0x143, token_category::operator_t), // ->*
		colon_colon   = _token_id(0x144, token_category::operator_t), // ::

		// LITERALS
		FALSE         = _token_id(0x190, token_category::bool_literal), // false
		TRUE          = _token_id(0x191, token_category::bool_literal), // true

		// KEYWORDS
		BREAK         = _token_id(0x201, token_category::keyword),
		CASE          = _token_id(0x202, token_category::keyword),
		CONST         = _token_id(0x203, token_category::keyword),
		CONTINUE      = _token_id(0x204, token_category::keyword),
		DEFAULT       = _token_id(0x205, token_category::keyword),
		DO            = _token_id(0x206, token_category::keyword),
		ELSE          = _token_id(0x207, token_category::keyword),
		EXTERN        = _token_id(0x208, token_category::keyword),
		FOR           = _token_id(0x209, token_category::keyword),
		GOTO          = _token_id(0x20A, token_category::keyword),
		IF            = _token_id(0x20B, token_category::keyword),
		INLINE        = _token_id(0x20C, token_category::keyword),
		RETURN        = _token_id(0x20D, token_category::keyword),
		SIZEOF        = _token_id(0x20E, token_category::keyword),
		STATIC        = _token_id(0x20F, token_category::keyword),
		SWITCH        = _token_id(0x210, token_category::keyword),
		TYPEDEF       = _token_id(0x211, token_category::keyword),
		WHILE         = _token_id(0x212, token_category::keyword),

		// TYPES
		BOOL          = _token_id(0x220, token_category::keyword),
		CHAR          = _token_id(0x221, token_category::keyword),
		SHORT         = _token_id(0x222, token_category::keyword),
		INT           = _token_id(0x223, token_category::keyword),
		LONG          = _token_id(0x224, token_category::keyword),
		FLOAT         = _token_id(0x225, token_category::keyword),
		DOUBLE        = _token_id(0x226, token_category::keyword),
		ENUM          = _token_id(0x227, token_category::keyword),
		STRUCT        = _token_id(0x228, token_category::keyword),
		UNION         = _token_id(0x229, token_category::keyword),
		UNSIGNED      = _token_id(0x22A, token_category::keyword),
		SIGNED        = _token_id(0x22B, token_category::keyword),
		VOID          = _token_id(0x22C, token_category::keyword),

		INT8          = _token_id(0x230, token_category::keyword), // __int8 int8_t
		INT16         = _token_id(0x231, token_category::keyword), // __int16 int16_t
		INT32         = _token_id(0x232, token_category::keyword), // __int32 int32_t
		INT64         = _token_id(0x233, token_category::keyword), // __int64 int64_t
		CHAR16        = _token_id(0x234, token_category::keyword), // char16_t
		CHAR32        = _token_id(0x235, token_category::keyword), // char32_t

		PP_DEFINE     = _token_id(0x250, token_category::preprocessor), // #define
		PP_IF         = _token_id(0x251, token_category::preprocessor_conditional), // #if
		PP_IFDEF      = _token_id(0x252, token_category::preprocessor_conditional), // #ifdef
		PP_IFNDEF     = _token_id(0x253, token_category::preprocessor_conditional), // #ifndef
		PP_ELSE       = _token_id(0x254, token_category::preprocessor_conditional), // #else
		PP_ELIF       = _token_id(0x255, token_category::preprocessor_conditional), // #elif
		PP_ENDIF      = _token_id(0x256, token_category::preprocessor_conditional), // #endif
		PP_ERROR      = _token_id(0x257, token_category::preprocessor), // #error
		PP_LINE       = _token_id(0x258, token_category::preprocessor), // #line
		PP_PRAGMA     = _token_id(0x259, token_category::preprocessor), // #pragma
		PP_UNDEF      = _token_id(0x25A, token_category::preprocessor), // #undef
		PP_WARNING    = _token_id(0x25B, token_category::preprocessor), // #warning
		PP_INCLUDE    = _token_id(0x25C, token_category::preprocessor), // #include "..."
		PP_QHEADER    = _token_id(0x25D, token_category::preprocessor), // #include <...>
		PP_HHEADER    = _token_id(0x25E, token_category::preprocessor), // #include ...
		PP_NUMBER     = _token_id(0x25F, token_category::internal),

		identifier    = _token_id(0x260, token_category::identifier),

		eol           = _token_id(0x300, token_category::eol),
		cont_line     = _token_id(0x301, token_category::eol),
		new_line      = _token_id(0x302, token_category::eol),
		gen_new_line  = _token_id(0x303, token_category::eol),
		eof           = _token_id(0x304, token_category::eof),
		eoi           = _token_id(0x305, token_category::eof),
		space         = _token_id(0x306, token_category::whitespace),
		line_comment  = _token_id(0x307, token_category::whitespace),
		block_comment = _token_id(0x308, token_category::whitespace),

		octal_int     = _token_id(0x310, token_category::int_literal),
		decimal_int   = _token_id(0x311, token_category::int_literal),
		hex_int       = _token_id(0x312, token_category::int_literal),
		int_literal   = _token_id(0x313, token_category::int_literal),
		long_literal  = _token_id(0x314, token_category::int_literal),
		float_literal = _token_id(0x315, token_category::float_literal),
		char_literal  = _token_id(0x316, token_category::char_literal),
		string_literal= _token_id(0x317, token_category::string_literal),

		last_token    = _token_id(0xF00, token_category::unknown),

		non_replacable_id = _token_id(0xF01, token_category::identifier),
		place_holder  = _token_id(0xF02, token_category::whitespace),
		place_marker  = _token_id(0xF03, token_category::internal),
		param_base    = _token_id(0xF04, token_category::parameter),
		ext_param_base= _token_id(0xF05, token_category::parameter)
	};

	template<typename Token>
	constexpr int category_from_token(Token token)
	{
		return (int)token & (int)token_category::token_type_mask;
	}

	constexpr bool is_category(token_id token, token_category category)
	{
		return category_from_token(token) == category_from_token(category);
	}

	constexpr bool is_preprocessor_token(token_id token)
	{
		return (int)token & (int)token_category::preprocessor_flag;
	}


}
