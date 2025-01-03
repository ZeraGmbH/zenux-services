#include "zdspclient.h"
#include "zscpi_response_definitions.h"
#include <parse.h>
#include <QDataStream>

extern TMemSection dm32DspWorkspace;
extern TMemSection dm32DialogWorkSpace;
extern TMemSection dm32UserWorkSpace;
extern TMemSection dm32CmdList;
extern TMemSection symbConsts1;

cZDSP1Client::cZDSP1Client(int socket, VeinTcp::TcpPeer* netclient, AbstractFactoryDeviceNodeDspPtr deviceNodeFactory) :
    m_deviceNodeFactory(deviceNodeFactory)
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

    m_bActive = false;
}

bool cZDSP1Client::setRawActualValueList(const QString &varsSemicolonSeparated)
{
    m_dspRawActualValueVarList.clear();
    DspVarClientPerspective dspVar;
    int localOffset = 0;
    int globaloffset = 0;
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    for(int i=0; i<varEntries.count(); i++) {
        if(dspVar.Init(varEntries[i])) {
            if (dspVar.segment() == localSegment) {
                dspVar.SetOffs(localOffset);
                localOffset += dspVar.size();
            }
            else {
                dspVar.SetOffs(globaloffset);
                globaloffset += dspVar.size();
            }
            m_dspRawActualValueVarList.append(dspVar);
        }
        else {
            m_dspRawActualValueVarList.clear();
            return false;
        }
    }

    m_memorySection.n = m_dspRawActualValueVarList.count();
    if(m_memorySection.n > 0) { // wir haben mindestens 1 variable
        m_dspVarArray.resize(m_memorySection.n);
        for(int i=0; i<m_memorySection.n; i++) { // und machen diese dem resolver zugänglich
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
    return true;
}

QString cZDSP1Client::getRawActualValueList()
{
    QString ret;
    QTextStream ts(&ret, QIODevice::WriteOnly);
    if (!m_dspRawActualValueVarList.empty()) {
        QList<DspVarClientPerspective>::iterator it;
        for(it = m_dspRawActualValueVarList.begin(); it != m_dspRawActualValueVarList.end(); ++it)
            ts << (*it).name() << ',' << (*it).size() << ';';
    }
    else
        ts << "Empty";
    return ret;
}

void cZDSP1Client::setCmdListDef(const QString &cmdListDef)
{
    m_sCmdListDef = cmdListDef;
}

QString cZDSP1Client::getCmdListDef()
{
    return m_sCmdListDef;
}

void cZDSP1Client::setCmdForIrqListDef(const QString &cmdIntListDef)
{
    m_sIntCmdListDef = cmdIntListDef;
}

QString cZDSP1Client::getCmdForIrqListDef()
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
            t &= ( (par = m_dspVarResolver.varOffset(sSearch, userMemoryOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
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
                t &= ( (par[i] = m_dspVarResolver.varOffset(sSearch, userMemoryOffset, globalstartadr)) > -1);
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
                t &= ( (par[i] = m_dspVarResolver.varOffset(sSearch, userMemoryOffset, globalstartadr)) > -1);
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
            bool t = ( (par = m_dspVarResolver.varOffset(sSearch, userMemoryOffset, globalstartadr)) > -1);
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
            *ok = ( (par1 = m_dspVarResolver.varOffset(sSearch, userMemoryOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
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

void cZDSP1Client::setActive(bool active)
{
    m_bActive = active;
}

ulong cZDSP1Client::setStartAdr(ulong startAdress, ulong globalMemStart)
{
    ulong usermemsize = 0;
    ulong globalmemsize = 0;
    m_memorySection.StartAdr = startAdress;
    for (int i = 0; i < m_memorySection.n; i++) {
        if (m_memorySection.DspVar[i].segment == localSegment) {
            m_memorySection.DspVar[i].adr = startAdress + usermemsize; // we need the adress for reading back data
            usermemsize += m_memorySection.DspVar[i].size;
        }
        else {
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
    for (int i = 0;;i++) {
        QString cs = s.section(';',i,i);
        if ( (cs.isEmpty()) || (cs==("Empty")) )
            break; // liste ist durch
        cl.append(GenDspCmd(cs, &ok, umo,globalstartadr));
        if (!ok) {
            errs = cs;
            break;
        }
    }
    return ok;
}

bool cZDSP1Client::GenCmdLists(QString& errs, ulong userMemoryOffset, ulong globalstartadr)
{
    bool ok = GenCmdList(m_sCmdListDef,m_DspCmdList,errs,userMemoryOffset,globalstartadr);
    if(ok)
        ok = GenCmdList(m_sIntCmdListDef, m_DspIntCmdList,errs,userMemoryOffset,globalstartadr);
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

QString cZDSP1Client::readActValues(const QString& variablesStringOnEmptyActOnly)
{
    QString variablesStringWithActual(variablesStringOnEmptyActOnly);
    if(variablesStringWithActual.isEmpty()) { // sonderfall liste leer -> alle messwerte lesen
        for (int i = 0; i < m_dspRawActualValueVarList.count(); i++) {
            DspVarClientPerspective Var = m_dspRawActualValueVarList.at(i);
            variablesStringWithActual += QString("%1,%2;").arg(Var.name()).arg(Var.size());
        }
    }
    return readDspVarList(variablesStringWithActual);
}

bool cZDSP1Client::readOneDspVarInt(const QString &varName, int& intval)
{ // einen int (32bit) wert lesen
    bool ret = false;
    QByteArray ba;
    QString ss = QString("%1,1").arg(varName);
    if(readOneDspVar(ss, &ba) != 0) {
        // 1 wort ab name (s) lesen
        intval = *((int*) (ba.data()));
        ret = true;
    }
    return ret;
}

TDspVar *cZDSP1Client::doReadVarFromDsp(TDspVar *&DspVar, int countVars, QByteArray *varRead)
{
    const int countBytes = countVars * 4;
    varRead->resize(countBytes);
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    if ((deviceNode->lseek(DspVar->adr) >= 0) &&
        (deviceNode->read(varRead->data(), countBytes) >= 0))
        return DspVar; // dev.  seek und dev. read ok
    return nullptr; // sonst fehler
}

TDspVar *cZDSP1Client::readOneDspVar(const QString &nameCommaLen, QByteArray *varRead)
{
    const QStringList listNameLen = nameCommaLen.split(",", Qt::SkipEmptyParts);
    if(listNameLen.count() < 2)
        return nullptr; // wrong parameter format
    QString name = listNameLen[0];
    TDspVar *DspVar = m_dspVarResolver.getDspVar(name);
    if (!DspVar)
        return nullptr; // fehler, den namen gibt es nicht

    QString countSection = listNameLen[1];
    bool ok;
    const int countVars = countSection.toInt(&ok);
    if (!ok || countVars < 1 )
        return nullptr; // fehler in der anzahl der elemente

    return doReadVarFromDsp(DspVar, countVars, varRead);
}

QString cZDSP1Client::readDspVarList(const QString& variablesString)
{
    QString ret;
    QTextStream ts(&ret, QIODevice::WriteOnly);
    ts.setRealNumberPrecision(8);
    QByteArray ba;
    const QStringList varEntries = variablesString.split(";", Qt::SkipEmptyParts);
    for(int i=0; i<varEntries.count(); i++) {
        QString nameCommaLen = varEntries[i]; // format '<name>,<count>'
        TDspVar *dspVar = readOneDspVar(nameCommaLen, &ba);
        if(!dspVar)
            return ZSCPI::scpiAnswer[ZSCPI::errexec];

        int n = ba.size() / 4;
        ts << dspVar->Name << ":";
        switch(dspVar->type)
        {
        case eInt :
        {
            ulong *ul = (ulong*) ba.data();
            for (int j = 0; j < n-1; j++,ul++)
                ts << (*ul) << "," ;
            ts << *ul << ";" ;
            break;
        }
        case eUnknown:
        case eFloat :
        {
            float *f = (float*) ba.data();
            for (int j = 0; j < n-1; j++,f++)
                ts << (*f) << "," ;
            ts << *f << ";" ;
            break;}
        }
    }
    return ret;
}

static bool tryStreamLongValue(const QString &strValue, QDataStream &stream)
{
    bool ok;
    qint32 value = strValue.toLong(&ok);
    if(ok)
        stream << value;
    return ok;
}

static bool tryStreamUlongValue(const QString &strValue, QDataStream &stream)
{
    bool ok;
    quint32 value = strValue.toULong(&ok);
    if(ok)
        stream << value;
    return ok;
}

static bool tryStreamFloatValue(const QString &strValue, QDataStream &stream)
{
    bool ok;
    float value = strValue.toFloat(&ok);
    if(ok)
        stream << value;
    return ok;
}

bool cZDSP1Client::doWriteDspVars(const QString &varsSemicolonSeparated)
{
    const int gran = 10; // immer 10 elemente allokieren
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    for(int i=0; i<varEntries.count(); i++) {
        QString varString = varEntries[i];
        const QStringList varNameVals = varString.split(",", Qt::SkipEmptyParts);
        if(varNameVals.count() < 2)
            return false;
        QString name = varNameVals[0];
        long adr = m_dspVarResolver.varAddress(name);
        if (adr == -1)
            return false;

        QByteArray ba;
        QDataStream bas(&ba, QIODevice::Unbuffered | QIODevice::ReadWrite);
        bas.setByteOrder(QDataStream::LittleEndian);
        bas.setFloatingPointPrecision(QDataStream::SinglePrecision);

        int n = 0, alloc = 0;
        int type = m_dspVarResolver.type(name);
        for(int valNo=1; valNo<varNameVals.count(); valNo++) {
            QString p = varNameVals[valNo];
            if(++n > alloc) {
                alloc += gran;
                ba.resize(alloc*4);
            }
            if(type == eUnknown) {
                if(!tryStreamLongValue(p, bas))
                    if(!tryStreamUlongValue(p, bas))
                        if(!tryStreamFloatValue(p, bas))
                            return false;
            }
            else if(type == eInt) {
                if(!tryStreamLongValue(p, bas))
                    if(!tryStreamUlongValue(p, bas))
                        return false;
            }
            else {
                if(!tryStreamFloatValue(p, bas))
                    return false;
            }
        }
        AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
        if (n > 0 && !deviceNode->write(adr, ba.data(), n*4 ))
            return false;
    }
    return true;
}

