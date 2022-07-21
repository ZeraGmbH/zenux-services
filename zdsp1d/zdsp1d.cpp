// implementation des eigenlichen servers zdsp1(d)

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDebug>
#include <QCoreApplication>
#include <xmlconfigreader.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDataStream>
#include <QFile>
#include <qbuffer.h>
#include <QTcpSocket>
#include <QTcpServer>
#include <QCryptographicHash>

#include <QTextStream>
#include <xiqnetserver.h>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>

#include "zeraglobal.h"
#include "zdspglobal.h"
#include "zhserver.h"
#include "zdsp1d.h"
#include "dsp.h"
#include "parse.h"
#include "debugsettings.h"
#include "dspsettings.h"
#include "ethsettings.h"
#include "rmconnection.h"

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

#define ADSP_IOC_MAGIC 'a'
/* ioctl commands */
#define ADSP_RESET _IOR(ADSP_IOC_MAGIC,1,char*)
#define ADSP_BOOT _IOR(ADSP_IOC_MAGIC,2,char*)
#define ADSP_INT_REQ _IOR(ADSP_IOC_MAGIC,3,char*)
#define ADSP_INT_ENABLE _IOR(ADSP_IOC_MAGIC,4,char*)
#define ADSP_INT_DISABLE _IOR(ADSP_IOC_MAGIC,5,char*)
#define IO_READ _IOR(ADSP_IOC_MAGIC,6,char*)


extern sMemSection dm32DspWorkspace;
extern sMemSection dm32DialogWorkSpace;
extern sMemSection dm32UserWorkSpace;
extern sMemSection dm32CmdList;
extern sMemSection symbConsts1;

extern sDspVar CmdListVar;
extern sDspVar UserWorkSpaceVar;

extern cNode* InitCmdTree();


static char devavail[6] = "avail";
static char devnavail[10]= "not avail";

static char dsprunning[8] = "running";
static char dspnrunning[12]= "not running";


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
    cParse CmdParser;
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
    QByteArray *ba = new QByteArray();
    QString ss = QString("%1,1").arg(s);
    if ( DspVarRead(ss,ba) != 0)
    { // 1 wort ab name (s) lesen
        ir = *((int*) (ba->data()));
        ret = true;
    }
    delete ba;

    return ret;
}


bool cZDSP1Client::DspVar(QString& s,float& fr)
{ // eine float wert lesen
    bool ret = false;
    QByteArray *ba = new QByteArray();
    QString ss = QString("%1,1").arg(s);
    if ( DspVarRead(ss,ba) != 0)
    {  // 1 wort ab name(s) lesen
        fr = *((float*) (ba->data()));
        ret = true;
    }
    delete ba;
    return ret;
}


sDspVar* cZDSP1Client::DspVarRead(QString& s,QByteArray* ba)
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
    QByteArray *ba = new QByteArray();

    for (int i=0;;i++)
    {
        QString vs = s.section(";",i,i); // variablen string:  varname, anzahl werte
        if (vs.isEmpty()) {
            ok = true;
            break; // dann sind wir fertig
        }

        sDspVar *DspVar;
        if ( (DspVar = DspVarRead(vs,ba)) == 0) break; // fehler aufgetreten

        int n = ba->size()/4;
        if (Encryption)
        {
            n=ba->size();
            char* c;
            sOutput +=QString("%1%2").arg(DspVar->Name).arg(":");
            sOutput += QString(c = qSEncryption((char*)(ba->data()),n));
            delete c;
        }
        else
        {
            ts << DspVar->Name << ":";
            switch (DspVar->type)
            {
            case eInt :{
                ulong *ul = (ulong*) ba->data();
                for (int j = 0; j < n-1; j++,ul++) ts << (*ul) << "," ;
                ts << *ul << ";" ;
                break;}
            case eUnknown:
            case eFloat :{
                float *f = (float*) ba->data();
                for (int j = 0; j < n-1; j++,f++) ts << (*f) << "," ;
                ts << *f << ";" ;
                break;}
            }
        };
    }
    delete ba;
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


/* globaler zeiger auf  "den"  server und eine signal behandlungsroutine */
cZDSP1Server* DSPServer;
int pipeFD[2];
char pipeBuf[2] = "I";
//int anzInt = 0;

void SigHandler(int)
{
    if (DSPServer->m_nDebugLevel & 2) syslog(LOG_INFO,"dsp interrupt received\n");
    // DSPServer->DspIntHandler();
    write(pipeFD[1], pipeBuf, 1);
}


struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;


cZDSP1Server::cZDSP1Server()
    :cZHServer()
{
    m_nSocketIdentifier = 0; // identifiers for clients, real fd's in former times

    m_pDebugSettings = 0;
    m_pETHSettings = 0;
    m_pDspSettings = 0;

    m_pInitializationMachine = new QStateMachine(this);
    myXMLConfigReader = new Zera::XMLConfig::cReader();

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish on any error condition

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    stateconnect2RMError = new QState(stateCONF);
    stateSendRMIdentandRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(myXMLConfigReader, SIGNAL(finishedParsingXML(bool)), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, SIGNAL(entered()), this, SLOT(doConfiguration()));
    QObject::connect(statesetupServer, SIGNAL(entered()), this, SLOT(doSetupServer()));
    QObject::connect(stateconnect2RM, SIGNAL(entered()), this, SLOT(doConnect2RM()));
    QObject::connect(stateconnect2RMError, SIGNAL(entered()), this, SLOT(connect2RMError()));
    QObject::connect(stateSendRMIdentandRegister, SIGNAL(entered()), this, SLOT(doIdentAndRegister()));
    QObject::connect(stateFINISH, SIGNAL(entered()), this, SLOT(doCloseServer()));

    m_pInitializationMachine->start();
 }


cZDSP1Server::~cZDSP1Server()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pETHSettings) delete  m_pETHSettings;
    if (m_pDspSettings) delete m_pDspSettings;

    if (clientlist.count() > 0)
        for (int i = 0; i < clientlist.count(); i++)
        {
            cZDSP1Client* cl = clientlist.at(i);
            delete cl;
        }

    resetDsp(); // we reset the dsp when we close the server

    close(DevFileDescriptor); // close dev.
    close(pipeFD[0]);
    close(pipeFD[1]);
}


