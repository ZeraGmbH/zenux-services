#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "mt310s2d.h"
#include "adjflash.h"
#include <QList>
#include <QString>

namespace Adjustment
{
enum jDataStatus
{
    adjusted,
    notAdjusted,
    wrongVERS = 2,
    wrongSNR = 4
};
}

class cAdjustment
{
public:
    cAdjustment(cMT310S2dServer* server);
    void addAdjFlashObject(cAdjFlash* obj);
    void removeAdjFlashObject(cAdjFlash* obj);
    quint8 getAdjustmentStatus();

private:
    cMT310S2dServer* m_pMyServer;
    QList<cAdjFlash*> m_AdjFlashList;
};

#endif // ADJUSTMENT_H
