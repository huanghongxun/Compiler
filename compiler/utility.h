#pragma once

#include <functional>

template<typename T, typename... Args>
std::function<bool(T)> is_any_of(T t0, Args... args)
{
	T t[] = { t0, args... };
	return [t](const T &a)
	{
		for (auto &i : t)
			if (i == a)
				return true;
		return false;
	};
}