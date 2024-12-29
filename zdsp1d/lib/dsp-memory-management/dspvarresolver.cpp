#include "dspvarresolver.h"

DspVarResolver::DspVarResolver()
{
    m_varHash.clear();
    m_varParser.SetDelimiter("(,+,-,)"); // setze die trennzeichen für den parser
    m_varParser.SetWhiteSpace(" (,)");
}

void DspVarResolver::setVarHash()
{
    m_varHash.clear();
    for (int i = 0; i < MemSectionList.count(); i++) {
        initMemsection(MemSectionList.at(i));
        setQHash(MemSectionList.at(i));
    }
}

void DspVarResolver::addSection(TMemSection* section)
{
    MemSectionList.append(section);
}

void DspVarResolver::initMemsection(TMemSection *psec)
{
    if (psec->Section == systemSection) { // wir initialisieren nur system sections
        long offs = 0;
        for (int i = 0; i< (psec->n); i++) {
            psec->DspVar[i].offs = offs;
            psec->DspVar[i].adr = psec->StartAdr + offs;
            offs += psec->DspVar[i].size;
        }
    }
}

void DspVarResolver::setQHash(TMemSection* psec) // zum setzen der qhash
{
    for (int i = 0; i< (psec->n); i++)
        m_varHash[psec->DspVar[i].Name] = &(psec->DspVar[i]);
}

long DspVarResolver::varOffset(const QString& varNameOrValue, ulong umo, ulong globalstartadr)
{
    QString ts = varNameOrValue.toUpper();
    const QChar* cts = ts.data();
    QString sSearch = m_varParser.GetKeyword(&cts); // der namen der variable, die gesucht ist
    if (m_varHash.contains(sSearch)) {
        ulong retoffs;
        TDspVar* pDspVar = m_varHash.value(sSearch);
        retoffs = pDspVar->offs;

        ts = ts.remove(' ');
        ts = ts.remove(sSearch); // name raus
        if (ts.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            bool ok;
            long offset = ts.toLong(&ok,10); // prüfen auf dez. konstante
            if (ok)
                retoffs += offset;
            else {
                offset = ts.toLong(&ok,16); // mal hex versuchen
                if (ok)
                    retoffs += offset;
                else
                    return -1; // sonst ist das ein fehler
            }
        }

        if (pDspVar->segment == globalSegment) // wenn daten im globalen segment liegen
            retoffs += (globalstartadr - umo);

        return retoffs;
    }

    bool ok;
    long offset = varNameOrValue.toLong(&ok, 10); // prüfen auf dez. konstante
    if (ok)
        return offset;

    offset = varNameOrValue.toLong(&ok, 16); // mal hex versuchen
    if (ok)
        return offset;

    return -1;
}

long DspVarResolver::varAddress(QString& varNameOrValue)
{
    QString ts = varNameOrValue.toUpper();
    const QChar* cts = ts.data();
    QString sSearch = m_varParser.GetKeyword(&cts); // der namen der variable, die gesucht ist
    if (m_varHash.contains(sSearch)) {
        TDspVar* pDspVar = m_varHash.value(sSearch);
        ts = ts.remove(' ');
        ts = ts.remove(sSearch); // name raus
        if (ts.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            bool ok;
            long offset = ts.toLong(&ok, 10); // prüfen auf dez. konstante
            if (ok)
                return pDspVar->adr + offset;

            offset = ts.toLong(&ok, 16); // mal hex versuchen
            if (ok)
                return pDspVar->adr +offset;

            return -1; // sonst ist das ein fehler
        }
        return pDspVar->adr;
    }

    bool ok;
    long adress = varNameOrValue.toLong(&ok, 10); // prüfen auf dez. konstante
    if (ok)
        return adress;

    adress = varNameOrValue.toLong(&ok, 16); // mal hex versuchen
    if (ok)
        return adress;

    return -1;
}

TDspVar* DspVarResolver::getDspVar(const QString &varName)
{
    QString ts = varName.toUpper();
    const QChar* cts = ts.data();
    QString sSearch = m_varParser.GetKeyword(&cts);

    if (m_varHash.contains(sSearch))
        return m_varHash.value(sSearch);
    else
        return 0;
}

int DspVarResolver::type(const QString &varName)
{
    QString ts = varName.toUpper();
    const QChar* cts = ts.data();
    QString sSearch = m_varParser.GetKeyword(&cts);
    if (m_varHash.contains(sSearch))
        return m_varHash.value(sSearch)->type;
    else
        return eUnknown;
}
