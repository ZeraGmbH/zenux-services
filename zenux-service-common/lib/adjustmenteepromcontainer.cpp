#include "adjustmenteepromcontainer.h"
#include "adjustmenteeprom.h"

AdjustmentEepromContainer::AdjustmentEepromContainer()
{
}

void AdjustmentEepromContainer::addAdjFlashObject(AdjustmentEeprom* obj)
{
    m_AdjFlashList.append(obj);
}

void AdjustmentEepromContainer::removeAdjFlashObject(AdjustmentEeprom *obj)
{
    m_AdjFlashList.removeOne(obj);
}

quint8 AdjustmentEepromContainer::getAdjustmentStatus()
{
    quint8 stat = 0;
    for (int i = 0; i < m_AdjFlashList.count(); i++)
        stat |= m_AdjFlashList.at(i)->getAdjustmentStatus();
    return stat;
}

