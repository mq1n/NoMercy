#pragma once
#include <chrono>

template <typename T>
class CTimer
{
	public:
		CTimer()
		{
			m_tStartPoint = std::chrono::high_resolution_clock::now();
		}
		~CTimer() = default;


		auto diff()
		{
			return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - m_tStartPoint).count();
		}

		void reset()
		{
			m_tStartPoint = std::chrono::high_resolution_clock::now();
		}

	private:
		std::chrono::time_point <std::chrono::high_resolution_clock> m_tStartPoint;
};
