#include "dspinterruptlogger.h"
#include "timerfactoryqt.h"

DspInterruptLogger::DspInterruptLogger(int periodMs)
{
    m_timer = TimerFactoryQt::createPeriodic(periodMs);
    connect(m_timer.get(), &TimerTemplateQt::sigExpired,
            this, &DspInterruptLogger::onTimer);
    m_timer->start();
}

void DspInterruptLogger::addInterruptCount(int perIntInterruptCount)
{
    m_interruptCounts.append(perIntInterruptCount);
}

void DspInterruptLogger::onTimer()
{
    if(!m_interruptCounts.isEmpty()) {
        int min = *std::min_element(m_interruptCounts.begin(), m_interruptCounts.end());
        int max = *std::max_element(m_interruptCounts.begin(), m_interruptCounts.end());
        float mean = std::accumulate(m_interruptCounts.begin(), m_interruptCounts.end(), 0.0)/m_interruptCounts.size();
        m_interruptCounts.clear();
        emit sigLogNewData(min, max, mean);
    }
}
