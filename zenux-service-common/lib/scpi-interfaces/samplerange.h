#ifndef SAMPLERANGE_H
#define SAMPLERANGE_H

#include "scpiconnection.h"

class cSampleRange: public ScpiConnection
{
    Q_OBJECT
public:
    cSampleRange(cSCPI* scpiinterface, QString name, quint16 srate, quint8 selcode);
    virtual void initSCPIConnection(QString leadingNodes) override;
    QString &getName();
    quint8 getSelCode();
    quint16 getSRate();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readSRate(QString &sInput);
    QString m_sName;
    quint16 m_nSRate;
    quint8 m_nSelCode; // selection code
};

#endif // SAMPLERANGE_H
