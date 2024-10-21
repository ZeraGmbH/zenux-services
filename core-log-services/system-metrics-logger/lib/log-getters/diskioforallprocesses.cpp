#include "diskioforallprocesses.h"
#include "procpiddecoder.h"
#include "procpidiodecoder.h"

bool DiskIoForAllProcesses::canGetValue()
{
    return true;
}

DiskValuesProcesses DiskIoForAllProcesses::getValue()
{
    QVector<int> pids = ProcPidDecoder::getPids();
    const DiskValuesProcesses currentValues = ProcPidIoDecoder::getPidGroupReadWrites(pids);
    DiskValuesProcesses diffValues;
    for(auto iter=currentValues.cbegin(); iter!=currentValues.cend(); ++iter) {
        const int pid = iter.key();
        const auto values = iter.value();
        if(m_lastValues.contains(pid)) {
            DiskValues setVal;
            if(values.bytesRead > m_lastValues[pid].bytesRead)
                setVal.bytesRead = values.bytesRead - m_lastValues[pid].bytesRead;
            if(values.bytesWritten > m_lastValues[pid].bytesWritten)
                setVal.bytesWritten = values.bytesWritten - m_lastValues[pid].bytesWritten;
            diffValues[pid] = setVal;
        }
    }
    m_lastValues = currentValues;
    return diffValues;
}
