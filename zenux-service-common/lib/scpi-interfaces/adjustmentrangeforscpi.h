#ifndef ADJUSTMENTRANGEFORSCPI_H
#define ADJUSTMENTRANGEFORSCPI_H

#include "scpiconnection.h"

class AdjustmentRangeForSCPI : public ScpiConnection
{
public:
    AdjustmentRangeForSCPI(cSCPI *scpi);
    virtual void initSCPIConnection(QString leadingNodes) override;

protected:
    virtual void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QString scpiReadWriteStatus(QString& sInput);
    QString scpiReadWriteJustCoeeficient(QString& sInput, quint8 index);
    QString scpiReadWriteJustNode(QString& sInput, quint8 index);

};

#endif // ADJUSTMENTRANGEFORSCPI_H
