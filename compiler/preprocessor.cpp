#include "preprocessor.h"
#include "string_utils.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
using namespace std;

compiler::preprocessor::preprocessor(program_ptr prog)
	: prog(prog)
{
}

compiler::program_ptr compiler::preprocessor::analyze()
{
	string code = prog->get_code();
	string command;
	istringstream ss(code);
	while (!ss.eof())
	{
		string line;
		getline(ss, line);

		if (boost::starts_with(line, "#include"))
		{

		}
	}

	return program_ptr();
}
