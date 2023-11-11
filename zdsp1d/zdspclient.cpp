#include "zdspclient.h"
#include "zdsp1d.h"
#include "zeraglobal.h"
#include <QDataStream>

extern sMemSection dm32DspWorkspace;
extern sMemSection dm32DialogWorkSpace;
extern sMemSection dm32UserWorkSpace;
extern sMemSection dm32CmdList;
extern sMemSection symbConsts1;

cZDSP1Client::cZDSP1Client(int socket, XiQNetPeer* netclient, cZDSP1Server* server)
{
    init(socket, netclient, server);
}

void cZDSP1Client::init(int socket, XiQNetPeer *netclient, cZDSP1Server *server)
{
    sock = socket;
    m_pNetClient = netclient;
    m_sCmdListDef = m_sIntCmdListDef = "Empty"; // alle listen default leer
    cDspCmd DspCmd;
    m_DspCmdList.append(DspCmd);
    m_DspIntCmdList.append(DspCmd);
    myServer = server;
    DspVarResolver.addSection( &dm32DspWorkspace);
    DspVarResolver.addSection( &dm32DialogWorkSpace);
    DspVarResolver.addSection( &dm32UserWorkSpace);
    DspVarResolver.addSection( &dm32CmdList);
    DspVarResolver.addSection( &symbConsts1);
    DspVarResolver.addSection( &msec);
    msec.StartAdr = msec.n = 0; msec.Section = userSection;
    DspVarResolver.setVarHash(); // wir setzen die hashtabelle und initialisieren diese

    Encryption = 0; // es werden alle var. abfragen im klartext gesendet
    m_bActive = false;
}

QString& cZDSP1Client::SetRavList(QString& s)
{
    int i;
    int localOffset, globaloffset;

    i = localOffset = globaloffset = 0;
    m_DspVarList.clear(); // liste löschen

    sOutput = ACKString;
    if (!s.isEmpty())
    {
        cDspClientVar v;
        for (i=0;;i++)
        {
            QString t = s.section(';',i,i); // alle teil strings bearbeiten
            if (t.isEmpty()) break; // dann sind wir fertig
            if ( v.Init(t) )
            {
                if (v.segment() == localSegment)
                {
                    v.SetOffs(localOffset);
                    localOffset += v.size();
                }
                else
                {
                    v.SetOffs(globaloffset);
                    globaloffset += v.size();
                }

                m_DspVarList.append(v);
            }
            else
            { // fehlerfall
                m_DspVarList.clear(); // liste löschen
                sOutput = NACKString;
                break;
            }
        }
    }

    msec.n = m_DspVarList.count();

    if (msec.n > 0)
    { // wir haben mindestens 1 variable
        varArray.resize(msec.n);

        for (i = 0;i < msec.n; i++)
        { // und machen diese dem resolver zugänglich
            varArray[i].Name = m_DspVarList[i].name();
            varArray[i].size = m_DspVarList[i].size();
            varArray[i].offs = m_DspVarList[i].offs();
            varArray[i].type = (dType)m_DspVarList[i].type();
            varArray[i].segment = (segmentType)m_DspVarList[i].segment();
        }

        msec.DspVar = varArray.data();
    }

    DspVarResolver.setVarHash(); // wir setzen die hashtabelle neu

    return (sOutput);
}


QString& cZDSP1Client::GetRavList()
{
    sOutput = "";
    QTextStream ts( &sOutput, QIODevice::WriteOnly );
    if ( !m_DspVarList.empty() )
    {
        QList<cDspClientVar>::iterator it;
        for ( it = m_DspVarList.begin(); it != m_DspVarList.end(); ++it )
        {
            ts << (*it).name() << ',' << (*it).size() << ';';
        }
    }
    else ts << "Empty";
    return(sOutput);
}


int cZDSP1Client::GetEncryption()
{
    return(Encryption);
}


void cZDSP1Client::SetEncryption(int i)
{
    Encryption=i;
}


QString& cZDSP1Client::SetCmdListDef(QString& s)
{
    m_sCmdListDef = s;
    sOutput = ACKString; // ist erstmal ok, wird später beim SET kommando geprüft
    return (sOutput);
}


QString& cZDSP1Client::GetCmdListDef()
{
    return (m_sCmdListDef);
}


QString& cZDSP1Client::SetCmdIntListDef(QString& s)
{
    m_sIntCmdListDef = s;
    sOutput = ACKString; // ist erstmal ok, wird später beim SET kommando geprüft
    return (sOutput);
}


QString& cZDSP1Client::GetCmdIntListDef()
{
    return (m_sIntCmdListDef);
}


bool cZDSP1Client::syntaxCheck(QString& s)
{
    int p1,p2=-1;
    bool ok = ( (((p1 = s.indexOf('(')) > 0) && ((p2 = s.indexOf(')')) > 0)) || (p2>p1) );
    // bool ok = ( (((p1 = s.find('(')) > 0) && ((p2 = s.find(')')) > 0)) || (p2>p1) );
    return ok;
}


