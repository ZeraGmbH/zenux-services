#include "fpgasettings.h"

enum configstate
{
    setCtrlDevNode,
    setMsgDevNode,
    setSecDevNode,
    setDspDevNode,
};

FPGASettings::FPGASettings(Zera::XMLConfig::cReader *xmlReader)
{
    m_pXMLReader = xmlReader;
    m_ConfigXMLMap["serviceconfig:connectivity:fpga:ctrl_device_node"] = setCtrlDevNode;
    m_ConfigXMLMap["serviceconfig:connectivity:fpga:msg_device_node"] = setMsgDevNode;
    m_ConfigXMLMap["serviceconfig:connectivity:fpga:sec_device_node"] = setSecDevNode;
    m_ConfigXMLMap["serviceconfig:connectivity:fpga:dsp_device_node"] = setDspDevNode;
}

QString FPGASettings::getCtrlDeviceNode()
{
    return m_ctrlDeviceNode;
}

QString FPGASettings::getMsgDeviceNode()
{
    return m_msgDeviceNode;
}

QString FPGASettings::getSecDeviceNode()
{
    return m_secDeviceNode;
}

QString FPGASettings::getDspDeviceNode()
{
    return m_dspDeviceNode;
}

void FPGASettings::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case setCtrlDevNode:
            m_ctrlDeviceNode = m_pXMLReader->getValue(key);
            break;
        case setMsgDevNode:
            m_msgDeviceNode = m_pXMLReader->getValue(key);
            break;
        case setSecDevNode:
            m_secDeviceNode = m_pXMLReader->getValue(key);
            break;
        case setDspDevNode:
            m_dspDeviceNode = m_pXMLReader->getValue(key);
            break;
        }
    }
}
