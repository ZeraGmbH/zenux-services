#include "scpiparserzdsp1d.h"

ScpiParserZdsp1d::ScpiParserZdsp1d() {
    delimiter = " :?;";
    whitespace = " ";
}

const QString ScpiParserZdsp1d::GetKeyword(QChar **s)
{
    QString ls = "";
    QChar tc=GetChar(s);
    if (!tc.isNull())
    { // whitespace weg und schon mal 1 zeichen
        for (;;) {
            ls+=tc;
            tc=**s; // hole nächstes zeichen
            if ( delimiter.contains(tc,Qt::CaseInsensitive) ) break; // wenn zeichen delimiter -> fertig
            if (tc.isNull()) break; // string zu ende -> fertig
            (*s)++; // sonst nächstes zeichen
        }
    };

    return(ls); // schlüsselwort ohne delimiter
}

QChar ScpiParserZdsp1d::GetChar(QChar** s)
{
    QChar tc=0;
    if (!(**s).isNull())
    { // abfrage für den fall, dass string schon zu ende
        do {
            tc = **s;
            (*s)++;
        }
        while ( (!tc.isNull()) && (whitespace.contains(tc,Qt::CaseInsensitive)) ); // ignoriere whitespace character
    }

    return(tc); // return = 0 oder zeichen != whitespace
}
