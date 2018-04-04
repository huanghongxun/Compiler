#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../compiler/unit_assert.h"

#undef assert_eq
#define assert_eq Assert::AreEqual
#include "../compiler/program.h"
#include "../compiler/lexical_analyzer.h"
#include "../compiler/syntax_analyzer.h"
#include "../compiler/semantic_analyzer.h"
#include "../compiler/bytecode_generator.h"
#include "../compiler/virtual_machine.h"

using namespace compiler;

void _assert_eq(int a, int b)
{
	Assert::AreEqual(a, b);
}

void _assert_neq(int a, int b)
{
	Assert::AreNotEqual(a, b);
}

void _assert_eq_double(double a, double b)
{
	Assert::AreEqual(a, b);
}

void _assert_neq_double(double a, double b)
{
	Assert::AreNotEqual(a, b);
}

void test(const string &code)
{
	program_ptr p = make_shared<program>("void __built_in assert(int a, int b);\n"
		"void __built_in assert_neq(int a, int b);\n"
		"void __built_in assert_eq_double(double a, double b);\n"
		"void __built_in assert_neq_double(double a, double b);\n" + code);
	lexical_analyzer analyzer(p);
	auto res = analyzer.analyze();
	syntax_analyzer sa(p, res);
	auto tree = sa.analyze();
	semantic_analyzer sem(p);
	sem.analyze(tree);
	bytecode_generator generator(p);
	virtual_machine vm(generator.build(tree));

	vm.register_built_in_func("assert", shared_ptr<built_in_function>(new built_in_function_2<void, int, int>(_assert_eq)));
	vm.register_built_in_func("assert_neq", shared_ptr<built_in_function>(new built_in_function_2<void, int, int>(_assert_neq)));
	vm.register_built_in_func("assert_eq_double", shared_ptr<built_in_function>(new built_in_function_2<void, double, double>(_assert_eq_double)));
	vm.register_built_in_func("assert_neq_double", shared_ptr<built_in_function>(new built_in_function_2<void, double, double>(_assert_neq_double)));

	vm.init();
	vm.start();
}

