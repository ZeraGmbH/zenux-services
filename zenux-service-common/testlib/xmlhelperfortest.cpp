#include "xmlhelperfortest.h"
#include <QRegularExpression>

QString XmlHelperForTest::removeTimeDependentEntriesFromXml(QString xml)
{
    static QRegularExpression regexDate("<Date>.*<\\/Date>");
    xml = xml.replace(regexDate, "<Date>nodate</Date>");
    static QRegularExpression regexTime("<Time>.*<\\/Time>");
    xml = xml.replace(regexTime, "<Time>notime</Time>");
    static QRegularExpression regexChecksum("<Chksum>.*<\\/Chksum>");
    xml = xml.replace(regexChecksum, "<Chksum>nocheck</Chksum>");
    return xml;
}
