#include "stdafx.h"
#include "CppUnitTest.h"
#include "../compiler/object_t.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ObjectTest)
{
public:

	TEST_METHOD(test_object_t) {
		object_t o1(1);
		object_t o2(o1);
		int i = o2.cast<int>();
		Assert::AreEqual(i, 1);
	}

};