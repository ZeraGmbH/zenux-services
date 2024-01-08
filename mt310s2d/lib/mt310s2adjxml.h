#ifndef ADJXML_H
#define ADJXML_H

#include <QString>
#include <QDomDocument>

class Mt310s2AdjXML
{
public:
    bool exportAdTojXMLFile(QString file);
    bool importAdjXMLFile(QString file);
    bool importAdjXMLString(QString& xml);

protected:
    virtual QString exportXMLString(int indent = 1) = 0; // adjustment data xml export to string
    virtual bool importXMLDocument(QDomDocument* qdomdoc) = 0;
};

#endif // ADJXML_H
