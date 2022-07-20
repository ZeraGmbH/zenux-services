#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "adjxml.h"
#include "adjflash.h"
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

class cAdjustment: public cAdjXML
{
public:
    cAdjustment(cSystemInfo* sInfo, QString& devNode, quint8 adr); //
    virtual ~cAdjustment();
    bool exportJDataFlash();
    bool importJDataFlash();
    bool exportJDataXML(QString& file);
    bool importJDataXML(QString& file);
    virtual void exportAdjData(QDomDocument& doc, QDomElement& qde);
    virtual bool importAdjData(QDomNode& node);
    void addAdjFlashObject(cAdjFlash* obj);
    void addAdjXMLObject(cAdjXML* obj);
    quint8 getAdjustmentStatus();
    quint16 getChecksum();

private:
    QList<cAdjFlash*> m_AdjFlashList;
    QList<cAdjXML*> m_AdjXMLList;
    cSystemInfo* m_pSystemInfo;
    QString m_sDeviceNode;
    quint8 m_nI2CAdr;
    quint8 m_nAdjStatus;
    quint16 m_nChecksum;

};

#endif // ADJUSTMENT_H
