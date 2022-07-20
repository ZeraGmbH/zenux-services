#include <QList>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>
#include <QDomDocument>
#include <QDomText>
#include <QDomNodeList>
#include <QFile>
#include <QDateTime>
#include <syslog.h>
#include <F24LC256.h>

#include "mt310s2dglobal.h"
#include "adjflash.h"
#include "justdata.h"
#include "adjxml.h"
#include "systeminfo.h"
#include "adjustment.h"
#include "micro-controller-io/atmel.h"

cAdjustment::cAdjustment(cMT310S2dServer *server)
    :m_pMyServer(server)
{
}


void cAdjustment::addAdjFlashObject(cAdjFlash* obj)
{
    m_AdjFlashList.append(obj);
}


void cAdjustment::removeAdjFlashObject(cAdjFlash *obj)
{
    m_AdjFlashList.removeOne(obj);
}


quint8 cAdjustment::getAdjustmentStatus()
{

    quint8 stat = 0;

    for (int i = 0; i < m_AdjFlashList.count(); i++)
        stat |= m_AdjFlashList.at(i)->getAdjustmentStatus();

    return stat;
}


