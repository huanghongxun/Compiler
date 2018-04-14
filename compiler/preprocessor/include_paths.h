#pragma once

#include <list>
#include <utility>
#include <boost/bimap.hpp>

#include "fs_util.h"

namespace compiler::preprocessor
{
	using namespace std;
	using namespace boost;

	template<typename _String>
	struct include_paths
	{
		typedef list<pair<filesystem::path, string>> include_list_type;
		typedef include_list_type::value_type include_value_type;
		typedef boost::bimap<string, string> pragma_once_set_type;

		include_paths()
			: was_sys_include_path(false) {}

		bool add_include_path(_String const& path, bool is_system)
		{
			return add_include_path(path, is_system ? system_include_paths : user_include_paths);
		}

		filesystem::path get_current_directory() const
		{
			return current_dir;
		}

		bool has_pragma_once(_String const& file_name)
		{
			return pragma_once_files.left.find(file_name) != pragma_once_files.left.end();
		}

		bool add_pragma_once_header(_String const& file_name, _String const& guard_name)
		{
			return pragma_once_files.insert(make_pair(file_name, guard_name)).second;
		}

		bool remove_pragma_once_header(_String const& guard_name)
		{
			if (pragma_once_files.right.find(guard_name) != pragma_once_files.right.end())
			{
				pragma_once_files.right.erase(guard_name);
				return true;
			}
			else
				return false;
		}



	private:
		bool add_include_path(_String const& path, include_list_type& paths)
		{
			auto newpath = filesystem::complete(filesystem::path(path), current_dir);
			if (!filesystem::exists(newpath) || !filesystem::is_directory(newpath))
				return false;
			paths.push_back(make_pair(newpath, path));
		}

		bool find_include_file(_String& s, _String& dir, include_list_type const& paths, _String const& current_file) const
		{
			namespace fs = filesystem;

			fs::path file_path(current_file);
			for (const auto &i : paths)
			{
				fs::path cur = create_path(i.first, s);

				if (fs::exists(cur))
				{
					fs::path dirpath = create_path(i.second, s);
					dir = dirpath.string();
					s = cur.normalize().string();
					return true;
				}
			}
			return false;
		}

		bool find_include_file(_String &s, _String &dir, bool is_system, _String const& current_file) const
		{
			namespace fs = filesystem;

			if (is_system)
			{
				return find_include_file(s, dir, system_include_paths, current_file);
			}
			else
			{
				return find_include_file(s, dir, user_include_paths, current_file);
			}
		}



		include_list_type user_include_paths;
		include_list_type system_include_paths;

		filesystem::path current_dir = filesystem::initial_path();
		
		pragma_once_set_type pragma_once_files;

	public:

	};
}