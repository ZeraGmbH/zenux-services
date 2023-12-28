#ifndef ADJXML_H
#define ADJXML_H

#include <QString>

class QDomDocument;

class Mt310s2AdjXML
{
public:
    Mt310s2AdjXML();

    virtual bool exportAdTojXMLFile(QString file);
    virtual bool importAdjXMLFile(QString file);
    virtual bool importAdjXMLString(QString& xml);

protected:
    virtual QString exportXMLString(int indent = 1) = 0; // adjustment data xml export to string
    virtual bool importXMLDocument(QDomDocument* qdomdoc) = 0;
};

#endif // ADJXML_H
