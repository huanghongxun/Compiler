#pragma once

#include <memory>
#include "../tokens.h"

namespace compiler::preprocessor
{
	using namespace std;

	template<typename _String, typename _Position>
	struct token_data
	{
		typedef _String string_type;
		typedef _Position position_type;

		token_data() : id(token_id::eoi) {}

		explicit token_data(int) : id(token_id::unknown) {}

		token_data(token_id id, string_type const& value, position_type const& pos)
			: id(id), value(value), pos(pos) {}

		token_data(token_data const& that)
			: token_data(that.id, that.value, that.pos) {}

		bool operator==(token_data const& that) const
		{
			return id == that.id && value == that.value;
		}

		operator token_id() const
		{
			return id;
		}

		string_type const& get_value() const
		{
			return value;
		}

		position_type const& get_position() const
		{
			return pos;
		}

		void set_token_id(token_id id)
		{
			this->id = id;
		}

		void set_value(string_type const& val)
		{
			this->value = val;
		}

		void set_position(position_type const& pos)
		{
			this->pos = pos;
		}

		token_id id;
		string_type value;
		position_type pos;
	};
	
	template<typename _String, typename _Position>
	struct lex_token
	{
		typedef _String string_type;
		typedef _Position position_type;
		typedef token_data<string_type, position_type> data_type;

		lex_token() : data(0) {}

		explicit lex_token(int) : data(new data_type(0)) {}

		lex_token(token_id id, string_type const& value, position_type const& pos)
			: data(new data_type(id, value, pos)) {}

		string_type const& get_value() const
		{
			return data->get_value();
		}

		position_type const& get_position() const
		{
			return data->get_position();
		}

		bool is_eoi() const
		{
			return data == nullptr || token_id(*data) == token_id::eoi;
		}

		bool is_valid() const
		{
			return data != nullptr && token_id(*data) != token_id::unknown;
		}

		void set_token_id(token_id id)
		{
			data->set_token_id(id);
		}

		void set_value(string_type const& val)
		{
			data->set_value(val);
		}

		void set_position(position_type const& pos)
		{
			data->set_position(pos);
		}

	private:

		shared_ptr<data_type> data;
	};

	template<typename _String, typename _Position>
	bool is_token_valid(lex_token<_String, _Position> const& t)
	{
		return t.is_valid();
	}

	template<typename _String, typename _Position, typename _Iterator>
	struct lex_iterator
	{
		lex_iterator(_Iterator now, _Iterator end)
			: now(now), end(end)
		{}

		lex_iterator(_Iterator now, _Iterator end, _Position pos)
			: now(now), end(end), pos(pos)
		{}

		_String get_current_line() const
		{
			return now.get_current_line();
		}

		_Position const& get_position() const
		{
			return pos;
		}

		lex_iterator& operator++()
		{
			next();
			return *this;
		}

		lex_iterator operator+(size_t offset) const
		{
			lex_iterator it = *this;
			for (size_t i = 0; i < offset; ++i)
				++it;
			return it;
		}



	private:


		_Iterator now, end, line_begin;
		_Position pos;
	};
}