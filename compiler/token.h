#pragma once

#include <memory>
#include <string>
#include <utility>

using std::string;
using std::move;

namespace compiler
{
	using namespace std;

	class token;

	typedef shared_ptr<token> token_ptr;

	enum class token_source_t
	{
		text,
		marco
	};

    class token
	{
    public:
		token();
		token(string code, int line_number, int line_column);

        string code;
        int line;
		int column;
		token_source_t source = token_source_t::text;

        bool eof();

		token_ptr super;
    };

}
