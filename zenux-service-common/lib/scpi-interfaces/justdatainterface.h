#ifndef JUSTDATAINTERFACE_H
#define JUSTDATAINTERFACE_H

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

    void Serialize(QDataStream& qds); // can be used to write adjustment data to flash memory
    void Deserialize(QDataStream& qds); // coefficients and nodes will be serialitzed both
    QString SerializeStatus();
    QString SerializeCoefficients(); // for xml file we serialize to qstring
    QString SerializeNodes(); // but coefficients and nodes seperately
    void DeserializeStatus(const QString& s);
    void DeserializeCoefficients(const QString& s);
    void DeserializeNodes(const QString& s );

    double getCorrection(double arg); // calculates correction value c= ax^order +bx^order-1 ...
    bool cmpCoefficients(); // calculates coefficients from nodes
    quint8 getStatus();
    void initJustData(double init); // for initialization of justdata
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    std::function<bool(bool &)> m_checkPermission;
private:
    quint8 m_nStatus;
    double* m_pCoefficient; // size of data depends on order
    AdjustmentNode* m_pJustNode; // same
    int m_nOrder; // we notice order
    int m_digits;

    QString m_ReadWriteStatus(QString& sInput);
    QString m_ReadWriteJustCoeeficient(QString& sInput, quint8 index);
    QString m_ReadWriteJustNode(QString& sInput, quint8 index);
    
    bool setNode(int index, AdjustmentNode jn); // !!! setting node sequence is relevant !!!
    AdjustmentNode* getNode(int index); // can be read back
    bool setCoefficient(int index, double); // !!! setting coefficient also is sequence relevant !!!
    double getCoefficient(int index);
};

	
#endif
