#pragma once

#include <stack>

namespace compiler::preprocessor
{
	struct if_block
	{
		if_block() :
			status(true), some_part_status(true),
			enclosing_status(true), is_in_else(false)
		{}

		if_block(bool status, bool enclosing_status)
			: status(status), some_part_status(status),
			enclosing_status(enclosing_status), is_in_else(false)
		{}

		void set_status(bool status)
		{
			this->status = status;
			if (status)
				some_part_status = true;
		}

		bool status; // current block is true
		bool some_part_status;
		bool enclosing_status;
		bool is_in_else; // inside the #else part
	};

	struct if_block_stack : private std::stack<if_block>
	{
		void enter_if_block(bool new_status)
		{
			bool enclosing_status = get_status();
			emplace(new_status && enclosing_status, enclosing_status);
		}

		bool enter_elif_block(bool new_status)
		{
			if (!is_inside_ifpart())
				return false;

			if (get_enclosing_status())
			{
				if (get_status())
				{
					top().set_status(false);
				}
				else if (new_status && !top().some_part_status)
				{
					top().set_status(new_status);
				}
			}

			return true;
		}

		bool enter_else_block(bool new_status)
		{
			if (!is_inside_ifpart())
				return false;

			if (get_enclosing_status())
			{
				if (!top().some_part_status)
				{
					top().set_status(true);
				}
				else if (get_status())
				{
					top().set_status(false);
				}

				top().is_in_else = true;
			}

			return true;
		}

		bool exit_if_block()
		{
			if (empty())
				return false;

			pop();
			return true;
		}

		bool get_status() const
		{
			return empty() || top().status;
		}

		bool get_some_part_status() const
		{
			return empty() || top().some_part_status;
		}

		bool get_enclosing_status() const
		{
			return empty() || top().enclosing_status;
		}

		size_t get_if_block_depth() const
		{
			return size();
		}

	protected:
		bool is_inside_ifpart() const
		{
			return !empty() && !top().is_in_else;
		}

		bool is_inside_elsepart() const
		{
			return !empty() && top().is_in_else;
		}
	};
}