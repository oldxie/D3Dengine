#pragma once
#include <chrono>

using namespace std::chrono;
class ChiliTimer
{
public:
	ChiliTimer() noexcept
	{
		last = steady_clock::now();
	}

	float Mark() noexcept
	{
		const auto old = last;
		last = steady_clock::now();
		const duration<float> frameTime = last - old;
		return frameTime.count();
	}

	float Peek() const noexcept
	{
		return duration<float>(steady_clock::now() - last).count();
	}
private:
	std::chrono::steady_clock::time_point last;
};



