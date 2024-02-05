#include "zdspclient.h"
#include "zscpi_response_definitions.h"
#include <parse.h>
#include <QDataStream>

extern TMemSection dm32DspWorkspace;
extern TMemSection dm32DialogWorkSpace;
extern TMemSection dm32UserWorkSpace;
extern TMemSection dm32CmdList;
extern TMemSection symbConsts1;

cZDSP1Client::cZDSP1Client(AbstractFactoryDeviceNodeDspPtr deviceNodeFactory) :
    m_deviceNodeFactory(deviceNodeFactory)
{
}

cZDSP1Client::cZDSP1Client(int socket, VeinTcp::TcpPeer* netclient, AbstractFactoryDeviceNodeDspPtr deviceNodeFactory) :
    cZDSP1Client(deviceNodeFactory)
{
    init(socket, netclient);
}

void cZDSP1Client::init(int socket, VeinTcp::TcpPeer *netclient)
{
    m_socket = socket;
    m_pNetClient = netclient;
    m_sCmdListDef = m_sIntCmdListDef = "Empty"; // alle listen default leer
    cDspCmd DspCmd;
    m_DspCmdList.append(DspCmd);
    m_DspIntCmdList.append(DspCmd);
    m_dspVarResolver.addSection( &dm32DspWorkspace);
    m_dspVarResolver.addSection( &dm32DialogWorkSpace);
    m_dspVarResolver.addSection( &dm32UserWorkSpace);
    m_dspVarResolver.addSection( &dm32CmdList);
    m_dspVarResolver.addSection( &symbConsts1);
    m_dspVarResolver.addSection( &m_memorySection);
    m_memorySection.StartAdr = m_memorySection.n = 0; m_memorySection.Section = userSection;
    m_dspVarResolver.setVarHash(); // wir setzen die hashtabelle und initialisieren diese

    Encryption = 0; // es werden alle var. abfragen im klartext gesendet
    m_bActive = false;
}

QString& cZDSP1Client::setRawActualValueList(QString& s)
{
    m_dspRawActualValueVarList.clear();
    sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
    if (!s.isEmpty()) {
        DspVarClientPerspective getDspVar;
        int localOffset = 0;
        int globaloffset = 0;
        for (int i=0; ; i++) {
            QString section = s.section(';', i, i); // alle teil strings bearbeiten
            if (section.isEmpty())
                break;
            if ( getDspVar.Init(section) ) {
                if (getDspVar.segment() == localSegment) {
                    getDspVar.SetOffs(localOffset);
                    localOffset += getDspVar.size();
                }
                else {
                    getDspVar.SetOffs(globaloffset);
                    globaloffset += getDspVar.size();
                }
                m_dspRawActualValueVarList.append(getDspVar);
            }
            else { // fehlerfall
                m_dspRawActualValueVarList.clear();
                sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                break;
            }
        }
    }
    m_memorySection.n = m_dspRawActualValueVarList.count();
    if (m_memorySection.n > 0) { // wir haben mindestens 1 variable
        m_dspVarArray.resize(m_memorySection.n);
        for (int i = 0;i < m_memorySection.n; i++) { // und machen diese dem resolver zugänglich
            m_dspVarArray[i].m_clientHandleName = m_dspRawActualValueVarList[i].getClientHandleName();
            m_dspVarArray[i].Name = m_dspRawActualValueVarList[i].name();
            m_dspVarArray[i].size = m_dspRawActualValueVarList[i].size();
            m_dspVarArray[i].offs = m_dspRawActualValueVarList[i].offs();
            m_dspVarArray[i].type = (dType)m_dspRawActualValueVarList[i].type();
            m_dspVarArray[i].segment = (segmentType)m_dspRawActualValueVarList[i].segment();
        }
        m_memorySection.DspVar = m_dspVarArray.data();
    }
    m_dspVarResolver.setVarHash(); // wir setzen die hashtabelle neu
    return (sOutput);
}

QString& cZDSP1Client::getRawActualValueList()
{
    sOutput = "";
    QTextStream ts( &sOutput, QIODevice::WriteOnly );
    if ( !m_dspRawActualValueVarList.empty() ) {
        QList<DspVarClientPerspective>::iterator it;
        for ( it = m_dspRawActualValueVarList.begin(); it != m_dspRawActualValueVarList.end(); ++it )
            ts << (*it).name() << ',' << (*it).size() << ';';
    }
    else
        ts << "Empty";
    return(sOutput);
}

int cZDSP1Client::GetEncryption()
{
    return(Encryption);
}

void cZDSP1Client::SetEncryption(int i)
{
    Encryption = i;
}