cDspCmd cZDSP1Client::GenDspCmd(QString& scmd,bool* ok, ulong umo, ulong globalstartadr)
{
    cParseZdsp CmdParser;
    CmdParser.SetDelimiter("(,)"); // setze die trennzeichen für den parser
    CmdParser.SetWhiteSpace(" (,)");

    QChar* cmds = scmd.data(); // zeiger auf den C-string von scmd
    sDspCmd *dspcmd;
    cDspCmd lcmd, cmd;
    QString sSearch = CmdParser.GetKeyword(&cmds); // das 1. keyword muss ein befehlscode sein
    if ( ((dspcmd = findDspCmd(sSearch)) != NULL ) && syntaxCheck(scmd) )
    { // bekannter befehlscode ?
        switch (dspcmd->CmdClass) {
        case CMD: // nur kommandowort, kein parameter
        {
            sSearch = CmdParser.GetKeyword(&cmds);
            *ok = sSearch.isEmpty(); // hier darf nichts stehen
            if (*ok) lcmd = cDspCmd(dspcmd->CmdCode);
            return lcmd;
        }
        case CMD1i16: // kommandowort, ein parameter
        {
            short par;
            bool t = true;
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= ( (par = DspVarResolver.offs(sSearch, umo,globalstartadr)) > -1); // -1 ist fehlerbedingung
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t) lcmd = cDspCmd(dspcmd->CmdCode,(ushort)par);
            *ok = t;
            return lcmd;
        }
        case CMD2i16:
        {
            short par[2];
            bool t = true;
            for (int i=0; i<2; i++)
            {
                sSearch = CmdParser.GetKeyword(&cmds);
                t &= ( (par[i] = DspVarResolver.offs(sSearch, umo,globalstartadr)) > -1);
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t) {
                lcmd = cDspCmd(dspcmd->CmdCode, (ushort)par[0], (ushort)par[1]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (sock << 16);
            }
            *ok = t;
            return lcmd;
        }
        case CMD3i16:
        {
            short par[3];
            bool t = true;
            int i;
            for (i=0; i<3; i++)
            {
                sSearch = CmdParser.GetKeyword(&cmds);

                t &= ( (par[i] = DspVarResolver.offs(sSearch, umo,globalstartadr)) > -1);
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t)
            {
                lcmd = cDspCmd( dspcmd->CmdCode, (ushort)par[0], (ushort)par[1], (ushort)par[2]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (sock << 16);
            }

            *ok = t;
            return lcmd;
        }
        case CMD1i32:
        {
            long par;
            bool t;
            sSearch = CmdParser.GetKeyword(&cmds);
            t = ( (par = DspVarResolver.offs(sSearch,umo,globalstartadr)) > -1);
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t) lcmd = cDspCmd(dspcmd->CmdCode,(ulong)par);
            *ok = t;
            return lcmd;
        }
        case CMD1i161fi32:
        {
            short par1;
            long par2 = 0;
            bool t;
            sSearch = CmdParser.GetKeyword(&cmds);
            *ok = ( (par1 = DspVarResolver.offs(sSearch,umo,globalstartadr)) > -1); // -1 ist fehlerbedingung
            if (!(*ok))
                return lcmd; // wenn fehler -> fertig
            sSearch = CmdParser.GetKeyword(&cmds);
            par2 = sSearch.toLong(&t); // test auf integer
            if (!t) par2 = sSearch.toLong(&t,16); // test auf hex
            if (!t)  {
                float tf = sSearch.toFloat(&t);
                long* pl = (long*) &tf;
                par2= *pl;
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t) lcmd = cDspCmd(dspcmd->CmdCode,(ushort)par1,(ulong)par2);
            *ok = t;
            return lcmd;
        }
        }
    }
    *ok = false;
    return cmd;
}


void cZDSP1Client::SetActive(bool b)
{
    m_bActive = b;
}


