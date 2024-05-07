#ifndef ADJUSTMENDATAHEADER_H
#define ADJUSTMENDATAHEADER_H

#include <QString>
#include <QDateTime>

struct AdjustmentDataHeader
{
    QString m_deviceName;
    QString m_serverVersion;
    QString m_deviceVersion;
    QString m_serialNumber;
    QDateTime m_adjustmentDate;
};

#endif // ADJUSTMENDATAHEADER_H
