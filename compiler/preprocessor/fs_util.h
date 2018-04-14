#pragma once

#include <boost/filesystem.hpp>

template<typename Source>
boost::filesystem::path create_path(boost::filesystem::path root, Source cur)
{
	boost::filesystem::path p(cur);
	if (!p.has_root_directory())
	{
		p = boost::filesystem::path(root.string());
		p /= boost::filesystem::path(cur);
	}

	return p;
}