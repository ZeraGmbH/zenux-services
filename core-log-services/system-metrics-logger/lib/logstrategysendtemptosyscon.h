#ifndef LOGSTRATEGYSENDTEMPTOSYSCON_H
#define LOGSTRATEGYSENDTEMPTOSYSCON_H

#include "abstractlogstrategy.h"
#include "i2csendtemp.h"


class LogStrategySendTempToSysCon : public AbstractLogStrategy
{
public:
    LogStrategySendTempToSysCon(QString valueLabel, QString unitLabel);
    void addValue(QList<float> &values, float newValue) override;
private:
    QString m_valueLabel;
    QString m_unitLabel;
    I2cSendTemp m_sendTemp;
};


#endif // LOGSTRATEGYSENDTEMPTOSYSCON_H