TEST_CLASS(VirtualMachineTest)
{
public:

	TEST_METHOD(test_vm_digit_dec) {
		test("int main()\n"
			"{\n"
			"assert(3, 3); assert_neq(5, 4);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_digit_hex) {
		test("int main()\n"
			"{\n"
			"assert(0x10, 16); assert_neq(0x11, 16);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_digit_oct) {
		test("int main()\n"
			"{\n"
			"assert(010, 8); assert_neq(011, 8);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_cast_primitive) {
		test("int main()\n"
			"{\n"
			"assert((int) 2.1, 2);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_cast_pointer) {
		test("int main()\n"
			"{\n"
			"float f = 23.0;\n"
			"assert(*((int*)&f), 1102577664);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_digit_double) {
		test("int main()\n"
			"{\n"
			"double d = 2.1;\n"
			"assert_neq_double(d, 2.2);\n"
			"d *= 2;\n"
			"assert_eq_double(d, 4.2);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_digit_scientific) {
		test("int main()\n"
			"{\n"
			"assert_eq_double(1e2, 100.0);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_global_int_var) {
		test("int i = 8;\n"
			"int main()\n"
			"{\n"
			"i = ((i * 9 + 5) % 9 + 1) / 2;\n"
			"assert(i, 3); assert_neq(i, 4);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_static_var) {
		test("int test() {\n"
			"    static int i = 1;\n"
			"	 return i += 1;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"assert((test(), test(), test()), 4);\n"
			"assert(test(), 5);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_global_array) {
		test("int arr[10];\n"
			"int main()\n"
			"{\n"
			"arr[0] = 1;\n"
			"assert(arr[0], 1);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_fibonacci) {
		test("int arr[10];\n"
			"int main()\n"
			"{\n"
			"arr[0] = 0; arr[1] = 1;\n"
			"for (int i = 2; i < 10; i++)\n"
			"	 arr[i] = arr[i - 1] + arr[i - 2];\n"
			"assert(arr[1], 1);\n"
			"assert(arr[2], 1);\n"
			"assert(arr[3], 2);\n"
			"assert(arr[4], 3);\n"
			"assert(arr[5], 5);\n"
			"assert(arr[6], 8);\n"
			"assert(arr[7], 13);\n"
			"assert(arr[8], 21);\n"
			"assert(arr[9], 34);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_two_dim_array) {
		test("int arr[10][10];\n"
			"int main()\n"
			"{\n"
			"for (int i = 0; i < 10; ++i) arr[i][0] = 0;\n"
			"for (int i = 0; i < 10; i++) arr[0][i] = 0;\n"
			"arr[1][0] = 1;\n"
			"for (int i = 1; i < 10; ++i)\n"
			"    for (int j = 1; j < 10; ++j)\n"
			"	     arr[i][j] = arr[i - 1][j] + arr[i][j - 1];\n"
			"assert(arr[9][9], 12870);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_sizeof) {
		test("double arr[10][10];\n"
			"int main()\n"
			"{\n"
			"assert(sizeof(arr), 800);\n"
			"assert(sizeof(int), 4);\n"
			"assert(sizeof(void*), 4);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_global_simple_pointer) {
		test("int main()\n"
			"{\n"
			"int i = 2;\n"
			"assert(*(&i), 2);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_global_pointer_move) {
		test("int main()\n"
			"{\n"
			"int i = 2, j = 4, k = 7; double a = 2.5, b = 3.2e-1, c = 4.4 / 2;\n"
			"assert(*(&i + 1), 4);\n"
			"assert(*(&i + 2), 7);\n"
			"assert(*(&j + 1), 7);\n"
			"assert(*(&k - 2), 2);\n"
			"assert(*(&j - 1), 2);\n"
			"assert(*(&k - 1), 4);\n"
			"assert_eq_double(*(&b - 1), 2.5);\n"
			"assert_eq_double(*(&b + 1), 2.2);\n"
			"assert_eq_double(*(&a + 2), 2.2);\n"
			"assert_eq_double(*(&c - 2), 2.5);\n"
			"assert_neq(*(&j + 1), 4);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_simple_func) {
		test("int test() { return 1 + 1; }\n"
			"int main()\n"
			"{\n"
			"assert(test(), 2); assert_neq(test(), 3);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_func_arg) {
		test("int test(int i) { return i * 2; }\n"
			"int main()\n"
			"{\n"
			"assert(test(1), 2); assert(test(5), 10); assert_neq(test(5), 12);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_func_recursive) {
		test("int factorial(int i)\n"
			"{\n"
			"if (i == 0) return 1; \n"
			"else return factorial(i - 1) * i;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"assert(factorial(5), 120); assert_neq(factorial(3), 5);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_comma_expression) {
		test("int main()\n"
			"{\n"
			"int i;\n"
			"i = (2, 5);\n"
			"assert(5, i);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_if_else) {
		test("int main()\n"
			"{\n"
			"int i = 2;\n"
			"if (i == 2)\n"
			"	 i = (i * 7 % 3);\n"
			"else"
			"	 i = 0;\n"
			"assert(2, i);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_for) {
		test("int factorial(int i)\n"
			"{\n"
			"if (i == 0) return 1; \n"
			"else return factorial(i - 1) * i;\n"
			"}\n"
			"int main()\n"
			"{ int ans = 0; for (int i = 0; i < 5; i = i + 1) ans = ans + factorial(i); \n"
			"assert(ans, 34);\n"
			"return 0;\n"
			"}");
	}

	TEST_METHOD(test_vm_no_built_in) {
		Assert::ExpectException<built_in_function_not_found>([]() {
			test("#include <iostream>\n"
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
				"    scanf(\"%d \\n \\t \\f \", &local);\n"
				"    printf(\"%d\", local);\n"
				"    return 0;\n"
				"}");
		});
	}
};