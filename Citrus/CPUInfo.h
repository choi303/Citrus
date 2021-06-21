#pragma once
#pragma comment(lib, "pdh.lib")
#include <Pdh.h>

class CPUInfo
{
public:
	CPUInfo()=default;
	CPUInfo(const CPUInfo&)=default;
	~CPUInfo()=default;
	void Init();
	void Frame();
	void ShutDown() const;
	int GetCpuPercentage() const;
private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

