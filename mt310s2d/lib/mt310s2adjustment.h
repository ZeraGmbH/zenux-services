#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "adjustmenteeprom.h"
#include "adjustmentstatusinterface.h"
#include <QList>
#include <QString>


class Mt310s2Adjustment : public AdjustmentStatusInterface
{
public:
    Mt310s2Adjustment();
    void addAdjFlashObject(AdjustmentEeprom* obj);
    void removeAdjFlashObject(AdjustmentEeprom* obj);
    quint8 getAdjustmentStatus() override;

private:
    QList<AdjustmentEeprom*> m_AdjFlashList;
};

#endif // ADJUSTMENT_H
