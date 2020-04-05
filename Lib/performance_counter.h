#pragma once

#include<chrono>

class PerformanceCounter
{
public:
	void start()
	{
		mStart = std::chrono::system_clock::now();
	}

	void end()
	{
		mEnd = std::chrono::system_clock::now();
	}

	template<class T = double>
	T getMicroseconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(mEnd - mStart).count();
	}

	template<class T = double>
	T getMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(mEnd - mStart).count();
	}

	template<class T = double>
	T getMinutes()
	{
		return std::chrono::duration_cast<std::chrono::minutes>(mEnd - mStart).count();
	}

private:
	std::chrono::system_clock::time_point mStart;
	std::chrono::system_clock::time_point mEnd;
};