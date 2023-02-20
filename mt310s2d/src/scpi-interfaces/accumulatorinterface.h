#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include <scpiconnection.h>

enum accumulatorCommands{
    cmdStatus
};

class AccumulatorInterface : public ScpiConnection
{
public:
    AccumulatorInterface(cSCPI* scpiInterface);
    void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
};

#endif // ACCUMULATORINTERFACE_H
