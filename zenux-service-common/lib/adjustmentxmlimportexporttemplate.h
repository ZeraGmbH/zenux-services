#ifndef ADJUSTMENTXMLIMPORTEXPORTTEMPLATE_H
#define ADJUSTMENTXMLIMPORTEXPORTTEMPLATE_H

#include <QString>
#include <QDomDocument>

class AdjustmentXmlImportExportTemplate
{
public:
    virtual ~AdjustmentXmlImportExportTemplate() = default;
    bool exportAdTojXMLFile(QString file);
    bool importAdjXMLFile(QString file);
    bool importAdjXMLString(QString& xml);

protected:
    virtual QString exportXMLString(int indent = 1) = 0;
    virtual bool importXMLDocument(QDomDocument* qdomdoc) = 0;
};

#endif // ADJUSTMENTXMLIMPORTEXPORTTEMPLATE_H
