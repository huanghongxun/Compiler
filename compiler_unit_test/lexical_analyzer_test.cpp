#include "stdafx.h"
#include "CppUnitTest.h"
#include "../compiler/lexical_analyzer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace compiler;

TEST_CLASS(LexicalAnalyzerTest)
{
public:

	TEST_METHOD(test_lexical_analyzer_1) {
		string code = "#include <iostream>\n"
			"#include <utility>\n"
			"#include <cstdio>\n"
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
			"    scanf(\"%d \\n \\t \\f\", &local);\n"
			"    printf(\"%d\", local);\n"
			"    return 0;\n"
			"}";


		program_ptr p = make_shared<program>(code);
		lexical_analyzer analyzer(p);
		auto res = analyzer.analyze();
		Assert::AreEqual(61, (int)res.size());
	}

	TEST_METHOD(test_lexical_analyzer_digit_hex) {
		program_ptr p = make_shared<program>("0x7FFFFFFF");
		Assert::AreEqual(1, (int)lexical_analyzer(p).analyze().size());
	}

	TEST_METHOD(test_lexical_analyzer_digit_double) {
		program_ptr p = make_shared<program>("2.5");
		Assert::AreEqual(1, (int)lexical_analyzer(p).analyze().size());
	}

	TEST_METHOD(test_lexical_analyzer_digit_oct) {
		program_ptr p = make_shared<program>("0800l");
		Assert::AreEqual(1, (int)lexical_analyzer(p).analyze().size());
	}

	TEST_METHOD(test_lexical_analyzer_digit_scientific) {
		program_ptr p = make_shared<program>("1e9");
		Assert::AreEqual(1, (int)lexical_analyzer(p).analyze().size());
		p = make_shared<program>("1.0e+9");
		Assert::AreEqual(1, (int)lexical_analyzer(p).analyze().size());
		p = make_shared<program>("9.0e-9");
		Assert::AreEqual(1, (int)lexical_analyzer(p).analyze().size());
	}

	TEST_METHOD(test_lexical_analyzer_string)
	{
		Assert::ExpectException<compilation_error>([this]() {
			program_ptr p = make_shared<program>("#include <iostream>\n"
				"#include <utility>\n"
				"#include <cstdio>\n"
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
				"    scanf(\"%d \\n \\t \\f \\x\", &local);\n" // \x000X
				"    printf(\"%d\", local);\n"
				"    return 0;\n"
				"}");
			lexical_analyzer(p).analyze();
		});
	}

};