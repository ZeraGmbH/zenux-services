// header datei für keyword parser

#ifndef KPARSER_H
#define KPARSER_H

#include <QString>

class cParse { // sucht nach zusammenhängenden wörtern mit frei wählbaren delimitern und whitespaces
public:
    cParse() {delimiter = " :?;"; whitespace = " ";} // setzt default delimiter, whitespace
    const QString GetKeyword(QChar**); // liesst nächstes schlüsselwort aus string
    QChar GetChar(QChar **); // liesst das nächste zeichen aus string
    const QString SetDelimiter(const QString s);
    const QString SetWhiteSpace(const QString s);
private:
    QString delimiter;
    QString whitespace;
};

#endif
