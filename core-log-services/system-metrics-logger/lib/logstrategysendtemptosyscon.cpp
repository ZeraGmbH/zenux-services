#include "logstrategysendtemptosyscon.h"

LogStrategySendTempToSysCon::LogStrategySendTempToSysCon(QString valueLabel, QString unitLabel) :
    m_valueLabel(valueLabel),
    m_unitLabel(unitLabel),
    m_sendTemp(I2cSendTemp("/dev/i2c-0", 0x22, 0))  // todo no magic string!!!
{
}

void LogStrategySendTempToSysCon::addValue(QList<float> &values, float newValue)
{
    qInfo("Send Temp: %.1f to SystemController %.1f", newValue);
    // send via I2C
    m_sendTemp.sendTemp(newValue);
}
