#include "mt310s2adjxml.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

Mt310s2AdjXML::Mt310s2AdjXML()
{
}

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
    if (!justqdom.setContent(xml)) {
        qCritical("importAdjXMLString: format error in xml");
        return false;
    }
    return importXMLDocument(&justqdom);
}
