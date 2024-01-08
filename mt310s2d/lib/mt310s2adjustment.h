#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "mt310s2adjflash.h"
#include "adjustmentstatusinterface.h"
#include <QList>
#include <QString>


class Mt310s2Adjustment : public AdjustmentStatusInterface
{
public:
    Mt310s2Adjustment();
    void addAdjFlashObject(Mt310s2AdjFlash* obj);
    void removeAdjFlashObject(Mt310s2AdjFlash* obj);
    quint8 getAdjustmentStatus() override;

private:
    QList<Mt310s2AdjFlash*> m_AdjFlashList;
};

#endif // ADJUSTMENT_H
