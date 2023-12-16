#include "mt310s2adjustment.h"
#include "mt310s2adjflash.h"

Mt310s2Adjustment::Mt310s2Adjustment(cMT310S2dServer *server)
    :m_pMyServer(server)
{
}


void Mt310s2Adjustment::addAdjFlashObject(Mt310s2AdjFlash* obj)
{
    m_AdjFlashList.append(obj);
}


void Mt310s2Adjustment::removeAdjFlashObject(Mt310s2AdjFlash *obj)
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

