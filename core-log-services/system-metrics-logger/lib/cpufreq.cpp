#include "cpufreq.h"
#include "systeminfofilelocator.h"
#include <QFile>

bool CpuFreq::canGetValue()
{
    if(!SystemInfoFileLocator::getSysFreqPaths().isEmpty())
        return true;
    return false;
}

float CpuFreq::getValue()
{
    QList<QString> pathList = SystemInfoFileLocator::getSysFreqPaths();
    float sumFrequency = 0;
    int valueCount = 0;
    for(auto path : pathList) {
        QFile freqFile(path);
        if(freqFile.open(QIODevice::ReadOnly)) {
            QString readFreq = freqFile.readAll().trimmed();
            if(readFreq.toInt()) {
                sumFrequency += readFreq.toInt() / 1000;
                valueCount += 1;
            }
            else
                qWarning("Could not convert frequency file to number");
        }
    }
    if(valueCount != 0)
        return sumFrequency / valueCount;
    qWarning("Could not get current frequency");
    return sumFrequency;
}
