#ifndef ADJXML_H
#define ADJXML_H

#include "adjustmentstatusinterface.h"

class QDomElement;
class QDomDocument;
class QDomNode;

class cAdjXML : public AdjustmentStatusInterface
{
public:
    virtual void exportAdjData(QDomDocument& doc, QDomElement& qde) = 0; // the derived class exports adjdata beneath qdomelement
    virtual bool importAdjData(QDomNode& node) = 0; // same for import
};

#endif // ADJXML_H
