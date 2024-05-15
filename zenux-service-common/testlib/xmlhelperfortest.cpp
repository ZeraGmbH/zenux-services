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
    const QString multiDocSeparator = "<!DOCTYPE";
    QStringList listXmlQuickAndDirty = xml.split(multiDocSeparator, Qt::SkipEmptyParts);

    QMap<QString, QString> sorted;
    for(QString xmlSingle : listXmlQuickAndDirty) {
        xmlSingle = multiDocSeparator + xmlSingle;
        QDomDocument doc;
        doc.setContent(xmlSingle);
        QDomNodeList elemList = doc.elementsByTagName("Type");
        Q_ASSERT(elemList.count() == 1);
        QString typeName = elemList.at(0).toElement().text();
        sorted[typeName] = xmlSingle;
    }

    QString prettyTotal;
    for(auto iter=sorted.cbegin(); iter!=sorted.cend(); iter++) {
        QDomDocument doc;
        doc.setContent(iter.value());
        prettyTotal += doc.toString(4);
    }
    return prettyTotal;
}

QString XmlHelperForTest::prepareForCompare(QString xml)
{
    return removeChecksumDependentEntriesFromXml(removeTimeDependentEntriesFromXml(prettify(xml)));
}

QString XmlHelperForTest::removeTimeDependentEntriesFromXml(QString xml)
{
    static QRegularExpression regexDate("<Date>.*<\\/Date>");
    xml = xml.replace(regexDate, "<Date>nodate</Date>");
    xml = xml.replace("<Date/>", "<Date>nodate</Date>");
    static QRegularExpression regexTime("<Time>.*<\\/Time>");
    xml = xml.replace(regexTime, "<Time>notime</Time>");
    xml = xml.replace("<Time/>", "<Time>notime</Time>");
    return xml;
}

QString XmlHelperForTest::removeChecksumDependentEntriesFromXml(QString xml)
{
    static QRegularExpression regexDate("<Chksum>.*<\\/Chksum>");
    QString emptyChksum = "<Chksum/>";
    if(xml.contains(regexDate))
        xml = xml.replace(regexDate, "<Chksum>nochksum</Chksum>");
    else if(xml.contains(emptyChksum))
        xml = xml.replace(emptyChksum, "<Chksum>nochksum</Chksum>");
    return xml;
}

QString XmlHelperForTest::removeDeviceSpecificEntriesFromXml(QString xml)
{
    static QRegularExpression regexVersionNumber("<VersionNumber>.*<\\/VersionNumber>");
    xml = xml.replace(regexVersionNumber, "<VersionNumber>DEVICE: Unkown;PCB: Unknown;LCA: Unknown;CTRL: Unknown</VersionNumber>");
    return xml;
}