void cZDSP1Server::doConfiguration()
{
    QStringList args;
    args = QCoreApplication::instance()->arguments();
    if (args.count() != 2) // we want exactly 1 parameter
    {
        m_nerror = parameterError;
        emit abortInit();
    }
    else
    {
        if ( pipe(pipeFD) == -1 )
        {
            m_nerror = pipeError;
            emit abortInit();
        }
        else
        {
            fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
            fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
            m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
            connect(m_pNotifier, SIGNAL(activated(int)), this, SLOT(DspIntHandler(int)));

            if (myXMLConfigReader->loadSchema(defaultXSDFile))
            {
                // we want to initialize all settings first
                m_pDebugSettings = new cDebugSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pDebugSettings,SLOT(configXMLInfo(const QString&)));
                m_pETHSettings = new cETHSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pETHSettings,SLOT(configXMLInfo(const QString&)));
                m_pDspSettings = new cDSPSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pDspSettings,SLOT(configXMLInfo(const QString&)));

                QString s = args.at(1);
                if(!myXMLConfigReader->loadXMLFile(s)) {
                    m_nerror = xmlfileError;
                    emit abortInit();
                }
            }
            else
            {
                m_nerror = xsdfileError;
                emit abortInit();
            }
        }
    }
}


void cZDSP1Server::doSetupServer()
{
    cParse* parser=new(cParse); // das ist der parser
    pCmdInterpreter=new cCmdInterpreter(this,InitCmdTree(),parser); // das ist der kommando interpreter

    m_sDspDeviceVersion = m_sDspSerialNumber = "Unknown"; // kennen wir erst mal nicht

    m_sDspBootPath = m_pDspSettings->getBootFile();

    DSPServer = this;

    ActivatedCmdList = 0; // der derzeit aktuelle kommando listen satz (0,1)

    m_nDebugLevel = m_pDebugSettings->getDebugLevel();

    myProtonetServer =  new XiQNetServer(this);
    myProtonetServer->setDefaultWrapper(&m_ProtobufWrapper);
    connect(myProtonetServer, SIGNAL(sigClientConnected(XiQNetPeer*)), this, SLOT(establishNewConnection(XiQNetPeer*)));
    myProtonetServer->startServer(m_pETHSettings->getPort(protobufserver)); // and can start the server now

    if (m_pETHSettings->isSCPIactive())
    {
        m_pSCPIServer = new QTcpServer(this);
        m_pSCPIServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_pSCPIServer, SIGNAL(newConnection()), this, SLOT(setSCPIConnection()));
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_pETHSettings->getPort(scpiserver));
    }

    m_sDspDeviceNode = m_pDspSettings->getDeviceNode(); // we try to open the dsp device
    if (DspDevOpen() < 0)
    {
        m_nerror = dspDeviceError; // and finish if not possible
        emit abortInit();
    }

    else
    {
        mySigAction.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&mySigAction.sa_mask);
        mySigAction. sa_flags = SA_RESTART;
        mySigAction.sa_restorer = NULL;
        sigaction(SIGIO, &mySigAction, NULL); // handler für sigio definieren
        SetFASync();
        m_nRetryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, SIGNAL(timeout()), this, SIGNAL(serverSetup()));

        if (setDspType()) // interrogate the mounted dsp device type and bootfile match
        {
            if (m_pDspSettings->isBoot()) // normally dsp gets booted by dsp server
            {
                if (resetDsp() && bootDsp()) // and try to reset and then boot it
                {
                    if (setSamplingSystem()) // now we try to set the dsp's sampling system
                    {
                        // our resource mananager connection must be opened after configuration is done
                        m_pRMConnection = new cRMConnection(m_pETHSettings->getRMIPadr(), m_pETHSettings->getPort(resourcemanager), m_pDebugSettings->getDebugLevel());
                        stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), stateSendRMIdentandRegister);
                        stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), stateconnect2RMError);
                        stateconnect2RMError->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);
                        emit serverSetup(); // so we enter state machine's next state
                    }
                    else
                    {
                        m_nerror = dspSetSamplingError;
                        emit abortInit();
                    }

                }
                else
                {
                    m_nerror = dspBootError;
                    emit abortInit();
                }
            }
            else // but for debugging purpose dsp is booted by ice
            {
                m_pRMConnection = new cRMConnection(m_pETHSettings->getRMIPadr(), m_pETHSettings->getPort(resourcemanager), m_pDebugSettings->getDebugLevel());
                stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), stateSendRMIdentandRegister);
                stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), stateconnect2RMError);
                stateconnect2RMError->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);
                emit serverSetup(); // so we enter state machine's next state

            }
        }
        else
        {
            m_nerror = dspBootFileError;
            emit abortInit();
        }
    }
}


void cZDSP1Server::doCloseServer()
{
    QCoreApplication::instance()->exit(m_nerror);
}


void cZDSP1Server::doConnect2RM()
{
    m_nerror = rmConnectionError; // preset error condition
    m_pRMConnection->connect2RM();
}


void cZDSP1Server::connect2RMError()
{
    m_nRetryRMConnect--;
    if (m_nRetryRMConnect == 0)
        emit abortInit();
    else
        m_retryTimer.start(200);
}


void cZDSP1Server::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(sServerName);

    quint32 port = m_pETHSettings->getPort(protobufserver);

    QString cmd, par;

    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP;DSP1;;ADSP Signal Processor;%1;")
                                 .arg(port));

    sDspVar* pDspVar = &CmdListVar;
    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP1;PGRMEMI;%1;DSP ProgramMemory(Interrupt);%2;")
                                 .arg(pDspVar->size)
                                 .arg(port));
    pDspVar++;

    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP1;PGRMEMC;%1;DSP ProgramMemory(Cyclic);%2;")
                                 .arg(pDspVar->size)
                                 .arg(port));

    pDspVar = &UserWorkSpaceVar;
    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP1;USERMEM;%1;DSP UserMemory;%2;")
                                 .arg(pDspVar->size)
                                 .arg(port));
#ifdef SYSTEMD_NOTIFICATION
    sd_notify(0, "READY=1");
#endif
}


int cZDSP1Server::DspDevOpen()
{
    if ( (DevFileDescriptor = open(m_sDspDeviceNode.toLatin1().data(), O_RDWR)) < 0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error opening dsp device: %s\n",m_sDspDeviceNode.toLatin1().data());
    }
    return DevFileDescriptor;
}


int cZDSP1Server::DspDevSeek(int fd, ulong adr)
{
    int r;
    if ( (r = lseek(fd,adr,0)) < 0 )
    {
        if  (DEBUG1)  syslog(LOG_ERR,"error positioning dsp device: %s\n",m_sDspDeviceNode.toLatin1().data());
    }
    return r;
}


int cZDSP1Server::DspDevWrite(int fd,char* buf,int len)
{
    int r;
    if ( (r = write(fd,buf,len)) <0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error writing dsp device: %s\n",m_sDspDeviceNode.toLatin1().data());
    }
    return r;
}


int cZDSP1Server::DspDevRead(int fd,char* buf,int len)
{
    int r;
    if ( (r = read(fd,buf,len)) <0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error reading dsp device: %s\n",m_sDspDeviceNode.toLatin1().data());
    }
    return r;
}


