#include "stdafx.h"
#include "CppUnitTest.h"
#include "../compiler/program.h"
#include "../compiler/lexical_analyzer.h"
#include "../compiler/syntax_analyzer.h"
#include "../compiler/semantic_analyzer.h"
#include "../compiler/bytecode_generator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace compiler;

TEST_CLASS(BytecodeGeneratorTest)
{
public:

	TEST_METHOD(test_bytecode) {
		string code = "void __built_in assert(bool b);\n"
			"int i = 8;\n"
			"int main()\n"
			"{\n"
			"	 i = ((i * 9 + 8) % 9 + 1) / 2;\n"
			"	 assert(i == 4);\n"
			"	 return 0;\n"
			"}";

		program_ptr p = make_shared<program>("main.cpp", code);
		lexical_analyzer analyzer(p);
		auto res = analyzer.analyze();
		syntax_analyzer sa(p, res);
		auto tree = sa.analyze();
		semantic_analyzer sem(p);
		sem.analyze(tree);
		bytecode_generator generator(p);
		bytecode bc = generator.build(tree);
		bc.get_func("main");
	}

};