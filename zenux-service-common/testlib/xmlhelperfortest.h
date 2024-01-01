#ifndef XMLHELPERFORTEST_H
#define XMLHELPERFORTEST_H

#include <QString>

class XmlHelperForTest
{
public:
    static QString prettify(QString xml);
    static QString removeTimeDependentEntriesFromXml(QString xml);
};

#endif // XMLHELPERFORTEST_H
