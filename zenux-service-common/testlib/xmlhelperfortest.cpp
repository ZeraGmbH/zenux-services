#include "xmlhelperfortest.h"
#include <QDomDocument>
#include <QRegularExpression>

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
