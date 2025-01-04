#include "zdspclient.h"
#include "dspcmdcompiler.h"
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
    DspCmdWithParamsRaw DspCmd;
    m_DspCmdList.append(DspCmd);
    m_DspIntCmdList.append(DspCmd);
    m_dspVarResolver.addSection( &dm32DspWorkspace);
    m_dspVarResolver.addSection( &dm32DialogWorkSpace);
    m_dspVarResolver.addSection( &dm32UserWorkSpace);
    m_dspVarResolver.addSection( &dm32CmdList);
    m_dspVarResolver.addSection( &symbConsts1);
    m_dspVarResolver.addSection( &m_memorySection);
    m_memorySection.StartAdr = m_memorySection.n = 0; m_memorySection.Section = userSection;
    m_dspVarResolver.actualizeVarHash(); // wir setzen die hashtabelle und initialisieren diese

    m_bActive = false;
}

bool cZDSP1Client::setRawActualValueList(const QString &varsSemicolonSeparated)
{
    m_dspRawActualValueList.clear();
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
            m_dspRawActualValueList.append(dspVar);
        }
        else {
            m_dspRawActualValueList.clear();
            return false;
        }
    }

    m_memorySection.n = m_dspRawActualValueList.count();
    if(m_memorySection.n > 0) { // wir haben mindestens 1 variable
        m_dspVarArray.resize(m_memorySection.n);
        for(int i=0; i<m_memorySection.n; i++) { // und machen diese dem resolver zugänglich
            m_dspVarArray[i].m_clientHandleName = m_dspRawActualValueList[i].getClientHandleName();
            m_dspVarArray[i].Name = m_dspRawActualValueList[i].name();
            m_dspVarArray[i].size = m_dspRawActualValueList[i].size();
            m_dspVarArray[i].offs = m_dspRawActualValueList[i].offs();
            m_dspVarArray[i].type = (dType)m_dspRawActualValueList[i].type();
            m_dspVarArray[i].segment = (segmentType)m_dspRawActualValueList[i].segment();
        }
        m_memorySection.DspVar = m_dspVarArray.data();
    }
    m_dspVarResolver.actualizeVarHash(); // wir setzen die hashtabelle neu
    return true;
}

QString cZDSP1Client::getRawActualValueList()
{
    QString ret;
    QTextStream ts(&ret, QIODevice::WriteOnly);
    if (!m_dspRawActualValueList.empty())
        for(auto it = m_dspRawActualValueList.constBegin(); it != m_dspRawActualValueList.constEnd(); ++it)
            ts << it->name() << ',' << it->size() << ';';
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

bool cZDSP1Client::GenCmdList(const QString &cmdsSemicolonSeparated,
                              QList<DspCmdWithParamsRaw> &genCmdList,
                              QString& err,
                              ulong userMemOffset,
                              ulong globalstartadr)
{
    bool ok = true;
    genCmdList.clear();
    for(int i = 0;;i++) {
        QString cs = cmdsSemicolonSeparated.section(';',i,i);
        if ( (cs.isEmpty()) || (cs==("Empty")) )
            break; // liste ist durch
        genCmdList.append(DspCmdCompiler::GenDspCmd(cs,
                                                    &ok,
                                                    &m_dspVarResolver,
                                                    m_socket,
                                                    userMemOffset,
                                                    globalstartadr));
        if(!ok) {
            err = cs;
            break;
        }
    }
    return ok;
}

bool cZDSP1Client::GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr)
{
    return
        GenCmdList(m_sCmdListDef, m_DspCmdList,errs, userMemOffset, globalstartadr) &&
        GenCmdList(m_sIntCmdListDef, m_DspIntCmdList, errs, userMemOffset, globalstartadr);
}

bool cZDSP1Client::isActive()
{
    return m_bActive;
}

QList<DspCmdWithParamsRaw> &cZDSP1Client::GetDspCmdList()
{
    return m_DspCmdList;
}

QList<DspCmdWithParamsRaw> &cZDSP1Client::GetDspIntCmdList()
{
    return m_DspIntCmdList;
}

int cZDSP1Client::getSocket() const
{
    return m_socket;
}

QString cZDSP1Client::readActValues(const QString& variablesStringOnEmptyActOnly)
{
    QString variablesStringWithActual(variablesStringOnEmptyActOnly);
    if(variablesStringWithActual.isEmpty()) { // sonderfall liste leer -> alle messwerte lesen
        for (int i = 0; i < m_dspRawActualValueList.count(); i++) {
            DspVarClientPerspective Var = m_dspRawActualValueList.at(i);
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

static bool tryStreamIntegerValue(const QString &strValue, QDataStream &stream)
{
    bool ok;
    qint32 lValue = strValue.toLong(&ok);
    if(ok) {
        stream << lValue;
        return true;
    }
    quint32 uValue = strValue.toULong(&ok);
    if(ok) {
        stream << uValue;
        return true;
    }
    return false;
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
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    for(const QString &varString : varEntries) {
        QByteArray byteArr;
        QDataStream stream(&byteArr, QIODevice::Unbuffered | QIODevice::ReadWrite);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

        const QStringList varNameVals = varString.split(",", Qt::SkipEmptyParts);
        if(varNameVals.count() < 2)
            return false;
        const QString &varName = varNameVals[0];

        int wordCount = 0;
        int type = m_dspVarResolver.getVarType(varName);
        for(int valIdx=1; valIdx<varNameVals.count(); valIdx++) {
            const QString &valueStr = varNameVals[valIdx];
            if(type == eUnknown) {
                if(!tryStreamIntegerValue(valueStr, stream) &&
                   !tryStreamFloatValue(valueStr, stream))
                    return false;
            }
            else if(type == eInt) {
                if(!tryStreamIntegerValue(valueStr, stream))
                    return false;
            }
            else
                if(!tryStreamFloatValue(valueStr, stream))
                    return false;
            wordCount++;
        }
        long adr = m_dspVarResolver.getVarAddress(varName);
        if (adr == -1)
            return false;
        AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
        if(wordCount>0 && !deviceNode->write(adr, byteArr.data(), wordCount*4))
            return false;
    }
    return true;
}
