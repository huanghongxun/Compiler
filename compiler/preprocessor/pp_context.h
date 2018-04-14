#pragma once

#include <filesystem>
#include <string>
#include "../lexical/nil_t.h"

namespace compiler::preprocessor
{
	using namespace std;

	template<typename _Iterator, typename _LexIterator, typename _String, typename _Derived = nil_t>
	struct context
	{
		typedef iterator_type = pp_iterator<context>

		_Iterator first, last;
		char const* file_name, current_file_name;
		bool initialized = false;

		context(_Iterator const& first, _Iterator const& last, char const* file_name)
			: first(first), last(last), file_name(file_name), current_file_name(file_name)
		{

		}

		_Iterator begin()
		{
			return _Iterator(*this, first, last, position_type(file_name));
		}

		_Iterator end()
		{
			return _Iterator();
		}

		bool add_include_path(_String const& path)
		{
			return includes.add_include_path(path);
		}

		bool add_sysinclude_path(_String const& path)
		{
			return includes.add_sysinclude_path(path);
		}

		bool add_marco_definition(_String const& marco, bool is_predefined = false)
		{

		}

		template<typename _Container>
		bool add_macco_definition(_String const& name, bool has_params, _Container const& parameters, bool is_predefined = false)
		{
			macros.add_macro()
		}

		bool is_macro_defined(_String const& name) const
		{
			return macros.is_defined(name);
		}

		bool remove_macro_definition(_String const& name, bool even_predefined = false)
		{
			includes.remove_pragma_once_header(name);
			return macros.remove_macro(name, macros.get_main_pos(), even_predefined);
		}

		void reset_macro_definitions()
		{
			macros.reset_macromap();
			macros.init_predefined_macros();
		}

		name_iterator macro_names_begin()
		{
			return macros.begin();
		}

		name_iterator macro_names_end()
		{
			return macros.end();
		}

		const_name_iterator macro_names_begin() const
		{
			return macros.begin();
		}

		const_name_iterator macro_names_end() const
		{
			return macros.end();
		}

		void init_context()
		{
			if (!initialized)
			{
				includes.set_current_directory(file_name);
				initialized = true;
			}
		}

		bool has_pragma_once()
	};
}