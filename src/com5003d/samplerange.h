#ifndef SAMPLERANGE_H
#define SAMPLERANGE_H

#include <QObject>

#include <scpiconnection.h>

enum SampleRangeCommands
{
    SampleRangeSamples
};

class cSampleRange: public cSCPIConnection
{
    Q_OBJECT

public:
    cSampleRange(cSCPI* scpiinterface, QString name, quint16 srate, quint8 selcode);
    virtual void initSCPIConnection(QString leadingNodes);
    QString &getName();
    quint8 getSelCode();
    quint16 getSRate();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    QString m_sName;
    quint16 m_nSRate;
    quint8 m_nSelCode; // selection code

    QString m_ReadSRate(QString &sInput);
};

#endif // SAMPLERANGE_H