QString cZDSP1Server::mTestDsp(QChar* s)
{
    int nr, tmode;
    bool ok, tstart;

    tstart=false;

    QString par = pCmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    tmode=par.toInt(&ok);
    if ((ok) && ( (tmode>=0) && (tmode<2) ))
    {
        par = pCmdInterpreter->m_pParser->GetKeyword(&s);
        nr=par.toInt(&ok);
        if ((ok) && ( (nr>=0) && (nr<1000) ))
            tstart=true;
    }

    if (tstart == true)
    {
        int i,j;
        int errcount = 0;
        switch (tmode)
        {
            case 0:
                for (i=0; i<nr; i++)
                {
                    mResetDsp(s);
                    for (j=0; j< 100; j++)
                    {
                        usleep(1000);
                        if (Test4DspRunning() == false)
                            break;
                    }
                    if (j==100)
                        errcount++;
                    else
                    {
                        mBootDsp(s);
                        usleep(1000);
                        if (Test4DspRunning() == false)
                            errcount++;
                    }
                    Answer = QString("Test booting dsp %1 times, errors %2").arg(nr).arg(errcount);
                }
                break;

            case 1:
                const int n = 10000;
                int i,j;
                bool err;
                ulong faultadr;
                int bw, br, br2;
//                float tval;
                char byte;

                QByteArray ba; // wir werden 10000 floats in das array schreiben
                QByteArray ba2; // zurückgelesene daten
                ba.resize(n*4);
                ba2.resize(n*4);

//                QDataStream bas ( &ba, QIODevice::Unbuffered | QIODevice::ReadWrite );
//                bas.setByteOrder(QDataStream::LittleEndian);
//                bas.setFloatingPointPrecision(QDataStream::SinglePrecision);
//                tval = 1.0e32;
                byte = 0;
                for (i=0; i<n*4;i++)
                {
//                    tval *=-1.0;
//                    bas << (tval * (random() / RAND_MAX));
                    ba[i] = byte;
                    byte = (byte +1) % 256;
                }

                cZDSP1Client* cl = GetClient(ActSock);
                QString sadr  = "UWSPACE";
                ulong adr = cl->DspVarResolver.adr(sadr) ;
                for (i=0; i< nr; i++)
                {
                    if (DspDevSeek(DevFileDescriptor, adr) < 0)
                    {
                        Answer = QString("Test write/read dsp data, dev seek fault");
                        break; // file positionieren
                    }

                    if (DspDevWrite(DevFileDescriptor, ba.data(), n*4 ) < 0)
                    {
                        Answer = QString("Test write/read dsp data, dev write fault");
                        break; // fehler beim schreiben
                    }

                    if (DspDevSeek(DevFileDescriptor, adr) < 0)
                    {
                        Answer = QString("Test write/read dsp data, dev seek fault");
                        break; // file positionieren
                    }

                    if (DspDevRead(DevFileDescriptor, ba2.data(), n*4) < 0)
                    {
                        Answer = QString("Test write/read dsp data, dev read fault");
                        break; // fehler beim schreiben
                    }

                    err = false;
                    for (j=0; j<n*4; j++)
                    {
                        if (ba[j] != ba2[j])
                        {
                            bw = ba[j]; // das geschriebene byte
                            br = ba2[j]; // das gelesene byte
                            faultadr = adr + j;
                            DspDevRead(DevFileDescriptor, ba2.data(), n*4);
                            br2 = ba2[j];
                            err = true;
                        }
                        if (err)
                            break;
                    }

                    if (err)
                    {
                        Answer = QString("Test write/read dsp data, data fault adress %1, write %2, read1 %3, read2 %4").arg(faultadr,16).arg(bw,16).arg(br,16).arg(br2,16);
                        break; // file positionieren
                    }

                }

                if (i==nr)
                    Answer = QString("Test write/read dsp data, %1 times %2 bytes transferred, no errors").arg(nr).arg(n*4);

                break;

        }

    }

    else Answer = ERRVALString; // fehler wert

    return Answer;
}


bool cZDSP1Server::resetDsp()
{
    int r = ioctl(DevFileDescriptor,ADSP_RESET); // und reset

    if ( r < 0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error %d reset dsp device: %s\n",r,m_sDspDeviceNode.toLatin1().data());
        Answer = ERREXECString; // fehler bei der ausführung
        return false;
    }
    Answer = ACKString;
    return true;
}


QString cZDSP1Server::mResetDsp(QChar*)
{
    resetDsp();
    return Answer;
}


bool cZDSP1Server::bootDsp()
{
    QFile f (m_sDspBootPath);
    //qDebug() << m_sDspBootPath;
    if (!f.open(QIODevice::Unbuffered | QIODevice::ReadOnly))
    { // dsp bootfile öffnen
        if (DEBUG1)  syslog(LOG_ERR,"error opening dsp boot file: %s\n",m_sDspBootPath.toLatin1().data());
        Answer = ERRPATHString;
        return false;
    }

    //long len = f.size();
    QByteArray BootMem;
    //BootMem.resize(len);
    //f.readLine(BootMem.data(), len);
    BootMem = f.readAll();
    f.close();
    //qDebug() << "md5sum bootfile = " << QString(QCryptographicHash::hash(BootMem, QCryptographicHash::Md5).toHex());

    int r = ioctl(DevFileDescriptor,ADSP_BOOT,BootMem.data()); // und booten

    if ( r < 0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error %d booting dsp device: %s\n",r,m_sDspDeviceNode.toLatin1().data());
        Answer = ERREXECString; // fehler bei der ausführung
        return false;
    }

    Answer = ACKString;
    return true;
}


bool cZDSP1Server::setSamplingSystem()
{
    QString ss;
    for (int i = 0; i < 10; i++) // we try max. 10 times to set .... this should work
    {
        mCommand2Dsp(ss = QString("DSPCMDPAR,2,%1,%2,%3;").arg(m_pDspSettings->getChannelNr())
                                                                     .arg(m_pDspSettings->getSamplesSignalPeriod())
                                                                     .arg(m_pDspSettings->getsamplesMeasurePeriod()));
        if (Answer == ACKString)
            return true;
        usleep(10000); // give dsp a bit time before next try
    }

    return false;
}


QString cZDSP1Server::mBootDsp(QChar *)
{
    bootDsp();
    return Answer;
}


int cZDSP1Server::SetBootPath(const char * s)
{
    QString par = s;
    QFile bp(par);
    if ( bp.exists() )
    {
        m_sDspBootPath = par;
        return 0;
    }
    else return 1; // fehler path
}


QString cZDSP1Server::mSetDspBootPath(QChar *s)
{
    QString par = pCmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    if ( SetBootPath(par.toLatin1().data()) )
        Answer = ERRPATHString;
    else
        Answer = ACKString;
    return Answer;
}


QString cZDSP1Server::mGetDspBootPath()
{
    return m_sDspBootPath;
}


