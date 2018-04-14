#include "syntax_parsers.h"
#include "syntax_descriptors.h"

using namespace compiler::syntax;

compiler::parser_if::parser_if(syntax_analyzer * analyzer)
	: syntax_parser(analyzer)
{
}

compiler::AST compiler::parser_if::parse() {
	if (!m_syntax_analyzer->peek_token("if"))
		return nullptr;

	AST ast = make_shared<syntax_tree>(descriptor_if(), m_syntax_analyzer->peek_token_with_code_info());

	m_syntax_analyzer->assert_next_token("if");
	m_syntax_analyzer->assert_next_token("(");
	ast->add_children(m_syntax_analyzer->parse_expression());
	m_syntax_analyzer->assert_next_token(")");
	ast->add_children(m_syntax_analyzer->parse_packed_statements());
	if (m_syntax_analyzer->peek_token("else")) {
		m_syntax_analyzer->assert_next_token("else");
		ast->add_children(m_syntax_analyzer->parse_packed_statements());
	}

	return ast;
}

compiler::AST compiler::parser_for::parse_for1() {
	if (is_name(m_syntax_analyzer->peek_token(0)) && is_name(m_syntax_analyzer->peek_token(1)))
	{
		return m_syntax_analyzer->parse_define_vars();
	}
	else
	{
		AST ast = m_syntax_analyzer->parse_expression();
		m_syntax_analyzer->assert_next_token(";");
		return ast;
	}
}

compiler::parser_for::parser_for(syntax_analyzer * analyzer)
	: syntax_parser(analyzer)
{
}

compiler::AST compiler::parser_for::parse() {
	if (!m_syntax_analyzer->peek_token("for"))
		return nullptr;

	AST ast = make_shared<syntax_tree>(descriptor_statements(), m_syntax_analyzer->peek_token_with_code_info());

	m_syntax_analyzer->assert_next_token("for");
	m_syntax_analyzer->assert_next_token("(");
	if (!m_syntax_analyzer->peek_token(";"))
		ast->add_children(parse_for1());

	AST nest = make_shared<syntax_tree>(descriptor_while(), m_syntax_analyzer->peek_token_with_code_info());
	if (m_syntax_analyzer->peek_token(";"))
		nest->add_children(make_shared<syntax_tree>(descriptor_constant("true"), m_syntax_analyzer->peek_token_with_code_info()));
	else
		nest->add_children(m_syntax_analyzer->parse_expression());

	m_syntax_analyzer->assert_next_token(";");

	AST f = nullptr;
	if (!m_syntax_analyzer->peek_token(")"))
		f = m_syntax_analyzer->parse_expression();
	m_syntax_analyzer->assert_next_token(")");

	AST statements = make_shared<syntax_tree>(descriptor_statements(), m_syntax_analyzer->peek_token_with_code_info());
	statements->add_children(m_syntax_analyzer->parse_packed_statements());
	if (f != nullptr)
		statements->add_children(f);

	nest->add_children(statements);
	ast->add_children(nest);

	return ast;
}

compiler::parser_while::parser_while(syntax_analyzer * analyzer)
	: syntax_parser(analyzer)
{
}

compiler::AST compiler::parser_while::parse()
{
	if (!m_syntax_analyzer->peek_token("while"))
		return nullptr;

	AST ast = make_shared<syntax_tree>(descriptor_while(), m_syntax_analyzer->peek_token_with_code_info());

	m_syntax_analyzer->assert_next_token("while");
	m_syntax_analyzer->assert_next_token("(");
	ast->add_children(m_syntax_analyzer->parse_expression());
	m_syntax_analyzer->assert_next_token(")");
	ast->add_children(m_syntax_analyzer->parse_packed_statements());

	return ast;
}

compiler::parser_return::parser_return(syntax_analyzer * analyzer)
	: syntax_parser(analyzer)
{
}

compiler::AST compiler::parser_return::parse()
{
	if (!m_syntax_analyzer->peek_token("return"))
		return nullptr;

	AST ast = make_shared<syntax_tree>(descriptor_return(), m_syntax_analyzer->peek_token_with_code_info());

	m_syntax_analyzer->assert_next_token("return");
	ast->add_children(m_syntax_analyzer->parse_expression());
	m_syntax_analyzer->assert_next_token(";");

	return ast;
}

compiler::parser_break::parser_break(syntax_analyzer * analyzer)
	: syntax_parser(analyzer)
{
}

compiler::AST compiler::parser_break::parse()
{
	if (!m_syntax_analyzer->peek_token("break"))
		return nullptr;

	AST ast = make_shared<syntax_tree>(descriptor_break(), m_syntax_analyzer->peek_token_with_code_info());

	m_syntax_analyzer->assert_next_token("break");
	m_syntax_analyzer->assert_next_token(";");

	return ast;
}

compiler::parser_continue::parser_continue(syntax_analyzer * analyzer)
	: syntax_parser(analyzer)
{
}

compiler::AST compiler::parser_continue::parse()
{
	if (!m_syntax_analyzer->peek_token("continue"))
		return nullptr;

	AST ast = make_shared<syntax_tree>(descriptor_continue(), m_syntax_analyzer->peek_token_with_code_info());

	m_syntax_analyzer->assert_next_token("continue");
	m_syntax_analyzer->assert_next_token(";");

	return ast;
}
