#include "CPUInfo.h"

void CPUInfo::Init()
{
    //initialize and check errors
    m_canReadCpu = true;

    //create query object pool for cpu usage
    PDH_STATUS pStatus = PdhOpenQuery(nullptr, 0, &m_queryHandle);
    if(pStatus != ERROR_SUCCESS)
    {m_canReadCpu = false; }

    //set query object to poll all cpus in the system
    pStatus = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
    if(pStatus != ERROR_SUCCESS)
    {m_canReadCpu = false; }

    m_lastSampleTime = GetTickCount();
    m_cpuUsage = 0;
    return;
}

void CPUInfo::Frame()
{
    PDH_FMT_COUNTERVALUE value;

    if (m_canReadCpu)
    {
        if ((m_lastSampleTime + 1000) < GetTickCount())
        {
            m_lastSampleTime = GetTickCount();

            PdhCollectQueryData(m_queryHandle);

            PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, nullptr, &value);

            m_cpuUsage = value.longValue;
        }
    }

    return;
}

void CPUInfo::ShutDown() const
{
    if (m_canReadCpu)
    {
        PdhCloseQuery(m_queryHandle);
    }
    return;
}

int CPUInfo::GetCpuPercentage() const
{
    int usage;

    if (m_canReadCpu)
    {
        usage = static_cast<int>(m_cpuUsage);
    }
    else
    {
        usage = 0;
    }

    return usage;
}