QString cZDSP1Server::mGetPCBSerialNumber()
{
    return m_sDspSerialNumber;
}


QString cZDSP1Server::mCommand2Dsp(QString& qs)
{
    // we need a client to do the job
    cZDSP1Client* cl = new cZDSP1Client(0, 0, this);

    do
    {
        Answer = ERREXECString;
        int ack;

        QString ss;
        if (! cl->DspVar( ss ="DSPACK",ack)) break;

        if ( ack ==  InProgress)
        {
            Answer = BUSYString;
            break;
        }

        if ( ack ==  CmdTimeout)
        {
            cl->DspVarWrite(ss = "DSPACK,0;");
            Answer = ERRTIMOString;
            break;
        }

        if (! cl->DspVarWrite(ss = "DSPACK,0;") ) break; // reset acknowledge
        if (! cl->DspVarWrite(qs)) break; // kommando und parameter -> dsp

        ioctl(DevFileDescriptor,ADSP_INT_REQ); // interrupt beim dsp auslösen
        Answer = ACKString; // sofort fertig melden ....sync. muss die applikation

    } while (0);

    delete cl;

    return Answer;
}


QString cZDSP1Server::mSetSamplingSystem(QChar *s)
{
    QString ss;

    return mCommand2Dsp(ss = QString("DSPCMDPAR,2,%1;").arg(QString(s)));
}


QString cZDSP1Server::mSetCommEncryption(QChar *s)
{
    bool ok;
    QString par = pCmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    int enc=par.toInt(&ok);
    if ((ok) && ( (enc==0) || (enc==1) ))
    {
        cZDSP1Client* cl = GetClient(ActSock);
        cl->SetEncryption(enc);
        Answer = ACKString; // acknowledge
    }
    else
        Answer = ERRVALString; // fehler wert

    return Answer;
}


QString cZDSP1Server::mGetSamplingSystem()
{
    do
    {
        Answer = ERREXECString;
        int n, ss, sm;

        cZDSP1Client* cl = GetClient(ActSock);

        QString s;
        if (! cl->DspVar(s = "NCHANNELS",n)) break;
        if (! cl->DspVar(s = "NSPERIOD",ss)) break;
        if (! cl->DspVar(s = "NSMEAS",sm)) break;

        Answer = QString("%1,%2,%3").arg(n).arg(ss).arg(sm);
    } while (0);

    return Answer;
}


QString cZDSP1Server::mGetCommEncryption()
{
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = QString::number(cl->GetEncryption());

    return Answer;
}


