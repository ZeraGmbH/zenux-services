#ifndef TEST_MOCKSERVICE_SEC1000D_H
#define TEST_MOCKSERVICE_SEC1000D_H

#include "mocksec1000d.h"
#include "resmanrunfacade.h"
#include "secinterface.h"
#include <QObject>
#include <memory>

class test_mockservice_sec1000d : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void getChannelCatSec1000d();
    void connectClientThenRemoveIt();
private:
    void createSecClientInterface();
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockSec1000d> m_sec1000d;
    std::unique_ptr<Zera::cSECInterface> m_secIFace;
};

#endif // TEST_MOCKSERVICE_SEC1000D_H
