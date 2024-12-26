#ifndef SCPIPARSER_ZDSP1D_H
#define SCPIPARSER_ZDSP1D_H

#include <QString>

// TODO: Move to zera-scpi's Parse

class ScpiParserZdsp1d
{
public:
    ScpiParserZdsp1d();
    const QString GetKeyword(QChar**); // liesst nächstes schlüsselwort aus string

    QChar GetChar(QChar **); // liesst das nächste zeichen aus string
private:
    QString delimiter;
    QString whitespace;
};

#endif
