#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "com5003adjxml.h"
#include "com5003adjflash.h"
#include "systeminfo.h"
#include <QString>
#include <QList>

namespace Adjustment
{
enum jDataStatus
{
    adjusted,
    notAdjusted,
    wrongVERS = 2,
    wrongSNR = 4
};
}

class Com5003Adjustment: public Com5003AdjXML
{
public:
    Com5003Adjustment(cSystemInfo* sInfo, QString& devNode, quint8 adr); //
    virtual ~Com5003Adjustment();
    bool exportJDataFlash();
    bool importJDataFlash();

    QString exportXMLString();
    bool exportAdTojXMLFile(QString& file);
    bool importAdjXMLFile(QString& file);
    virtual void exportAdjData(QDomDocument& doc, QDomElement& qde) override;
    virtual bool importAdjData(QDomNode& node) override;

    void addAdjFlashObject(Com5003AdjFlash* obj);
    void addAdjXMLObject(Com5003AdjXML* obj);
    quint8 getAdjustmentStatus() override;
    quint16 getChecksum();

private:
    QList<Com5003AdjFlash*> m_AdjFlashList;
    QList<Com5003AdjXML*> m_AdjXMLList;
    cSystemInfo* m_pSystemInfo;
    QString m_sDeviceNode;
    quint8 m_nI2CAdr;
    quint8 m_nAdjStatus;
    quint16 m_nChecksum = 0;

};

#endif // ADJUSTMENT_H
