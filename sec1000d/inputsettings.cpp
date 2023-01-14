#include <xmlconfigreader.h>

#include "inputsettings.h"


cInputSettings::cInputSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;

    m_ConfigXMLMap[QString("serviceconfig:connectivity:inputs:n")] = InputSettings::setnumber;
    // for the inputs we generate new entries dynamically
}


bool cInputSettings::hasInput(QString name)
{
   return muxInfoHash.contains(name);
}


qint8 cInputSettings::mux(QString name)
{
    return muxInfoHash[name];
}


void cInputSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case InputSettings::setnumber:
        {
            m_nCount = m_pXMLReader->getValue(key).toInt();
            for (int i = 0; i < m_nCount; i++) {
                m_ConfigXMLMap[QString("serviceconfig:connectivity:inputs:inp%1:name").arg(i+1)] = InputSettings::setinputname1+i;
                m_ConfigXMLMap[QString("serviceconfig:connectivity:inputs:inp%1:muxer").arg(i+1)] = InputSettings::setinputmuxer1+i;

            }
        }
        default:
        {
            if (cmd >= InputSettings::setinputname1 && cmd < InputSettings::setinputname1 + 32)
                actName = m_pXMLReader->getValue(key);
            else if (cmd >= InputSettings::setinputmuxer1 && cmd < InputSettings::setinputmuxer1 + 32) {
                    quint8 mux = m_pXMLReader->getValue(key).toInt();
                    muxInfoHash[actName] = mux;
            }
        }
        }
    }
}
