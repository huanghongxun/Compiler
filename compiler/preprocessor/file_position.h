#pragma once

namespace compiler::preprocessor
{
	template<typename _String>
	struct file_position
	{
		file_position()
			: file(), line(1), column(1) {}

		explicit file_position(_String const& file, size_t line = 1, size_t column = 1)
			: file(file), line(line), column(column) {}

		_String file;
		size_t line, column;

		bool operator==(file_position<_String> const& b) const
		{
			return file == b.file && line == b.line && column == b.column;
		}
	};

	template<typename _String>
	struct position_policy<file_position<_String>>
	{
		size_t chars_per_tab;

		position_policy(size_t chars_per_tab = 0)
			: chars_per_tab(chars_per_tab) {}

		void next_line(file_position<_String>& pos)
		{
			pos.line++;
			pos.column = 1;
		}

		void next_char(file_postiion<_String>& pos)
		{
			pos.column++;
		}

		void tabulation(file_position<_String>& pos)
		{
			pos.column = pos.column + chars_per_tab - (pos.column - 1) % chars_per_tab;
		}
	};
}