#ifndef ADJFLASH_H
#define ADJFLASH_H

#include "adjustmentstatusinterface.h"
#include <QDataStream>
#include <QString>
#include <QDateTime>

class Com5003AdjFlash : public AdjustmentStatusInterface
{
public:
    virtual void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) = 0; // the derived class exports adjdata to qdatastream
    virtual bool importAdjData(QString& s, QDataStream& stream) = 0; // same for import
};

#endif // ADJFLASH_H
