#pragma once
#include <mutex>
#include <stack>

template<class T, class ID = uint64_t> class IUniqueID
{
public:
	inline IUniqueID()
	{
		m_mutexUniqueId.lock();
		if(!m_freeUniqueId.empty())
		{
			m_uniqueId = m_freeUniqueId.top();
			m_freeUniqueId.pop();
		}
		else
		{
			m_uniqueId = m_nextUniqueId++;
		}
		m_mutexUniqueId.unlock();
	}
	inline virtual ~IUniqueID()
	{
		m_mutexUniqueId.lock();
		m_freeUniqueId.push(m_uniqueId);
		m_mutexUniqueId.unlock();
	}
	
	inline virtual ID UniqueId() const { return m_uniqueId; }
	
private:
	ID m_uniqueId;
	static ID m_nextUniqueId;
	static std::stack<ID> m_freeUniqueId;
	static std::mutex m_mutexUniqueId;
};

template<class T, class ID> ID IUniqueID<T, ID>::m_nextUniqueId = ID();
template<class T, class ID> std::stack<ID> IUniqueID<T, ID>::m_freeUniqueId;
template<class T, class ID> std::mutex IUniqueID<T, ID>::m_mutexUniqueId;
