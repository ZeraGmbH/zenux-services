#include "mt310s2adjxml.h"
#include <QFile>
#include <QTextStream>

bool Mt310s2AdjXML::exportAdTojXMLFile(QString file)
{
    QString filename = file + ".xml";
    QFile adjfile(filename);
    if (!adjfile.open(QIODevice::WriteOnly )) {
        qCritical("exportAdTojXMLFile: could not open xml file: %s", qPrintable(filename));
        return false;
    }
    QString sXML = exportXMLString();
    QTextStream stream( &adjfile );
    stream << sXML;
    adjfile.close();
    return true;
}

bool Mt310s2AdjXML::importAdjXMLFile(QString file)
{
    QString filename = file + ".xml";
    QFile adjfile(filename);
    if(!adjfile.open(QIODevice::ReadOnly)) {
        qCritical("ImportAdjXMLFile: could not open xml file: %s", qPrintable(filename));
        return false;
    }
    QDomDocument justqdom("TheDocument");
    if(!justqdom.setContent(&adjfile)) {
        qCritical("importAdjXMLFile: format error in xml file: %s", qPrintable(filename));
        return false;
    }
    adjfile.close();
    return importXMLDocument(&justqdom);
}

bool Mt310s2AdjXML::importAdjXMLString(QString &xml)
{
    QDomDocument justqdom("TheDocument");
    QString errorMsg;
    if (!justqdom.setContent(xml, &errorMsg)) {
        qCritical("importAdjXMLString failed with: \"%s\"", qPrintable(errorMsg));
        qCritical("XML received: \"%s\"", qPrintable(xml));
        return false;
    }
    return importXMLDocument(&justqdom);
}
