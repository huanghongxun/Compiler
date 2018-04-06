#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdio>

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

template<typename T>
std::string joins(const std::string &delimiter, const T &arg0) {
	return arg0;
}

template<typename T, typename ... Ts>
std::string joins(const std::string& delimiter, const T &arg0, const Ts& ... args) {
	return arg0 + delimiter + joins(delimiter, args...);
}

bool is_hex_digit(char ch);

bool is_number(std::string s);

bool is_name_char(int ch);

bool is_name(const std::string &name);

bool is_constant(const std::string &token);

int is_symbol(int s1, int s2, int s3 = 0);

std::vector<char> parse_constant_string(std::string pattern);

template<typename T>
struct lexical_hex
{
	T value;

	operator T() const
	{
		return value;
	}

	friend std::istream& operator>>(std::istream& in, lexical_hex& out)
	{
		in >> std::hex >> out.value;
		return in;
	}
};

template<typename T>
struct lexical_oct
{
	T value;

	operator T() const
	{
		return value;
	}

	friend std::istream& operator>>(std::istream& in, lexical_oct& out)
	{
		in >> std::oct >> out.value;
		return in;
	}
};