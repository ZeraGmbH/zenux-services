#ifndef ADJDATAITEMINTERFACE_H
#define ADJDATAITEMINTERFACE_H

#include "adjdataitem.h"
#include "scpiconnection.h"
#include <QDataStream>
#include <QString>

class AdjDataItemInterface: public ScpiConnection // base class for adjustment coefficients and nodes
{
public:
    struct TJustDataParam
    {
        cSCPI *scpiinterface;
        double init;
        std::function<bool(bool &)> checkPermission;
        int digits;
    };
    AdjDataItemInterface(TJustDataParam param, AdjDataItem* adjItem);
    ~AdjDataItemInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;

    AdjDataItem *getAdjItem();

    // xml
    quint8 getStatus();
    QString statusToString();
    void statusFromString(const QString& s);
    QString coefficientsToString();
    void coefficientsFromString(const QString& s);
    QString nodesToString();
    void nodesFromString(const QString& s );

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    std::function<bool(bool &)> m_checkPermission;
private:
    QString scpiReadWriteStatus(QString& sInput);
    QString scpiReadWriteJustCoeeficient(QString& sInput, quint8 index);
    QString scpiReadWriteJustNode(QString& sInput, quint8 index);

    AdjDataItem* m_adjItem;
    int m_digits;
};

#endif
