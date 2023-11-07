#ifndef TEST_SERVICEMOCK_H
#define TEST_SERVICEMOCK_H

#include "resmanrunfacade.h"
#include <QObject>
#include <memory>

class test_servicemock : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void runMockMt310s2d();
private:
    void feedEventLoop();
    std::shared_ptr<ResmanRunFacade> m_resMan;
};

#endif // TEST_SERVICEMOCK_H