ulong cZDSP1Client::setStartAdr(ulong sa, ulong globalmemstart)
{
    ulong usermemsize, globalmemsize;

    usermemsize = globalmemsize = 0;
    msec.StartAdr = sa;

    for (int i = 0; i < msec.n; i++)
    {
        if (msec.DspVar[i].segment == localSegment)
        {
            msec.DspVar[i].adr = sa + usermemsize; // we need the adress for reading back data
            usermemsize += msec.DspVar[i].size;
        }
        else
        {
            msec.DspVar[i].adr = globalmemstart+globalmemsize;
            globalmemsize += msec.DspVar[i].size;
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
    return sock;
}


QString& cZDSP1Client::readActValues(QString& s)
{
    QString par(s);

    if (par.isEmpty())
    { // sonderfall liste leer -> alle messwerte lesen
        for (int i = 0; i < m_DspVarList.count(); i++)
        {
            cDspClientVar Var = m_DspVarList.at(i);
            par += QString("%1,%2;").arg(Var.name()).arg(Var.size());
        }
    }
    return DspVarListRead(par);
}


bool cZDSP1Client::DspVar(QString& s,int& ir)
{ // einen int (32bit) wert lesen

    bool ret = false;
    QByteArray ba;
    QString ss = QString("%1,1").arg(s);
    if ( DspVarRead(ss, &ba) != 0)
    { // 1 wort ab name (s) lesen
        ir = *((int*) (ba.data()));
        ret = true;
    }
    return ret;
}


bool cZDSP1Client::DspVar(QString& s,float& fr)
{ // eine float wert lesen
    bool ret = false;
    QByteArray ba;
    QString ss = QString("%1,1").arg(s);
    if ( DspVarRead(ss, &ba) != 0)
    {  // 1 wort ab name(s) lesen
        fr = *((float*) (ba.data()));
        ret = true;
    }
    return ret;
}


sDspVar* cZDSP1Client::DspVarRead(QString& s, QByteArray* ba)
{
    bool ok;

    QString name = s.section(",",0,0);
    sDspVar *DspVar;

    if ( (DspVar = DspVarResolver.vadr(name)) == 0) return 0; // fehler, den namen gibt es nicht

    QString p = s.section(",",1,1);
    int n = p.toInt(&ok);
    if (!ok || (n<1) ) return 0; // fehler in der anzahl der elemente

    ba->resize(4*n);



    int fd = myServer->DevFileDescriptor;
    if ( (myServer->DspDevSeek(fd, DspVar->adr) >= 0) && (myServer->DspDevRead(fd, ba->data(), n*4 ) >= 0) )
    {
        return DspVar; // dev.  seek und dev. read ok
    }

    return 0; // sonst fehler
}


char* cZDSP1Client::qSEncryption(char* ch,int n )
{
    char* tm1;
    short* tm2=new short[n+1]; // sind dopp elt soviele bytes wie in ba (+2)
    tm1=(char*) tm2; // zeiger um daten an string zu koppeln
    char c;
    for (int j=0;j<n;j++,ch++)
    {
        c=*ch;
        *tm2++ = (((c << 4) | c) & 0x0F0F) | 0x3030;
        //  *tm2++=((c>>4) & 0xF) | 0x30;
        //  *tm2++=(c & 0xF) | 0x30;
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

        sDspVar *DspVar;
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
    if (!ok) sOutput=ERREXECString;
    return sOutput;
}


const char* cZDSP1Client::DspVarWriteRM(QString& s)
{
    if ( DspVarWrite(s) ) return ACKString;
    else return ERREXECString;
}


bool cZDSP1Client::DspVarWrite(QString& s)
{
    const int gran = 10; // immer 10 elemente allokieren
    bool ok=false;
    int fd = myServer->DevFileDescriptor;

    for (int i=0;;i++)
    {
        QString vs = s.section(";",i,i);
        if (vs.isEmpty())
        {
            ok = true;
            break; // dann sind wir fertig
        }
        QString name = vs.section(",",0,0);
        long adr;

        if ( (adr = DspVarResolver.adr(name) ) == -1) break; // fehler, den namen gibt es nicht

        int n,alloc;
        n = alloc = 0; // keine elemente

        QByteArray ba;
        QDataStream bas ( &ba, QIODevice::Unbuffered | QIODevice::ReadWrite );
        bas.setByteOrder(QDataStream::LittleEndian);
        bas.setFloatingPointPrecision(QDataStream::SinglePrecision);
        bool ok2 = true;
        int type;
        type = DspVarResolver.type(name);
        if (type == eUnknown)
        {
            for (int j=1;;j++)
            {
                QString p = vs.section(",",j,j);
                if (p.isEmpty()) break;
                if (++n > alloc)
                {
                    alloc += gran;
                    ba.resize(alloc*4);
                }
                qint32 vl = p.toLong(&ok2); // test auf long
                if (ok2) bas << vl;
                else {
                    quint32 vul = p.toULong(&ok2); // test auf ulong
                    if (ok2) bas << vul;
                    else {
                        float vf = p.toFloat(&ok2); // test auf float
                        if (ok2) bas << vf;
                    }
                }
                if (!ok2) break;
            }
        }
        else
        {
            for (int j=1;;j++)
            {
                QString p = vs.section(",",j,j);
                if (p.isEmpty()) break;
                if (++n > alloc)
                {
                    alloc += gran;
                    ba.resize(alloc*4);
                }

                if (type == eInt)
                {
                    qint32 vl = p.toLong(&ok2); // test auf long
                    if (ok2)
                        bas << vl;
                    else
                    {
                        quint32 vul = p.toULong(&ok2); // test auf ulong
                        if (ok2) bas << vul;
                    }
                }
                else
                {
                    float vf = p.toFloat(&ok2); // test auf float
                    if (ok2)
                        bas << vf;
                }
                if (!ok2) break;
            }
        }

        if (!ok2) break;
        if (n>0) {
            if (myServer->DspDevSeek(fd, adr) < 0) break; // file positionieren
            if (myServer->DspDevWrite(fd, ba.data(), n*4 ) < 0) break; // fehler beim schreiben
        }
    }
    return ok;
}

