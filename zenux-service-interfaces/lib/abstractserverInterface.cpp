#include "abstractserverInterface.h"
#include "proxy.h"

void AbstractServerInterface::setClientSuperSmart(const NetworkConnectionInfo &netInfo,
                                                  const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory) {
    setClientSmart(Zera::Proxy::getInstance()->getConnectionSmart(netInfo, tcpNetworkFactory));
}
