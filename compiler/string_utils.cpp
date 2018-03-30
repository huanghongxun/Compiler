#pragma once

#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "string_utils.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

bool is_hex_digit(char ch)
{
	return '0' <= ch && ch <= '9' || 'a' <= ch && ch <= 'f' || 'A' <= ch && ch <= 'F';
}

bool is_number(string s)
{
	erase_last(s, "l");
	erase_last(s, "l");
	erase_last(s, "f");
	to_lower(s);
	try
	{
		if (s.find('e') != string::npos)
		{
			vector<string> vec;
			split(vec, s, is_any_of("e"));

			if (vec.size() != 2)
				return false;

			lexical_cast<double>(vec[0]);
			lexical_cast<int>(vec[1]);
		}
		else
			lexical_cast<double>(s);
		return true;
	}
	catch (bad_lexical_cast&)
	{
		return false;
	}
}

bool is_name_char(int ch)
{
	return isdigit(ch) || isalpha(ch) || ch == '_';
}

bool is_name(const string &name)
{
	if (name.empty())
		return false;

	if (isdigit(name[0]))
		return false;

	for (char c : name)
		if (!is_name_char(c))
			return false;

	return true;
}

bool is_constant(const string &token)
{
	return !token.empty() && (isdigit(token[0]) || starts_with(token, "'") || starts_with(token, "\"") || token == "false" || token == "true");
}

int to_decimal(char hex)
{
	if (isdigit(hex))
		return hex - '0';
	else if ('a' <= hex && hex <= 'f')
		return hex - 'a' + 10;
	else if ('A' <= hex && hex <= 'f')
		return hex - 'A' + 10;
	else
		throw invalid_argument(string_format("Not a valid hex character %c.", hex));
}

int is_symbol(int s1, int s2, int s3) {
	static const char *SYMBOLS[] = { "+=", "-=", "*=", "/=", "|=", "&=", "%=", "^=", "<=", ">=", "==", "!=", "&&", "||", "<<", ">>",
		">>=", "<<=", "//", "/*", "*/", "...", "--", "++" };

	int s[] = { s1, s2, s3 };

	for (auto &i : SYMBOLS) {
		int *p = s;
		int j;
		for (j = 0; i[j]; ++j, ++p)
			if (*p != i[j]) {
				j = -1;
				break;
			}
		if (j != -1)
			return j;
	}
	return 0;
}

vector<char> parse_constant_string(string pattern)
{
	vector<char> chars;
	for (auto it = pattern.begin(); it != pattern.end(); ++it)
	{
		if (*it == '\\')
		{
			char next = *(it + 1);
			if (*(it + 1) == 'x')
			{
				if (it + 2 == pattern.end() || it + 3 == pattern.end())
					throw invalid_argument("No hexadecimal number after \\x");
				chars.push_back(to_decimal(*(it + 2)) * 16 + to_decimal(*(it + 3)));
				it += 3;
			}
			else if (next == '0')
				chars.push_back(0);
			else if (next == 'a')
				chars.push_back('\a');
			else if (next == 'b')
				chars.push_back('\b');
			else if (next == 'f')
				chars.push_back('\f');
			else if (next == '\'')
				chars.push_back('\'');
			else if (next == '"')
				chars.push_back('"');
			else if (next == '\\')
				chars.push_back('\\');
			else if (next == '\?')
				chars.push_back('\?');
			else if (next == 'v')
				chars.push_back('\v');
			else if (next == 't')
				chars.push_back('\t');
			else if (next == 'r')
				chars.push_back('\r');
			else if (next == 'n')
				chars.push_back('\n');
			else if (isdigit(*(it + 1)))
			{
				if (it + 2 == pattern.end() || it + 3 == pattern.end() || it + 4 == pattern.end())
					throw invalid_argument("No hexadecimal number after \\x");
				chars.push_back(to_decimal(*(it + 1)) * 64 + to_decimal(*(it + 2)) * 8 + to_decimal(*(it + 3)));
				it += 4;
			}
			else
				throw invalid_argument(string_format("Unknown escape \\%c", next));
		}
	}
	chars.push_back(0);
	return chars;
}
