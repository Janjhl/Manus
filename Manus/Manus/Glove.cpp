#include "stdafx.h"
#include "Glove.h"
#include "hidapi.h"

Glove::Glove(const char* device_path)
	: m_running(false)
	, m_packets(0)
{
	memset(&m_report, 0, sizeof(m_report));
	m_thread = std::thread(DeviceThread, this, device_path);
}

Glove::~Glove()
{
	// Instruct the device thread to stop and
	// wait for it to shut down.
	m_running = false;
	if (m_thread.joinable())
		m_thread.join();
}

bool Glove::GetState(GLOVE_STATE* state)
{
	// TODO: Convert report to state
	return false;
}

void Glove::DeviceThread(Glove* glove, const char* device_path)
{
	hid_device* device = hid_open_path(device_path);
	if (!device)
		return;

	glove->m_running = true;

	// Keep retrieving reports while the SDK is running and the device is connected
	while (glove->m_running && device)
	{
		GLOVE_REPORT report;
		int read = hid_read(device, (unsigned char*)&report, sizeof(report));

		if (read == -1)
			break;

		if (read == sizeof(report))
		{
			glove->m_report = report;
			glove->m_packets++;
		}
	}

	hid_close(device);

	glove->m_running = false;
}
