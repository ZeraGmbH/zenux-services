#ifndef ADJXML_H
#define ADJXML_H

#include "adjustmentstatusinterface.h"

class QDomElement;
class QDomDocument;
class QDomNode;

class Com5003AdjXML : public AdjustmentStatusInterface
{
public:
    virtual void exportAdjDataXml(QDomDocument& doc, QDomElement& qde) = 0; // the derived class exports adjdata beneath qdomelement
    virtual bool importAdjDataXml(QDomNode& node) = 0; // same for import
};

#endif // ADJXML_H
