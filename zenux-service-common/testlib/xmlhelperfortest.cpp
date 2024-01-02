#include "xmlhelperfortest.h"
#include <QDomDocument>
#include <QRegularExpression>
#include <QFile>

QString XmlHelperForTest::loadXml(QString xmlFile)
{
    QFile file(xmlFile);
    QDomDocument doc;
    file.open(QFile::ReadOnly);
    doc.setContent(&file);
    return doc.toString(-1).replace("\n", "");
}

QString XmlHelperForTest::prettify(QString xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    return doc.toString(4);
}

QString XmlHelperForTest::removeTimeDependentEntriesFromXml(QString xml)
{
    static QRegularExpression regexDate("<Date>.*<\\/Date>");
    xml = xml.replace(regexDate, "<Date>nodate</Date>");
    static QRegularExpression regexTime("<Time>.*<\\/Time>");
    xml = xml.replace(regexTime, "<Time>notime</Time>");
    return xml;
}
