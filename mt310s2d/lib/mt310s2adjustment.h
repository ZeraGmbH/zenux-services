#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "mt310s2d.h"
#include "mt310s2adjflash.h"
#include "adjustmentstatusinterface.h"
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

class Mt310s2Adjustment : public AdjustmentStatusInterface
{
public:
    Mt310s2Adjustment(cMT310S2dServer* server);
    void addAdjFlashObject(Mt310s2AdjFlash* obj);
    void removeAdjFlashObject(Mt310s2AdjFlash* obj);
    quint8 getAdjustmentStatus() override;

private:
    cMT310S2dServer* m_pMyServer;
    QList<Mt310s2AdjFlash*> m_AdjFlashList;
};

#endif // ADJUSTMENT_H
