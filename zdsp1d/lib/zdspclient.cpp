#include "zdspclient.h"
#include "dspcmdcompiler.h"
#include "dspvardevicenodeinout.h"
#include "zscpi_response_definitions.h"

cZDSP1Client::cZDSP1Client(int socket, VeinTcp::TcpPeer* netclient, AbstractFactoryDeviceNodeDspPtr deviceNodeFactory) :
    m_deviceNodeFactory(deviceNodeFactory),
    m_pNetClient(netclient),
    m_socket(socket),
    m_sCmdListDef("Empty"),
    m_sIntCmdListDef("Empty")
{
    DspCmdWithParamsRaw DspCmd;
    m_DspCmdList.append(DspCmd);
    m_DspIntCmdList.append(DspCmd);
    m_dspVarResolver.addSection(&m_userMemSection);
    m_userMemSection.StartAdr = 0;
    m_userMemSection.n = 0;
    m_userMemSection.Section = userSection;
    m_dspVarResolver.actualizeVarHash(); // wir setzen die hashtabelle und initialisieren diese
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

    m_userMemSection.n = m_dspRawActualValueList.count();
    if(m_userMemSection.n > 0) { // wir haben mindestens 1 variable
        m_dspVarArray.resize(m_userMemSection.n);
        for(int i=0; i<m_userMemSection.n; i++) { // und machen diese dem resolver zugänglich
            m_dspVarArray[i].m_clientHandleName = m_dspRawActualValueList[i].getClientHandleName();
            m_dspVarArray[i].Name = m_dspRawActualValueList[i].name();
            m_dspVarArray[i].size = m_dspRawActualValueList[i].size();
            m_dspVarArray[i].offs = m_dspRawActualValueList[i].offs();
            m_dspVarArray[i].type = (dType)m_dspRawActualValueList[i].type();
            m_dspVarArray[i].segment = (segmentType)m_dspRawActualValueList[i].segment();
        }
        // WTF!!!!!!!!!!!!!
        m_userMemSection.DspVar = m_dspVarArray.data();
    }
    m_dspVarResolver.actualizeVarHash(); // wir setzen die hashtabelle neu
    return true;
}

void cZDSP1Client::setCmdListDef(const QString &cmdListDef)
{
    m_sCmdListDef = cmdListDef;
}

void cZDSP1Client::setCmdForIrqListDef(const QString &cmdIntListDef)
{
    m_sIntCmdListDef = cmdIntListDef;
}

void cZDSP1Client::setActive(bool active)
{
    m_bActive = active;
}

ulong cZDSP1Client::setStartAdr(ulong startAdress, ulong globalMemStart)
{
    ulong usermemsize = 0;
    ulong globalmemsize = 0;
    m_userMemSection.StartAdr = startAdress;
    for (int i = 0; i < m_userMemSection.n; i++) {
        if (m_userMemSection.DspVar[i].segment == localSegment) {
            m_userMemSection.DspVar[i].adr = startAdress + usermemsize; // we need the adress for reading back data
            usermemsize += m_userMemSection.DspVar[i].size;
        }
        else {
            m_userMemSection.DspVar[i].adr = globalMemStart+globalmemsize;
            globalmemsize += m_userMemSection.DspVar[i].size;
        }
    }
    return usermemsize;
}

bool cZDSP1Client::GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr)
{
    DspCmdCompiler compiler(&m_dspVarResolver, m_socket);
    return
        compiler.compileCmds(m_sCmdListDef, m_DspCmdList,errs, userMemOffset, globalstartadr) &&
        compiler.compileCmds(m_sIntCmdListDef, m_DspIntCmdList, errs, userMemOffset, globalstartadr);
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
    if(readOneDspVar(ss, &ba)) {
        // 1 wort ab name (s) lesen
        intval = *((int*) (ba.data()));
        ret = true;
    }
    return ret;
}

TDspVar *cZDSP1Client::readOneDspVar(const QString &nameCommaLen, QByteArray *varRead)
{
    const QStringList listNameLen = nameCommaLen.split(",", Qt::SkipEmptyParts);
    if(listNameLen.count() < 2)
        return nullptr; // wrong parameter format
    QString name = listNameLen[0];
    TDspVar *dspVar = m_dspVarResolver.getDspVar(name);
    if (!dspVar)
        return nullptr; // fehler, den namen gibt es nicht

    QString countSection = listNameLen[1];
    bool ok;
    const int countVars = countSection.toInt(&ok);
    if (!ok || countVars < 1 )
        return nullptr; // fehler in der anzahl der elemente

    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
    if(dspInOut.doReadVarFromDsp(dspVar, countVars, varRead))
        return dspVar;
    return nullptr;
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
