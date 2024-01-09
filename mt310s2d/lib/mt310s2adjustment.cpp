#include "mt310s2adjustment.h"
#include "adjustmenteeprom.h"

Mt310s2Adjustment::Mt310s2Adjustment()
{
}


void Mt310s2Adjustment::addAdjFlashObject(AdjustmentEeprom* obj)
{
    m_AdjFlashList.append(obj);
}


void Mt310s2Adjustment::removeAdjFlashObject(AdjustmentEeprom *obj)
{
    m_AdjFlashList.removeOne(obj);
}


quint8 Mt310s2Adjustment::getAdjustmentStatus()
{

    quint8 stat = 0;

    for (int i = 0; i < m_AdjFlashList.count(); i++)
        stat |= m_AdjFlashList.at(i)->getAdjustmentStatus();

    return stat;
}

