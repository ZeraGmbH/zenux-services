#ifndef ADJDATAITEMSCPI_H
#define ADJDATAITEMSCPI_H

#include "adjdataitem.h"
#include "scpiconnection.h"
#include <QDataStream>
#include <QString>

class AdjDataItemScpi: public ScpiConnection // base class for adjustment coefficients and nodes
{
public:
    struct TJustDataParam
    {
        std::shared_ptr<cSCPI> scpiinterface;
        std::function<bool(bool &)> checkPermission;
        int digits;
    };
    AdjDataItemScpi(TJustDataParam param, AdjDataItem* adjItem);
    ~AdjDataItemScpi();
    virtual void initSCPIConnection(QString leadingNodes) override;

    AdjDataItem *getAdjItem();

    // xml
    quint8 getStatus() const;
    QString statusToString();
    void statusFromString(const QString& s);
    QString coefficientsToString() const;
    void coefficientsFromString(const QString& s);
    QString nodesToString() const;
    void nodesFromString(const QString& s );

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    std::function<bool(bool &)> m_checkPermission;
private:
    QString scpiReadWriteStatus(const QString& scpi);
    QString scpiReadWriteJustCoeeficient(const QString& scpi, quint8 index);
    QString scpiReadWriteJustNode(const QString& scpi, quint8 index);

    AdjDataItem* m_adjItem;
    int m_digits;
};

#endif
