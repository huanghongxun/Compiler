#include "stdafx.h"
#include "CppUnitTest.h"
#include <iostream>
#include "../compiler/lexical_analyzer.h"
#include "../compiler/syntax_analyzer.h"
#include "../compiler/semantic_analyzer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace compiler;
using namespace std;

TEST_CLASS(SyntaxAnalyzerTest)
{
public:

	void test(const string &code)
	{
		program_ptr p = make_shared<program>(code);
		lexical_analyzer analyzer(p);
		auto tokens = analyzer.analyze();
		syntax_analyzer syn(p, tokens);
		auto res = syn.analyze();
		semantic_analyzer sem(p);
		sem.analyze(res);

		stringstream ss;
		print(ss, res, 0);
		string str = ss.str();
		Logger::WriteMessage(str.c_str());
	}

	TEST_METHOD(test_syntax_analyzer_1) {
		test("#include <stdio.h>\n"
			"#include <stdlib.h>\n"
			"#include <math.h>\n"
			"using namespace std;\n"
			"// this is a line comment\n"
			"int i;\n"
			"double d;\n"
			"/* this is \n"
			" a block comment.*/\n"
			"int __built_in scanf(const char* fmt, ...);\n"
			"int __built_in printf(const char* fmt, ...);\n"
			"int main() {\n"
			"    int local;\n"
			"    scanf(\"%d \\n \\t \\f \", &local);\n"
			"    printf(\"%d\", local);\n"
			"    return 0;\n"
			"}");
	}

	TEST_METHOD(test_syntax_analyzer_built_in_func)
	{
		test("void __built_in assert(bool b); int i = 8;\n"
			"int main()\n"
			"{\n"
			"i = ((i * 9 + 8) % 9 + 1) / 2;\n"
			"assert(i == 4);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_syntax_analyzer_global_var) {
		test("void __built_in assert(bool b); int i = 8;\n"
			"int main()\n"
			"{\n"
			"i = ((i * 9 + 8) % 9 + 1) / 2;\n"
			"assert(i == 4);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_syntax_analyzer_array_access) {
		Assert::ExpectException<compilation_error>([this]() {
			test("int arr[10];\n"
				"int main()\n"
				"{\n"
				"arr[1.5];\n"
				"return 0;\n"
				"}");
		});
	}

	TEST_METHOD(test_syntax_analyzer_array_assigning) {
		Assert::ExpectException<compilation_error>([this]() {
			test("int arr[10];\n"
				"int main()\n"
				"{\n"
				"int i;\n"
				"arr = &i;\n"
				"return 0;\n"
				"}");
		});
	}

	TEST_METHOD(test_syntax_analyzer_increment_lvalue) {
		// Since i++ has higher priority than ++i has,
		// ++++i++ can be considered as ++(++(i++)).
		// As i++ is not a lvalue, ++(i++) can not be compiled.
		Assert::ExpectException<compilation_error>([this]() {
			test("int arr[10];\n"
				"int main()\n"
				"{\n"
				"int i = 0;\n"
				"++++i++;\n"
				"return 0;\n"
				"}");
		});
	}

	void print(stringstream &ss, AST tree, int depth)
	{
		for (int i = 0; i < depth; ++i)
			ss << ' ';
		ss << tree->desc.type().name() << endl;
		for (auto &a : tree->children)
			print(ss, a, depth + 1);
	}
};