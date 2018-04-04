#pragma once

#include <stack>
#include <map>
#include <memory>
#include "local_variable_table.h"
#include "instruction.h"
#include "syntax_tree.h"
#include "bytecode_appender.h"

namespace compiler
{
	using namespace std;
	
	class bytecode_generator_base
	{
	public:
		virtual void build(bytecode_appender &appender, AST ast) = 0;
	};

	class bytecode_inc : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_assign : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_binary_operator : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_unary_operator : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_func_call : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_constant : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_var : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_primitive_cast : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_if : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_while : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};

	class bytecode_return : public bytecode_generator_base
	{
	public:
		void build(bytecode_appender &appender, AST ast) override;
	};
}