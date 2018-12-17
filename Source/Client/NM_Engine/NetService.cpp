#include "NM_Index.h"
#include "NM_Main.h"
#include "NetLib.h"

size_t NetService::Run()
{
	return m_service.run();
}

size_t NetService::Poll()
{
	return m_service.poll();
}

void NetService::Stop()
{
	m_service.stop();
}

bool NetService::HasStopped()
{
	return m_service.stopped();
}


boost::asio::io_service& NetService::operator()()
{
	return m_service;
}

