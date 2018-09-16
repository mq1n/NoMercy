
#pragma once

#include <memory>
#include <vector>
#include <mutex>

namespace clan
{
	class SocketHandle;
	class NetworkConditionVariableImpl;

	/// \brief Base class for all classes that generate network events
	class NetworkEvent
	{
	protected:
		virtual SocketHandle *get_socket_handle() = 0;

		friend class NetworkConditionVariable;
	};

	/// \brief Condition variable that also awaken on network events
	class NetworkConditionVariable
	{
	public:
		NetworkConditionVariable();

		/// \brief Waits for event changes or until notify is called
		template<typename Lock>
		bool wait(Lock &lock, int count, NetworkEvent **events, int timeout = -1)
		{
			lock.unlock();
			try
			{
				bool result = wait_impl(count, events, timeout);
				lock.lock();
				return result;
			}
			catch (...)
			{
				lock.lock();
				throw;
			}
		}

		/// \brief Awakens any thread waiting for event changes
		void notify();

	private:
		bool wait_impl(int count, NetworkEvent **events, int timeout);

		std::shared_ptr<NetworkConditionVariableImpl> impl;
	};
}
