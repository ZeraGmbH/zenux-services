#ifndef XMLHELPERFORTEST_H
#define XMLHELPERFORTEST_H

#include <QString>

class XmlHelperForTest
{
public:
    static QString loadXml(QString xmlFile);
    static QString prettify(QString xml);
    static QString prepareForCompare(QString xml);
    static QString removeTimeDependentEntriesFromXml(QString xml);
    static QString removeChecksumDependentEntriesFromXml(QString xml);
    static QString removeDeviceSpecificEntriesFromXml(QString xml);
};

#endif // XMLHELPERFORTEST_H
