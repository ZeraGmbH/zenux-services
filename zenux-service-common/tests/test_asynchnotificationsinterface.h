#ifndef TEST_ASYNCHNOTIFICATIONSINTERFACE_H
#define TEST_ASYNCHNOTIFICATIONSINTERFACE_H

#include <scpi.h>
#include <memory>

class test_asynchnotificationsinterface : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void findScpiRegisterObject();
    void findScpiUnregisterObject();

    void expectTwoDelegates();
    void removeScpiConnection();
private:
    std::unique_ptr<cSCPI> m_scpiInterface;
};

#endif // TEST_ASYNCHNOTIFICATIONSINTERFACE_H