QString& cZDSP1Client::SetCmdListDef(QString& s)
{
    m_sCmdListDef = s;
    sOutput = ZSCPI::scpiAnswer[ZSCPI::ack]; // ist erstmal ok, wird später beim SET kommando geprüft
    return sOutput;
}

QString& cZDSP1Client::GetCmdListDef()
{
    return m_sCmdListDef;
}

QString& cZDSP1Client::SetCmdIntListDef(QString& s)
{
    m_sIntCmdListDef = s;
    sOutput = ZSCPI::scpiAnswer[ZSCPI::ack]; // ist erstmal ok, wird später beim SET kommando geprüft
    return sOutput;
}

QString& cZDSP1Client::GetCmdIntListDef()
{
    return m_sIntCmdListDef;
}

bool cZDSP1Client::syntaxCheck(QString& s)
{
    int openPos = s.indexOf('(');
    int closePos = s.indexOf(')');
    bool ok = openPos > 0 && closePos>openPos;
    if(!ok)
        qCritical("Invalid command syntax: '%s'", qPrintable(s));
    return ok;
}

cDspCmd cZDSP1Client::GenDspCmd(QString cmd, bool* ok, ulong userMemoryOffset, ulong globalstartadr)
{
    cParse CmdParser;
    CmdParser.SetDelimiter("(,)"); // setze die trennzeichen für den parser
    CmdParser.SetWhiteSpace(" (,)");
    if(!syntaxCheck(cmd)) {
        *ok = false;
        return cDspCmd();
    }
    const QChar* cmds = cmd.data();
    QString sSearch = CmdParser.GetKeyword(&cmds); // das 1. keyword muss ein befehlscode sein
    sDspCmd *dspcmd = findDspCmd(sSearch);
    if (dspcmd)
    { // bekannter befehlscode ?
        switch (dspcmd->CmdClass) {
        case CMD: // nur kommandowort, kein parameter
        {
            sSearch = CmdParser.GetKeyword(&cmds);
            *ok = sSearch.isEmpty(); // hier darf nichts stehen
            cDspCmd lcmd;
            if (*ok)
                lcmd = cDspCmd(dspcmd->CmdCode);
            return lcmd;
        }
        case CMD1i16: // kommandowort, ein parameter
        {
            short par;
            bool t = true;
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= ( (par = m_dspVarResolver.offs(sSearch, userMemoryOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            cDspCmd lcmd;
            if (t)
                lcmd = cDspCmd(dspcmd->CmdCode,(ushort)par);
            *ok = t;
            return lcmd;
        }
        case CMD2i16:
        {
            short par[2];
            bool t = true;
            for (int i=0; i<2; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                t &= ( (par[i] = m_dspVarResolver.offs(sSearch, userMemoryOffset, globalstartadr)) > -1);
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            cDspCmd lcmd;
            if (t) {
                lcmd = cDspCmd(dspcmd->CmdCode, (ushort)par[0], (ushort)par[1]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_socket << 16);
            }
            *ok = t;
            return lcmd;
        }
        case CMD3i16:
        {
            short par[3];
            bool t = true;
            for (int i=0; i<3; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                t &= ( (par[i] = m_dspVarResolver.offs(sSearch, userMemoryOffset, globalstartadr)) > -1);
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            cDspCmd lcmd;
            if (t) {
                lcmd = cDspCmd( dspcmd->CmdCode, (ushort)par[0], (ushort)par[1], (ushort)par[2]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_socket << 16);
            }
            *ok = t;
            return lcmd;
        }
        case CMD1i32:
        {
            long par;
            sSearch = CmdParser.GetKeyword(&cmds);
            bool t = ( (par = m_dspVarResolver.offs(sSearch, userMemoryOffset, globalstartadr)) > -1);
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            cDspCmd lcmd;
            if (t)
                lcmd = cDspCmd(dspcmd->CmdCode,(ulong)par);
            *ok = t;
            return lcmd;
        }
        case CMD1i161fi32:
        {
            short par1;
            long par2 = 0;
            sSearch = CmdParser.GetKeyword(&cmds);
            *ok = ( (par1 = m_dspVarResolver.offs(sSearch, userMemoryOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            cDspCmd lcmd;
            if (!(*ok))
                return lcmd; // wenn fehler -> fertig
            sSearch = CmdParser.GetKeyword(&cmds);
            bool t;
            par2 = sSearch.toLong(&t); // test auf integer
            if (!t)
                par2 = sSearch.toLong(&t,16); // test auf hex
            if (!t)  {
                float tf = sSearch.toFloat(&t);
                long* pl = (long*) &tf;
                par2= *pl;
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t)
                lcmd = cDspCmd(dspcmd->CmdCode,(ushort)par1,(ulong)par2);
            *ok = t;
            return lcmd;
        }
        }
    }
    *ok = false;
    return cDspCmd();
}


void cZDSP1Client::SetActive(bool b)
{
    m_bActive = b;
}


ulong cZDSP1Client::setStartAdr(ulong startAdress, ulong globalMemStart)
{
    ulong usermemsize, globalmemsize;

    usermemsize = globalmemsize = 0;
    m_memorySection.StartAdr = startAdress;

    for (int i = 0; i < m_memorySection.n; i++)
    {
        if (m_memorySection.DspVar[i].segment == localSegment)
        {
            m_memorySection.DspVar[i].adr = startAdress + usermemsize; // we need the adress for reading back data
            usermemsize += m_memorySection.DspVar[i].size;
        }
        else
        {
            m_memorySection.DspVar[i].adr = globalMemStart+globalmemsize;
            globalmemsize += m_memorySection.DspVar[i].size;
        }
    }
    return usermemsize;
}


bool cZDSP1Client::GenCmdList(QString& s, QList<cDspCmd> &cl, QString& errs, ulong umo, ulong globalstartadr)
{
    bool ok = true;
    cl.clear();
    for (int i = 0;;i++)
    {
        QString cs = s.section(';',i,i);
        if ( (cs.isEmpty()) || (cs==("Empty")) )break; // liste ist durch
        cl.append(GenDspCmd(cs, &ok, umo,globalstartadr));
        if (!ok)
        {
            errs = cs;
            break;
        }
    }
    return ok;
}


bool cZDSP1Client::GenCmdLists(QString& errs, ulong umo, ulong globalstartadr)
{
    bool ok;
    ok = GenCmdList(m_sCmdListDef,m_DspCmdList,errs,umo,globalstartadr);
    if (ok) ok =  GenCmdList(m_sIntCmdListDef, m_DspIntCmdList,errs,umo,globalstartadr);
    return ok;
}


bool cZDSP1Client::isActive()
{
    return m_bActive;
}


QList<cDspCmd> &cZDSP1Client::GetDspCmdList()
{
    return (m_DspCmdList);
}


QList<cDspCmd> &cZDSP1Client::GetDspIntCmdList()
{
    return (m_DspIntCmdList);
}


int cZDSP1Client::getSocket()
{
    return m_socket;
}


QString& cZDSP1Client::readActValues(QString& s)
{
    QString par(s);

    if (par.isEmpty())
    { // sonderfall liste leer -> alle messwerte lesen
        for (int i = 0; i < m_dspRawActualValueVarList.count(); i++)
        {
            DspVarClientPerspective Var = m_dspRawActualValueVarList.at(i);
            par += QString("%1,%2;").arg(Var.name()).arg(Var.size());
        }
    }
    return DspVarListRead(par);
}


bool cZDSP1Client::readDspVarInt(QString varName, int& intval)
{ // einen int (32bit) wert lesen
    bool ret = false;
    QByteArray ba;
    QString ss = QString("%1,1").arg(varName);
    if ( DspVarRead(ss, &ba) != 0) {
        // 1 wort ab name (s) lesen
        intval = *((int*) (ba.data()));
        ret = true;
    }
    return ret;
}


TDspVar* cZDSP1Client::DspVarRead(QString nameLen, QByteArray* varRead)
{
    QString name = nameLen.section(",",0,0);
    TDspVar *DspVar = m_dspVarResolver.getDspVar(name);
    if (!DspVar)
        return nullptr; // fehler, den namen gibt es nicht

    QString countSection = nameLen.section(",",1,1);
    bool ok;
    const int countVars = countSection.toInt(&ok);
    if (!ok || (countVars<1) )
        return nullptr; // fehler in der anzahl der elemente

    const int countBytes = countVars * 4;
    varRead->resize(countBytes);

    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    if ( (deviceNode->lseek(DspVar->adr) >= 0) && (deviceNode->read(varRead->data(), countBytes ) >= 0) )
        return DspVar; // dev.  seek und dev. read ok

    return nullptr; // sonst fehler
}


char* cZDSP1Client::qSEncryption(char* ch,int n )
{
    char* tm1;
    short* tm2=new short[n+1]; // sind dopp elt soviele bytes wie in ba (+2)
    tm1=(char*) tm2; // zeiger um daten an string zu koppeln
    char c;
    for (int j=0;j<n;j++,ch++) {
        c=*ch;
        *tm2++ = (((c << 4) | c) & 0x0F0F) | 0x3030;
    }
    *tm2='!'; // delimiter ! weil ; in daten sein kann . die 0 als abschluss ist hier drin ....ich weiß
    return tm1;
}


QString& cZDSP1Client::DspVarListRead(QString& s)
{
    bool ok=false;
    sOutput="";
    QTextStream ts( &sOutput, QIODevice::WriteOnly );
    ts.setRealNumberPrecision(8);
    QByteArray ba;

    for (int i=0;;i++)
    {
        QString vs = s.section(";",i,i); // variablen string:  varname, anzahl werte
        if (vs.isEmpty()) {
            ok = true;
            break; // dann sind wir fertig
        }

        TDspVar *DspVar;
        if ( (DspVar = DspVarRead(vs, &ba)) == 0) break; // fehler aufgetreten

        int n = ba.size()/4;
        if (Encryption)
        {
            n = ba.size();
            char* c;
            sOutput +=QString("%1%2").arg(DspVar->Name).arg(":");
            sOutput += QString(c = qSEncryption((char*)(ba.data()),n));
            delete c;
        }
        else
        {
            ts << DspVar->Name << ":";
            switch (DspVar->type)
            {
            case eInt :{
                ulong *ul = (ulong*) ba.data();
                for (int j = 0; j < n-1; j++,ul++) ts << (*ul) << "," ;
                ts << *ul << ";" ;
                break;}
            case eUnknown:
            case eFloat :{
                float *f = (float*) ba.data();
                for (int j = 0; j < n-1; j++,f++) ts << (*f) << "," ;
                ts << *f << ";" ;
                break;}
            }
        };
    }
    if (!ok)
        sOutput = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return sOutput;
}


QString cZDSP1Client::DspVarWriteRM(QString& s)
{
    if ( DspVarWrite(s) )
        return ZSCPI::scpiAnswer[ZSCPI::ack];
    else
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
}


bool cZDSP1Client::DspVarWrite(QString s)
{
    const int gran = 10; // immer 10 elemente allokieren
    bool ok = false;

    for (int i=0;;i++) {
        QString vs = s.section(";",i,i);
        if (vs.isEmpty()) {
            ok = true;
            break; // dann sind wir fertig
        }
        QString name = vs.section(",",0,0);
        long adr = m_dspVarResolver.adr(name);
        if (adr == -1)
            break; // fehler, den namen gibt es nicht

        int n,alloc;
        n = alloc = 0; // keine elemente

        QByteArray ba;
        QDataStream bas (&ba, QIODevice::Unbuffered | QIODevice::ReadWrite);
        bas.setByteOrder(QDataStream::LittleEndian);
        bas.setFloatingPointPrecision(QDataStream::SinglePrecision);
        bool toNumberConvertOk = true;

        int type = m_dspVarResolver.type(name);
        if (type == eUnknown) {
            for (int j=1;;j++) {
                QString p = vs.section(",",j,j);
                if (p.isEmpty())
                    break;
                if (++n > alloc) {
                    alloc += gran;
                    ba.resize(alloc*4);
                }
                qint32 vl = p.toLong(&toNumberConvertOk); // test auf long
                if (toNumberConvertOk)
                    bas << vl;
                else {
                    quint32 vul = p.toULong(&toNumberConvertOk); // test auf ulong
                    if (toNumberConvertOk)
                        bas << vul;
                    else {
                        float vf = p.toFloat(&toNumberConvertOk); // test auf float
                        if (toNumberConvertOk)
                            bas << vf;
                    }
                }
                if(!toNumberConvertOk)
                    break;
            }
        }
        else {
            for (int j=1;;j++) {
                QString p = vs.section(",", j, j);
                if (p.isEmpty())
                    break;
                if (++n > alloc) {
                    alloc += gran;
                    ba.resize(alloc*4);
                }
                if (type == eInt) {
                    qint32 vl = p.toLong(&toNumberConvertOk); // test auf long
                    if (toNumberConvertOk)
                        bas << vl;
                    else {
                        quint32 vul = p.toULong(&toNumberConvertOk); // test auf ulong
                        if (toNumberConvertOk)
                            bas << vul;
                    }
                }
                else {
                    float vf = p.toFloat(&toNumberConvertOk); // test auf float
                    if (toNumberConvertOk)
                        bas << vf;
                }
                if (!toNumberConvertOk)
                    break;
            }
        }
        if (!toNumberConvertOk)
            break;
        AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
        if (n > 0 && !deviceNode->write(adr, ba.data(), n*4 ))
            break;
    }
    return ok;
}

