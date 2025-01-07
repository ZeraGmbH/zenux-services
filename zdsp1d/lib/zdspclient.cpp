#include "zdspclient.h"
#include "dspcmdcompiler.h"
#include "dspvardevicenodeinout.h"

cZDSP1Client::cZDSP1Client(int socket, VeinTcp::TcpPeer* netclient, AbstractFactoryDeviceNodeDspPtr deviceNodeFactory) :
    m_pNetClient(netclient),
    m_deviceNodeFactory(deviceNodeFactory),
    m_socket(socket),
    m_sCmdListDef("Empty"),
    m_sIntCmdListDef("Empty")
{
    DspCmdWithParamsRaw DspCmd;
    m_DspCmdList.append(DspCmd);
    m_DspIntCmdList.append(DspCmd);
    m_dspVarResolver.addSection(&m_userMemSection);
}

bool cZDSP1Client::setRawActualValueList(const QString &varsSemicolonSeparated)
{
    m_dspVarArray.clear();
    int localOffset = 0;
    int globaloffset = 0;
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    for(int i=0; i<varEntries.count(); i++) {
        TDspVar dspVar;
        if(dspVar.Init(varEntries[i])) {
            if (dspVar.segment == localSegment) {
                dspVar.offs = localOffset;
                localOffset += dspVar.size;
            }
            else {
                dspVar.offs = globaloffset;
                globaloffset += dspVar.size;
            }
            m_dspVarArray.append(dspVar);
        }
        else {
            m_dspVarArray.clear();
            return false;
        }
    }

    m_userMemSection.m_varCount = m_dspVarArray.count();
    if(m_userMemSection.m_varCount > 0) { // wir haben mindestens 1 variable
        // WTF!!!!!!!!!!!!!
        m_userMemSection.m_dspVars = m_dspVarArray.data();
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

bool cZDSP1Client::hasDspCmds() const
{
    return !m_sCmdListDef.isEmpty() || !m_sIntCmdListDef.isEmpty();
}

ulong cZDSP1Client::setStartAdr(ulong startAdress, ulong globalMemStart)
{
    ulong usermemsize = 0;
    ulong globalmemsize = 0;
    m_userMemSection.m_startAddress = startAdress;
    for (int i = 0; i < m_userMemSection.m_varCount; i++) {
        if (m_userMemSection.m_dspVars[i].segment == localSegment) {
            m_userMemSection.m_dspVars[i].adr = startAdress + usermemsize; // we need the adress for reading back data
            usermemsize += m_userMemSection.m_dspVars[i].size;
        }
        else {
            m_userMemSection.m_dspVars[i].adr = globalMemStart+globalmemsize;
            globalmemsize += m_userMemSection.m_dspVars[i].size;
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

bool cZDSP1Client::isActive() const
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
    QString variablesStringWithActual = variablesStringOnEmptyActOnly;
    if(variablesStringWithActual.isEmpty()) { // sonderfall liste leer -> alle messwerte lesen
        for(const TDspVar &dspVar : qAsConst(m_dspVarArray))
            variablesStringWithActual += QString("%1,%2;").arg(dspVar.Name).arg(dspVar.size);
    }
    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
    return dspInOut.readDspVarList(variablesStringWithActual, &m_dspVarResolver);
}
