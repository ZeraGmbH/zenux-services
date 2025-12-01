#ifndef MOCKI2CCTRLEMOB_H
#define MOCKI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    MockI2cCtrlEMOB(qint8 muxChannel, const QString &instrumentTypePostfix);
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
    ZeraMControllerIo::atmelRM readEmobInstrumentSubType(QString& answer) override;
    ZeraMControllerIo::atmelRM readEmobErrorStatus(quint8& err) override;
    ZeraMControllerIo::atmelRM clearErrorStatus() override;
    static QByteArray getDefaultExchangeData();
    ZeraMControllerIo::atmelRM readExchangeData(QByteArray& answer) override;
    ZeraMControllerIo::atmelRM writeExchangeData(QByteArray &data, int senderId) override;
private:
    qint8 m_muxChannel;
    QString m_instrumentTypePostfix;
};

#endif // MOCKI2CCTRLEMOB_H
