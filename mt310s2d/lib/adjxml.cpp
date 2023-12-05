#include <QFile>
#include <QDomDocument>
#include <QTextStream>

#include "adjxml.h"
#include "mt310s2dglobal.h"


cAdjXML::cAdjXML()
{
}


bool cAdjXML::exportAdjXML(QString file)
{
    QString filename = file + ".xml";

    QFile adjfile(filename);
    if ( !adjfile.open( QIODevice::WriteOnly ) )
    {
        qCritical("%s", "exportAdjXML: could not open xml file");
        return false;
    }

    QString sXML;

    sXML = exportXMLString();
    QTextStream stream( &adjfile );
    stream << sXML;
    adjfile.close();

    return true;
}


bool cAdjXML::importAdjXML(QString file)
{
    QString filename = file + ".xml";

    QFile adjfile(filename);
    if ( !adjfile.open( QIODevice::ReadOnly ) )
    {
        qCritical("%s", "importAdjXML: could not open xml file");
        return false;
    }

    QDomDocument justqdom( "TheDocument" );
    if ( !justqdom.setContent( &adjfile ) )
    {
        adjfile.close();
        qCritical("%s", "importAdjXML: format error in xml file");
        return false;
    }

    adjfile.close();

    return importXMLDocument(&justqdom);
}


bool cAdjXML::importAdjXMLString(QString &xml)
{
    QDomDocument justqdom( "TheDocument" );
    if ( !justqdom.setContent(xml) )
    {
        qCritical("%s", "importAdjXMLString: format error in xml file");
        return false;
    }

    return importXMLDocument(&justqdom);
}


