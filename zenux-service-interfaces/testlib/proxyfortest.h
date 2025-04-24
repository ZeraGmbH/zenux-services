#ifndef PROXYFORTEST_H
#define PROXYFORTEST_H

#include <proxy.h>

class ProxyForTest : public Zera::Proxy
{
public:
    // Zera::Proxy's ctor is protected
    explicit ProxyForTest();
};

#endif // PROXYFORTEST_H
