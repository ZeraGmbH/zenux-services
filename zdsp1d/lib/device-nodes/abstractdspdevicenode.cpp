#include <abstractdspdevicenode.h>

int AbstractDspDeviceNode::m_readTransactions = 0;
int AbstractDspDeviceNode::m_writeTransactions = 0;

int AbstractDspDeviceNode::getReadTransactions()
{
    return m_readTransactions;
}

int AbstractDspDeviceNode::getWriteTransactions()
{
    return m_writeTransactions;
}

void AbstractDspDeviceNode::resetAllTransactions()
{
    m_readTransactions = 0;
    m_writeTransactions = 0;
}

void AbstractDspDeviceNode::incReadTransactionCount()
{
    m_readTransactions++;
}

void AbstractDspDeviceNode::incWriteTransactionCount()
{
    m_writeTransactions++;
}
