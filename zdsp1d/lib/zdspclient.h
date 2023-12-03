#ifndef ZDSPCLIENT_H
#define ZDSPCLIENT_H

#include "dsp.h"
#include "dspvarresolver.h"
#include "dspvarclientperspective.h"
#include <xiqnetpeer.h>

class ZDspServer;

class cZDSP1Client
{
public:
    cZDSP1Client(){}
    cZDSP1Client(int socket, XiQNetPeer *netclient);
    ~cZDSP1Client(){} //  allokierten speicher ggf. freigeben

    QString& setRawActualValueList(QString&);
    QString& getRawActualValueList();
    QString& SetCmdListDef(QString& );
    QString& GetCmdListDef();
    QString& SetCmdIntListDef(QString&);
    QString& GetCmdIntListDef();
    void SetEncryption(int);
    int GetEncryption();
    bool GenCmdLists(QString&, ulong, ulong); // baut die cmdlisten  für den dsp zusammen wenn fehler -> false
    cDspCmd GenDspCmd(QString&, bool*, ulong, ulong); // generiert ein dsp kommando aus einem string
    QString &readActValues(QString&); // liess die messergebnisse (liste)
    bool isActive();
    void SetActive(bool); // merkt sich in m_bActive ob diese liste aktiv ist
    ulong setStartAdr(ulong, ulong); // zum relokalisieren der userdaten
    QString &DspVarListRead(QString&); // lesen dsp daten ganze Liste
    bool readDspVarInt(QString varName, int& intval); // einen int (32bit) wert lesen
    bool readDspVarFloat(QString varName, float& floatVal); // eine float wert lesen
    TDspVar* DspVarRead(QString nameLen, QByteArray* varRead); // lesen dsp variable;  name , länge stehen im parameter string; werte im anschluss im qbytearray
    bool DspVarWrite(QString);  // schreiben  true wenn ok
    QString DspVarWriteRM(QString&); // dito schreiben mit rückmeldung
    QList<cDspCmd>& GetDspCmdList(); // damit der server die komplette liste aller clients
    QList<cDspCmd>& GetDspIntCmdList(); // an den dsp übertragen kann
    int getSocket();

    DspVarResolver m_dspVarResolver; // zum auflösen der variablen aus kommandos
    XiQNetPeer* m_pNetClient; // our network client

private:
    void init(int socket, XiQNetPeer *netclient);
    bool GenCmdList(QString&, QList<cDspCmd>& ,QString&,ulong,ulong);
    bool syntaxCheck(QString&);
    char* qSEncryption(char*,int );

    int m_socket; // socket für den die verbindung besteht
    bool m_bActive;
    int Encryption;
    QString sOutput;
    QString m_sCmdListDef; // kommando liste defintion
    QString m_sIntCmdListDef; // interrupt kommando  liste defintion

    QList<DspVarClientPerspective> m_dspRawActualValueVarList; // liste mit variablen beschreibungen
    QList<cDspCmd> m_DspCmdList; // liste mit dsp kommandos (periodisch)
    QList<cDspCmd>  m_DspIntCmdList; // liste mit dsp kommandos (interrupt)
    QVector<TDspVar> m_dspVarArray; // array von TDspVar
    TMemSection m_memorySection; // eine memory section für den DspVarResolver für die variablen des clients
};
#endif // ZDSPCLIENT_H
