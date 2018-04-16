#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include <string>
#include "../compiler/tokens.h"
#include "../compiler/lexical/lexical.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace compiler::parser;
using namespace compiler;
using namespace std;

vector<char> v;

TEST_CLASS(ParserTest)
{
public:

	struct obj_t
	{
		void operator()(char c) const
		{
			v.push_back(c);
		}
	};

	TEST_METHOD(test_parser) {
		auto ppsp = ch_p(token_id::space) | ch_p(token_id::block_comment);
		auto p = ch_p('A')[obj_t()] >> ch_p('B') >> (ch_p('C') | ch_p('D'));
		string str = "ABC";
		Assert::AreEqual(true, parse(str.begin(), str.end(), p).hit);
		str = "ABC123";
		Assert::AreEqual(true, parse(str.begin(), str.end(), p >> (int_p | real_p)).hit);
		Assert::AreEqual(false, parse(str.begin(), str.end(), p >> (int_p ^ real_p)).hit);
		str = "AAAAAA";
		Assert::AreEqual(true, parse(str.begin(), str.end(), *ch_p('A')).hit);
		str = "ABA";
		Assert::AreEqual(true, parse(str.begin(), str.end(), ch_p('A') >> !ch_p('B') >> ch_p('A')).hit);
		str = "AA";
		Assert::AreEqual(true, parse(str.begin(), str.end(), ch_p('A') >> !ch_p('B') >> ch_p('A')).hit);


		str = "123";
		Assert::AreEqual(true, parse(str.begin(), str.end(), int_p).hit);

		str = "1.123e+18";
		Assert::AreEqual(true, parse(str.begin(), str.end(), real_p).hit);
		str = "1.123e";
		Assert::AreEqual(false, parse(str.begin(), str.end(), real_p).hit);
	}

};