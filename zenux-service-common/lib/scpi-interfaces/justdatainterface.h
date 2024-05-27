#ifndef JUSTDATAINTERFACE_H
#define JUSTDATAINTERFACE_H

#include "adjdataitem.h"
#include "scpiconnection.h"
#include "adjustmentnode.h"
#include <QDataStream>
#include <QString>

// a JustDataInterface object has a max. possible order
// the order must not necessarily be used
// setting only the first node results in a effectively 0 order
// a new generated object is also initialized like that
class JustDataInterface: public ScpiConnection // base class for adjustment coefficients and nodes
{
public:
    enum StatusMasks
    {
        Justified = 128
    };
    struct TJustDataParam
    {
        cSCPI *scpiinterface;
        int order;
        double init;
        std::function<bool(bool &)> checkPermission;
        int digits;
    };
    JustDataInterface(TJustDataParam param);
    ~JustDataInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;

    AdjDataItem &getAdjItem();

    // xml
    quint8 getStatus();
    QString statusToString();
    void statusFromString(const QString& s);
    QString coefficientsToString();
    void coefficientsFromString(const QString& s);
    QString nodesToString();
    void nodesFromString(const QString& s );

    double getCorrection(double arg); // calculates correction value c= ax^order +bx^order-1 ...
    bool calcCoefficientsFromNodes();
    void initJustData(double init); // for initialization of justdata

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    std::function<bool(bool &)> m_checkPermission;
private:
    QString scpiReadWriteStatus(QString& sInput);
    QString scpiReadWriteJustCoeeficient(QString& sInput, quint8 index);
    QString scpiReadWriteJustNode(QString& sInput, quint8 index);

    bool setNode(int index, AdjustmentNode jn); // !!! setting node sequence is relevant !!!
    AdjustmentNode* getNode(int index); // can be read back
    bool setCoefficient(int index, double); // !!! setting coefficient also is sequence relevant !!!
    double getCoefficient(int index);

    AdjDataItem m_adjItem;
    int m_digits;
};

	
#endif
