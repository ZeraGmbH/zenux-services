#ifndef PARSE_ZDSP_H
#define PARSE_ZDSP_H

#include <QString>

// TODO: Move to zera-scpi's Parse

class DspVarParser
{
public:
    DspVarParser();
    const QString GetKeyword(QChar**); // liesst nächstes schlüsselwort aus string

    QChar GetChar(QChar **); // liesst das nächste zeichen aus string
private:
    QString delimiter;
    QString whitespace;
};

#endif
