#ifndef ADJDATAHEADERINTERN_H
#define ADJDATAHEADERINTERN_H

#include <QString>
#include <QDateTime>

struct AdjDataHeaderIntern
{
    QString m_deviceName;
    QString m_serverVersion;
    QString m_deviceVersion;
    QString m_serialNumber;
    QDateTime m_adjustmentDate;
};

#endif // ADJDATAHEADERINTERN_H
