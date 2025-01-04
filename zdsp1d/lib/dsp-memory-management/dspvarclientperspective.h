#ifndef DSPVARCLIENTPERSPECTIVE_H
#define DSPVARCLIENTPERSPECTIVE_H

#include <QString>

class DspVarClientPerspective { // zur verwaltung der dsp variablen auf client/server ebene
public:
    DspVarClientPerspective();
    bool Init(const QString& varDefinition); // legt eine variable an
    QString getClientHandleName() const;
    QString name() const;
    void SetOffs(long);
    int size() const;
    int type() const;
    ulong offs() const;
    int segment() const;

private:
    QString m_clientHandleName;
    QString m_sName; // eine dsp variable hat einen namen
    int m_nOffsAdr; // hat eine rel. start adresse
    int m_nSize; // und eine anzahl von elementen
    int m_nType; // der typ der variablen (efloat oder eint)
    int m_nSegment; //
};

#endif // DSPVARCLIENTPERSPECTIVE_H