QString cZDSP1Server::mSetEN61850SourceAdr(QChar* s)
{
    int i;
    QByteArray* ba = new(QByteArray);
    QString ss(s);
    ushort adr[6];
    bool ok;
    for (i = 0;i < 6;i++)
    {
        QString t = ss.section(',',i,i); // versuch 6 voneinander mit , getrennte parameter zu lesen
        adr[i] = t.toUShort(&ok);
        if (ok) ok &= (adr[i] < 256); // test ob adr bytes < 256
        if (!ok) break;
    }
    if (!ok) Answer = NACKString;
    else
        do
    {
        Answer = ERREXECString; // vorbesetzen
        QString as;
        cZDSP1Client* cl = GetClient(ActSock);
        if (!cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3",ba)) break;
        else
        {
            ulong* pardsp = (ulong*) ba->data();
            pardsp[1] &= 0xFFFF0000; // die oberen bits behalten wir weil dest adr
            pardsp[1] = pardsp[1] | (adr[0] << 8) | adr[1];
            pardsp[2] = 0;
            for (i = 2;i<6;i++) pardsp[2] = (pardsp[2] << 8) + adr[i];
            mCommand2Dsp(as = QString("DSPCMDPAR,6,%1,%2,%3;").arg(pardsp[0]).arg(pardsp[1]).arg(pardsp[2])); // setzt answer schon
        }
    } while(0);
    delete ba;

    return Answer;
}


QString cZDSP1Server::mSetEN61850DestAdr(QChar *s)
{
    int i;
    QByteArray* ba = new(QByteArray);
    QString ss(s);
    ushort adr[6]; // 2 * 4 werte reservieren
    bool ok;
    for (i = 0;i < 6;i++)
    {
        QString t = ss.section(',',i,i); // versuch 6 voneinander mit , getrennte parameter zu lesen
        adr[i] = t.toUShort(&ok);
        if (ok) ok &= (adr[i] < 256); // test ob adr bytes < 256
        if (!ok) break;
    }
    if (!ok) Answer = NACKString;
    else
    do
    {
        Answer = ERREXECString; // vorbesetzen
        QString as;
        cZDSP1Client* cl = GetClient(ActSock);
        if (!cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3",ba)) break;
        else
        {
            ulong* pardsp = (ulong*) ba->data();
            pardsp[0] = 0;
            for (i = 0;i<4;i++) pardsp[0] = (pardsp[0] << 8) +adr[i];
            pardsp[1] &= 0xFFFF; // die unteren bits behalten wir weil source adr
            pardsp[1] = pardsp[1] | (adr[4] << 24) | (adr[5] << 16);
            mCommand2Dsp(as = QString("DSPCMDPAR,6,%1,%2,%3;").arg(pardsp[0]).arg(pardsp[1]).arg(pardsp[2]));
        }
    } while(0);

    delete ba;
    return Answer;
}


QString cZDSP1Server::mSetEN61850EthTypeAppId(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHTYPEAPPID,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mGetEN61850EthTypeAppId()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHTYPEAPPID,1",ba))
    {
        ulong *dataCount = (ulong*) ba->data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;
}


QString cZDSP1Server::mSetEN61850PriorityTagged(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHPRIORITYTAGGED,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mGetEN61850PriorityTagged()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHPRIORITYTAGGED,1",ba))
    {
        ulong *dataCount = (ulong*) ba->data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;

}


QString cZDSP1Server::mSetEN61850EthSync(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("SYNCASDU,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mGetEN61850EthSync()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "SYNCASDU,1",ba))
    {
        ulong *dataCount = (ulong*) ba->data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;

}



QString cZDSP1Server::mSetEN61850DataCount(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHDATACOUNT,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mGetEN61850DataCount()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHDATACOUNT,2",ba))
    {
        ulong *dataCount = (ulong*) ba->data(); // data zeigt auf 2*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0] << "," << dataCount[1];
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;
}


QString cZDSP1Server::mSetEN61850SyncLostCount(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHSYNCLOSTCOUNT,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mGetEN61850SyncLostCount()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHSYNCLOSTCOUNT,1",ba))
    {
        ulong *dataCount = (ulong*) ba->data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;
}


QString cZDSP1Server::mGetEN61850SourceAdr()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3",ba))
    {
        ulong* AdrByte = (ulong*) ba->data(); // data zeigt auf 3*4 byte
        ushort adr[6];  // dest, source address  sind je 6 byte
        int i;
        for (i = 0;i < 2;i++) adr[i] = ( AdrByte[1] >> ((1-i) * 8) ) & 0xFF;  // dsp byte order
        for (i = 0;i < 4;i++) adr[2+i] = ( AdrByte[2] >> ((3-i) * 8) ) & 0xFF; // -> network byte order
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        for (i = 0; i < 5; i++) ts << adr[i] << ",";
        ts << adr[i] << ";";
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;
}


QString cZDSP1Server::mGetEN61850DestAdr()
{
    QByteArray* ba = new(QByteArray);
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3",ba))
    {
        ulong* AdrByte = (ulong*) ba->data(); // data zeigt auf 3*4 byte
        ushort adr[6];  // dest, source address  sind je 6 byte
        int i;
        for (i = 0;i < 4;i++) adr[i] = ( AdrByte[0] >> ((3-i) * 8) ) & 0xFF;  // dsp byte order
        for (i = 0;i < 2;i++) adr[4+i] = ( AdrByte[1] >> ((3-i) * 8) ) & 0xFF; // -> network byte order
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        for (i = 0; i < 5; i++) ts << adr[i] << ",";
        ts << adr[i] << ";";
    }
    else
    {
        Answer = ERREXECString;
    }
    delete ba;

    return Answer;
}


QString cZDSP1Server::mSetDspCommandStat(QChar *s)
{
    Answer = ERREXECString;
    QString ss;

    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("DSPACK,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mGetDspCommandStat()
{
    int stat;
    cZDSP1Client* cl = GetClient(ActSock);

    QString s;
    if (! cl->DspVar(s = "DSPACK",stat))
        Answer = ERREXECString;
    else
        Answer = QString("%1").arg(stat);

    return Answer;
}


QString cZDSP1Server::mTriggerIntListHKSK(QChar *s)
{
    QString ss(s);
    ulong par = ss.toULong();
    par = (par & 0xFFFF )| (ActSock << 16);
    return mCommand2Dsp(ss = QString("DSPCMDPAR,4,%1;").arg(par)); // liste mit prozessNr u. HKSK
}


QString cZDSP1Server::mTriggerIntListALL(QChar *)
{
    QString ss;
    return mCommand2Dsp(ss = QString("DSPCMDPAR,1;"));
}


QString cZDSP1Server::mResetMaxima(QChar *)
{
    QString ss;
    return mCommand2Dsp(ss =  QString("DSPCMDPAR,3;"));
}


int cZDSP1Server::SetDeviceNode(char* s)
{ // nur beim start zu setzen, nicht während des ablaufs
    QString devn = s;
    QFile dn(devn);
    if (dn.exists())
    {
        m_sDspDeviceNode = devn;
        return 0;
    }
    else return 1;
}


int cZDSP1Server::SetDebugLevel(const char* s)
{
    QString p = s;
    int dl = p.toInt();
    if ( (dl>=0) && (dl<=MaxDebugLevel) )
    {
        m_nDebugLevel=dl;
        return 0;
    }
    else return 1;
}


QString cZDSP1Server::mGetDeviceVersion()
{
    int r;
    r = ioctl(DevFileDescriptor,IO_READ,VersionNr);

    if ( r < 0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error %d reading device version: %s\n",r,m_sDspDeviceNode.toLatin1().data());
        Answer = ERREXECString; // fehler bei der ausführung
        return Answer.toLatin1().data();
    }

    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "VNR,1;";
    p = cl->DspVarListRead(p);  // ab "VNR"  1 wort lesen
    p = p.section(':',1,1);
    p = p.remove(';');
    double d = p.toDouble();
    m_sDspDeviceVersion = QString("DSPLCA: V%1.%2;DSP V%3").arg((r >>8) & 0xff).arg(r & 0xff).arg(d,0,'f',2);

    return m_sDspDeviceVersion;
}


QString cZDSP1Server::mGetServerVersion()
{
    return sSoftwareVersion;
}


QString cZDSP1Server::mGetDspStatus()
{
    if ( Test4DspRunning() )
        Answer = dsprunning;
    else
        Answer = dspnrunning;

    return Answer;
}


QString cZDSP1Server::mGetDeviceStatus()
{
    if ( Test4HWPresent() )
        Answer = devavail;
    else
        Answer = devnavail;

    return Answer;
}


QString cZDSP1Server::mGetDeviceLoadAct()
{
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "BUSY,1;";
    Answer = cl->DspVarListRead(p);  // ab "BUSY"  1 wort lesen

    return Answer;
}


QString cZDSP1Server::mGetDeviceLoadMax()
{
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "BUSYMAX,1;";
    Answer = cl->DspVarListRead(p);  // ab "BUSYMAX"  1 wort lesen
    return Answer;
}


QString cZDSP1Server::mResetDeviceLoadMax()
{
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "BUSYMAX,0.0";
    Answer = cl->DspVarWriteRM(p);
    return Answer;
}


QDataStream& operator<<(QDataStream& ds,cDspCmd c)
{
    ds << (quint32) c.w[0] << (quint32) c.w[1];
    return ds;
}


void cZDSP1Server::DspIntHandler(int)
{ // behandelt den dsp interrupt

    QByteArray *ba;
    QString s;
    cZDSP1Client *client,*client2;
    char buf[2];
    int process = 0;

    read(pipeFD[0], buf, 1); // first we read the pipe

    if (true /*anzInt ==0*/)
    {

    if ((!clientlist.isEmpty()) && (client = clientlist.first()) !=0) // wenn vorhanden nutzen wir immer den 1. client zum lesen
    {
        //anzInt++;
        ba = new QByteArray();

        if (client->DspVarRead(s = "CTRLCMDPAR,20", ba)) // 20 worte lesen
        {
            ulong* pardsp = (ulong*) ba->data();
            int n = pardsp[0]; // anzahl der interrupts
            //qDebug() << QString("Interrupts n=%1").arg(n);
            for (int i = 1; i < (n+1); i++)
            {
                process = pardsp[i] >> 16;
                if ((client2 = GetClient(process)) !=0) // gibts den client noch, der den interrupt haben wollte
                {
                    s = QString("DSPINT:%1").arg(pardsp[i] & 0xFFFF);

                    if (m_clientIDHash.contains(client2)) // es war ein client der über protobuf (clientid) angelegt wurde
                    {
                        ProtobufMessage::NetMessage protobufIntMessage;
                        ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();

                        intMessage->set_body(s.toStdString());
                        intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

                        QByteArray idba = m_clientIDHash[client2];
                        protobufIntMessage.set_clientid(idba.data(), idba.size() );
                        protobufIntMessage.set_messagenr(0); // interrupt

                        client2->m_pNetClient->sendMessage(protobufIntMessage);
                    }
                    else
                    {
                        QByteArray block;

                        QDataStream out(&block, QIODevice::WriteOnly);
                        out.setVersion(QDataStream::Qt_4_0);
                        out << (qint32)0;

                        out << s.toUtf8();
                        out.device()->seek(0);
                        out << (qint32)(block.size() - sizeof(qint32));

                        XiQNetPeer* pNetclient;
                        pNetclient = client2->m_pNetClient;

                        if (pNetclient == 0)
                            m_pSCPISocket->write(block);
                        else
                            pNetclient->getTcpSocket()->write(block);
                    }
                }
            }

        }

        client->DspVarWrite(s = QString("CTRLACK,%1;").arg(CmdDone)); // jetzt in jedem fall acknowledge
        delete ba;

    }

    else
    {
        cZDSP1Client *dummyClient = new cZDSP1Client(0, 0, this); // dummyClient einrichten
        dummyClient->DspVarWrite(s = QString("CTRLACK,%1;").arg(CmdDone)); // und rücksetzen
        delete dummyClient;
    }
    }
}


bool cZDSP1Server::BuildDSProgram(QString &errs)
{
    // die programmlisten aller aktiven clients zusammen bauen

    bool ok;
    ulong umo = dm32UserWorkSpace.StartAdr; // usermememory offset

    CmdMem.clear();
    CmdIntMem.clear();
    QDataStream mds1 ( &CmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite );
    mds1.setByteOrder(QDataStream::LittleEndian);
    QDataStream mds2 ( &CmdIntMem, QIODevice::Unbuffered | QIODevice::ReadWrite );
    mds2.setByteOrder(QDataStream::LittleEndian);
    cZDSP1Client* client;
    cDspCmd cmd;
    QString s;

    if (clientlist.count() > 0)
    {
        s =  QString( "DSPMEMOFFSET(%1)" ).arg(dm32DspWorkspace.StartAdr);
        client = clientlist.at(0);
        cmd = client->GenDspCmd(s, &ok, 0, 0);
        mds1 << cmd;

        for (int i = 0; i < clientlist.count(); i++)
        {
            client = clientlist.at(i);
            if (client->isActive())
            {
                s =  QString( "USERMEMOFFSET(%1)" ).arg(umo);
                cmd = client->GenDspCmd(s, &ok, 0, 0);
                mds1 << cmd;
                mds2 << cmd;

                if (!client->GenCmdLists(errs, umo, UserWorkSpaceGlobalSegmentAdr))
                    return false;

                umo += client->setStartAdr(umo, UserWorkSpaceGlobalSegmentAdr); // relokalisieren der daten im dsp

                QList<cDspCmd> cmdl = client->GetDspCmdList();
                for (int j = 0; j < cmdl.size(); j++ ) mds1 << cmdl[j]; // cycl. liste
                QList<cDspCmd> cmdl2 = client->GetDspIntCmdList();
                for ( int j = 0; j < cmdl2.size(); j++ ) mds2 << cmdl2[j]; // interrupt liste
            }
        }

        client = clientlist.at(0);
        s = QString( "DSPINTPOST()"); // wir triggern das senden der serialisierten interrupts
        cmd = client->GenDspCmd(s, &ok, 0, 0);
        mds1 << cmd;

    }

    client = new cZDSP1Client(0, 0, this); // dummyClient einrichten damit was jetzt kommt noch

    s =  QString( "INVALID()"); // funktioniert selbst wenn wenn wir keinen mehr haben
    cmd = client->GenDspCmd(s, &ok, 0, 0);
    mds1 << cmd; // kommando listen ende
    mds2 << cmd;

    delete client;

    return true;
}


bool cZDSP1Server::LoadDSProgram()
{
    // die programmlisten aller aktiven clients laden
    QString s,s2;

    ActivatedCmdList = (ActivatedCmdList + 1) & 1;
    if (ActivatedCmdList == 0)
    {
        s = QString("CMDLIST");
        s2=QString("INTCMDLIST");
    }
    else
    {
        s = QString("ALTCMDLIST");
        s2=QString("ALTINTCMDLIST");
    };

    cZDSP1Client* client = new cZDSP1Client(0, 0, this); // dummyClient einrichten zum laden der kette

    ulong offset = client->DspVarResolver.adr(s) ;
    if (DspDevSeek(DevFileDescriptor, offset) < 0 )  // startadr im treiber setzen
        return false;

    if (DspDevWrite(DevFileDescriptor, CmdMem.data(), CmdMem.size()) < 0)
        return false;

    offset = client->DspVarResolver.adr(s2) ;
    if (DspDevSeek(DevFileDescriptor, offset) < 0 )  // startsadr im treiber setzen
        return false;

    if (DspDevWrite( DevFileDescriptor, CmdIntMem.data(), CmdIntMem.size()) < 0)
        return false;

    QString ss;
    mCommand2Dsp(ss = QString("DSPCMDPAR,7,%1;").arg(ActivatedCmdList));
    // dem dsp die neue liste mitteilen

    delete client;

    return true;
}



QString cZDSP1Server::mUnloadCmdList(QChar *)
{
    QString error;
    cZDSP1Client* cl = GetClient(ActSock);
    cl->SetActive(false);
    BuildDSProgram(error); // wir bauen neu
    if (!LoadDSProgram()) // und laden
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}


QString cZDSP1Server::mLoadCmdList(QChar *)
{
    static int count = 0;
    cZDSP1Client* cl = GetClient(ActSock);
    QString errs;
    cl->SetActive(true);
    if (BuildDSProgram(errs))
    { // die cmdlisten und die variablen waren schlüssig
        if (!LoadDSProgram())
        {
            Answer = ERREXECString;
            cl->SetActive(false);
        }
        else
            Answer = ACKString;
    }
    else
    {
        cl->SetActive(false);
        Answer = QString("%1 %2").arg(ERRVALString).arg(errs); // das "fehlerhafte" kommando anhängen
    }

    count++;
    qDebug() << QString("LoadCmdList(%1)").arg(count);

    return Answer;
}


QString cZDSP1Server::mSetRavList(QChar *s)
{
    QString qs(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer  = cl->SetRavList(qs);

    return Answer;
}


QString cZDSP1Server::mGetRavList() {
       cZDSP1Client* cl = GetClient(ActSock);
       Answer = cl->GetRavList();

       return Answer;
}


QString cZDSP1Server::mSetCmdIntList(QChar *s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->SetCmdIntListDef(par);

    return Answer;
}


QString cZDSP1Server::mGetCmdIntList()
{
       cZDSP1Client* cl = GetClient(ActSock);
       Answer = cl->GetCmdIntListDef();

       return Answer;
}


QString cZDSP1Server::mSetCmdList(QChar *s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->SetCmdListDef(par);

    return Answer;
}


QString cZDSP1Server::mGetCmdList()
{
      cZDSP1Client* cl = GetClient(ActSock);
      Answer = cl->GetCmdListDef();

      return Answer;
}


QString cZDSP1Server::mMeasure(QChar *s)
{
    QString par(s); // holt den parameter aus dem kommando

    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->readActValues(par);

    return Answer;
}


bool cZDSP1Server::setDspType()
{
    int r;
    r = readMagicId();
    QString s;

    if ( r == MAGIC_ID21262 )
    {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21262;
        return m_sDspBootPath.contains(s = "zdsp21262.ldr");
        // adressen im dsp stehen für adsp21262 default richtig
    }
    else
    if ( r == MAGIC_ID21362)
    {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21362;
        if (m_sDspBootPath.contains(s = "zdsp21362.ldr"))
        {
            // für adsp21362 schreiben wir die adressen um
            dm32DspWorkspace.StartAdr = dm32DspWorkSpaceBase21362;
            dm32DialogWorkSpace.StartAdr = dm32DialogWorkSpaceBase21362;
            dm32UserWorkSpace.StartAdr = dm32UserWorkSpaceBase21362;
            dm32CmdList.StartAdr = dm32CmdListBase21362;

            sDspVar* pDspVar = &CmdListVar;

            pDspVar->size = IntCmdListLen21362; pDspVar++;
            pDspVar->size = CmdListLen21362; pDspVar++;
            pDspVar->size = IntCmdListLen21362; pDspVar++;
            pDspVar->size = CmdListLen21362;

            pDspVar = &UserWorkSpaceVar;
            pDspVar->size = uwSpaceSize21362;

            return true;
        }

        else
            return false;
    }

    else
        return false;
}


int cZDSP1Server::readMagicId()
{
    int r;
    r = ioctl(DevFileDescriptor,IO_READ,MagicId);
    if (DEBUG1) syslog(LOG_INFO,"zdsp1d readMagicId : 0x%x\n", r);

    return r;
}


bool cZDSP1Server::Test4HWPresent()
{
    int r;
    r = readMagicId();

    return ( (r == MAGIC_ID21262) || (r == MAGIC_ID21362));
}


bool cZDSP1Server::Test4DspRunning()
{
    int r;
    r = ioctl(DevFileDescriptor,IO_READ,DSPStat);

    return ((r & DSP_RUNNING) > 0);
}


QString cZDSP1Server::mDspMemoryRead(QChar* s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->DspVarListRead(par);

    return Answer;
}


QString cZDSP1Server::mDspMemoryWrite(QChar* s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->DspVarWriteRM(par);

    return Answer;
}


cZDSP1Client* cZDSP1Server::GetClient(int s)
{
    cZDSP1Client* client;
    if (clientlist.count() > 0)
    {
        for (int i = 0; i < clientlist.count(); i++)
        {
            client = clientlist.at(i);
            if (client->sock == s)
                return client;
        }
    }

    return NULL;
}


cZDSP1Client* cZDSP1Server::GetClient(XiQNetPeer *peer)
{
    cZDSP1Client* client;
    if (clientlist.count() > 0)
    {
        for (int i = 0; i < clientlist.count(); i++)
        {
            client = clientlist.at(i);
            if (client->m_pNetClient == peer)
                return client;
        }
    }

    return NULL;
}


void cZDSP1Server::establishNewConnection(XiQNetPeer *newClient)
{
    connect(newClient, &XiQNetPeer::sigMessageReceived, this, &cZDSP1Server::executeCommand);
    connect(newClient, SIGNAL(sigConnectionClosed()), this, SLOT(deleteConnection()));
    AddClient(newClient); // we additionally add the client to our list
}


void cZDSP1Server::deleteConnection()
{
    XiQNetPeer* client = qobject_cast<XiQNetPeer*>(sender());
    DelClient(client);
}


void cZDSP1Server::executeCommand(std::shared_ptr<google::protobuf::Message> cmd)
{
    QString m_sInput, m_sOutput;
    std::shared_ptr<ProtobufMessage::NetMessage> protobufCommand = nullptr;

    XiQNetPeer* client = qobject_cast<XiQNetPeer*>(sender());
    protobufCommand = std::static_pointer_cast<ProtobufMessage::NetMessage>(cmd);

    if ( (protobufCommand != 0) && (client != 0))
    {
        if (protobufCommand->has_netcommand() && protobufCommand->has_clientid())
        {
            // in case of "lost" clients we delete the clients and its data
            QByteArray clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            DelClient(clientId);
            //LoadDSProgram(); // after deleting client we reload dsp program
        }

        else

        if (protobufCommand->has_clientid() && protobufCommand->has_messagenr())
        {
            QByteArray clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            quint32 messageNr = protobufCommand->messagenr();
            ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();

            if (!m_zdspdClientHash.contains(clientId)) // we didn't get any command from here yet
            {
                cZDSP1Client *zdspclient = AddClient(client); // we add a new client with the same socket but different identifier
                m_zdspdClientHash[clientId] = zdspclient;
                m_clientIDHash[zdspclient] = clientId; // we need this list in case of interrupts
            }

            ActSock = m_zdspdClientHash[clientId]->getSocket(); // we set the actual socket (identifier) we have to work on
            m_sInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
            m_sOutput = pCmdInterpreter->CmdExecute(m_sInput);

            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();

            // dependent on rtype caller can se ack, nak, error
            // in case of error the body has to be analysed for details

            if (m_sOutput.contains(ACKString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else
            if (m_sOutput.contains(NACKString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else
            if (m_sOutput.contains(BUSYString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRCONString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRAUTString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRVALString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRXMLString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRMMEMString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRPATHString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERREXECString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(ERRTIMOString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

            QByteArray ba = m_sOutput.toUtf8();
            Answer->set_body(ba.data()); // in any case we set the body

            protobufAnswer.set_clientid(clientId, clientId.count());
            protobufAnswer.set_messagenr(messageNr);

            client->sendMessage(protobufAnswer);
        }
        else
        {
            ActSock = GetClient(client)->getSocket();

            m_sInput =  QString::fromStdString(protobufCommand->scpi().command());
            m_sOutput = pCmdInterpreter->CmdExecute(m_sInput);

            QByteArray block;

            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << m_sOutput.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            client->getTcpSocket()->write(block);
        }
    }
}


void cZDSP1Server::setSCPIConnection()
{
    m_pSCPISocket = m_pSCPIServer->nextPendingConnection();

    m_pSCPIClient = AddSCPIClient();

    connect(m_pSCPISocket, SIGNAL(readyRead()), this, SLOT(SCPIInput()));
    connect(m_pSCPISocket, SIGNAL(disconnected()), this, SLOT(SCPIdisconnect()));
}


void cZDSP1Server::SCPIInput()
{
    QString m_sInput, m_sOutput;

    m_sInput = "";
    while ( m_pSCPISocket->canReadLine() )
        m_sInput += m_pSCPISocket->readLine();

    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');

    m_sOutput = pCmdInterpreter->CmdExecute(m_sInput) + "\n";

    QByteArray ba;

    ba = m_sOutput.toLatin1();
    m_pSCPISocket->write(ba);

}


void cZDSP1Server::SCPIdisconnect()
{
    disconnect(m_pSCPISocket, 0, 0, 0); // we disconnect everything
    DelSCPIClient();
}



void cZDSP1Server::SetFASync()
{
    fcntl(DevFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(DevFileDescriptor, F_GETFL);
    fcntl(DevFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}


cZDSP1Client* cZDSP1Server::AddClient(XiQNetPeer* m_pNetClient)
{
    // fügt einen client hinzu
    // int socket = m_pNetClient->getSocket();
    m_nSocketIdentifier++;
    if (m_nSocketIdentifier == 0)
        m_nSocketIdentifier++;
    cZDSP1Client* client = new cZDSP1Client(m_nSocketIdentifier, m_pNetClient,this);
    clientlist.append(client);
    if DEBUG3 syslog(LOG_INFO,"client %d added\n", m_nSocketIdentifier);
    return client;
}


void cZDSP1Server::DelClient(XiQNetPeer* netClient)
{ // entfernt alle cZDSP1Clients die an diesem netClient kleben
    QList<cZDSP1Client*> todeleteList;

    for (int i = 0; i < clientlist.count(); i++)
    {
        XiQNetPeer* peer;
        cZDSP1Client* zdspclient;

        zdspclient = clientlist.at(i);
        peer = zdspclient->m_pNetClient;
        if (peer == netClient)
        {
            todeleteList.append(zdspclient);
            if (m_clientIDHash.contains(zdspclient))
            {
                QByteArray ba;
                ba = m_clientIDHash.take(zdspclient);
                m_zdspdClientHash.remove(ba);
            }

        }
    }

    for (int i = 0; i < todeleteList.count(); i++)
    {
        cZDSP1Client* client;
        client = todeleteList.at(i);
        if DEBUG3 syslog(LOG_INFO,"client %d deleted\n", client->getSocket());
        clientlist.removeOne(client);
        delete client;
    }
    LoadDSProgram(); // after deleting clients we reload dsp program
}


void cZDSP1Server::DelClient(QByteArray clientId)
{
    if (m_zdspdClientHash.contains(clientId))
    {
        cZDSP1Client *client = m_zdspdClientHash.take(clientId);
        m_clientIDHash.remove(client);
        clientlist.removeOne(client);
        delete client;
        LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
    }
}


cZDSP1Client *cZDSP1Server::AddSCPIClient()
{
    return AddClient(0); // we add this client with netclient (XiQNetPeer) = 0 because it is no XiQNetPeer but
}


void cZDSP1Server::DelSCPIClient()
{
    clientlist.removeAll(m_pSCPIClient);
    LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client

}


QString cZDSP1Server::SCPICmd(SCPICmdType cmd, QChar *s)
{
    switch ((int)cmd)
    {
    case    TestDsp:            return mTestDsp(s);
    case 	ResetDsp:           return mResetDsp(s);
    case	BootDsp:            return mBootDsp(s);
    case 	SetDspBootPath:     return mSetDspBootPath(s);
    case 	SetRavList: 		return mSetRavList(s);
    case 	SetCmdList: 		return mSetCmdList(s);
    case   SetCmdIntList: 		return mSetCmdIntList(s);
    case 	Measure:            return mMeasure(s);
    case 	UnloadCmdList: 		return mUnloadCmdList(s);
    case 	LoadCmdList: 		return mLoadCmdList(s);
    case 	DspMemoryRead: 		return mDspMemoryRead(s);
    case   DspMemoryWrite:		return mDspMemoryWrite(s);
    case   SetSamplingSystem:	return mSetSamplingSystem(s);
    case	SetCommEncryption:	return mSetCommEncryption(s);
    case   SetEN61850DestAdr:    	return mSetEN61850DestAdr(s);
    case   SetEN61850SourceAdr:    	return mSetEN61850SourceAdr(s);
    case 	SetEN61850EthTypeAppId:  return mSetEN61850EthTypeAppId(s);
    case 	SetEN61850PriorityTagged:  return mSetEN61850PriorityTagged(s);
    case   SetEN61850EthSync: 	return mSetEN61850EthSync(s);
    case   SetDspCommandStat:	return mSetDspCommandStat(s);
    case   SetEN61850DataCount:	return mSetEN61850DataCount(s);
    case   SetEN61850SyncLostCount:	return mSetEN61850SyncLostCount(s);
    case   TriggerIntListHKSK:	return mTriggerIntListHKSK(s);
    case   TriggerIntListALL:		return mTriggerIntListALL(s);
    case   ResetMaxima:		return mResetMaxima(s);
    case   ResetDeviceLoadMax:	return mResetDeviceLoadMax();
    }
    Answer = "ProgrammierFehler"; // hier sollten wir nie hinkommen

    return Answer;
}


QString cZDSP1Server::SCPIQuery( SCPICmdType cmd)
{
    switch ((int)cmd)
    {
    case 		GetDspBootPath: 		return mGetDspBootPath();
    case 		GetPCBSerialNumber: 	return mGetPCBSerialNumber();
    case 		GetDeviceVersion:		return mGetDeviceVersion();
    case 		GetServerVersion: 		return mGetServerVersion();
    case		GetDeviceLoadMax: 	return mGetDeviceLoadMax();
    case 		GetDeviceLoadAct: 	return mGetDeviceLoadAct();
    case		GetDspStatus:		return mGetDspStatus();
    case 		GetDeviceStatus: 		return mGetDeviceStatus();
    case 		GetRavList: 		return mGetRavList();
    case 		GetCmdIntList: 		return mGetCmdIntList();
    case 		GetCmdList: 		return mGetCmdList();
    case		GetSamplingSystem:	return mGetSamplingSystem();
    case        GetCommEncryption:	return mGetCommEncryption();
    case		GetEN61850DestAdr:	return mGetEN61850DestAdr();
    case		GetEN61850SourceAdr:	return mGetEN61850SourceAdr();
    case		GetEN61850EthTypeAppId:  return mGetEN61850EthTypeAppId();
    case		GetEN61850PriorityTagged:  return mGetEN61850PriorityTagged();
    case                 GetEN61850EthSync: 	return mGetEN61850EthSync();
    case		GetEN61850DataCount:	return mGetEN61850DataCount();
    case		GetEN61850SyncLostCount:	return mGetEN61850SyncLostCount();
    case 		GetDspCommandStat:	return mGetDspCommandStat();
    }
    Answer = "ProgrammierFehler"; // hier sollten wir nie hinkommen
    return Answer;
}


