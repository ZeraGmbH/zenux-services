#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "com5003adjxml.h"
#include "com5003adjflash.h"
#include "systeminfo.h"
#include "atmelpermissiontemplate.h"
#include <QString>
#include <QList>

class Com5003Adjustment: public Com5003AdjXML
{
public:
    Com5003Adjustment(cSystemInfo* sInfo, QString& devNode, quint8 adr, AtmelPermissionTemplate *permissionQueryHandler); //
    virtual ~Com5003Adjustment();
    bool exportAdjFlash(QDateTime dateTimeWrite);
    bool importAdjFlash();

    QString exportXMLString(int indent = 1);
    bool exportAdTojXMLFile(QString& file);
    bool importAdjXMLFile(QString& file);
    bool importAdjXMLString(QString& xml);

    virtual void exportAdjDataXml(QDomDocument& doc, QDomElement& qde) override;
    virtual bool importAdjDataXml(QDomNode& node) override;

    void addAdjFlashObject(Com5003AdjFlash* obj);
    void addAdjXMLObject(Com5003AdjXML* obj);
    quint8 getAdjustmentStatus() override;
    quint16 getChecksum();

private:
    bool importXMLDocument(QDomDocument* qdomdoc);

    QList<Com5003AdjFlash*> m_AdjFlashList;
    QList<Com5003AdjXML*> m_AdjXMLList;
    cSystemInfo* m_pSystemInfo;
    QString m_sDeviceNode;
    quint8 m_nI2CAdr;
    AtmelPermissionTemplate *m_permissionQueryHandler;
    quint8 m_nAdjStatus;
    quint16 m_nChecksum = 0;

};

#endif // ADJUSTMENT_H
