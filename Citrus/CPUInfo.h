#pragma once
#pragma comment(lib, "pdh.lib")
#include <Pdh.h>
#include <wrl.h>

class CPUInfo
{
public:
	CPUInfo();
	CPUInfo(const CPUInfo&);
	~CPUInfo();
	void Init();
	void Frame();
	void ShutDown();
	int GetCpuPercentage();
private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

