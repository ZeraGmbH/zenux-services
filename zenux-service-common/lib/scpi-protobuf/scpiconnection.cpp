#include <scpi.h>
#include "scpiconnection.h"
#include "scpidelegate.h"

ScpiConnection::ScpiConnection(cSCPI *scpiInterface)
    : m_pSCPIInterface(scpiInterface)
{
}

ScpiConnection::~ScpiConnection()
{
    removeSCPIConnections();
    for (int i = 0; i < m_DelegateList.count(); i++) {
        cSCPIDelegate* ptr = m_DelegateList.at(i);
        delete ptr;
    }
}

void ScpiConnection::removeSCPIConnections()
{
    if(m_pSCPIInterface) {
        for (int i = 0; i < m_DelegateList.count(); i++) {
            cSCPIDelegate* ptr = m_DelegateList.at(i);
            m_pSCPIInterface->delSCPICmds(ptr->getCommand());
        }
    }
}

void ScpiConnection::executeCommand(int, QString&, QString&)
{
}
