#ifndef TEST_EMOB_LOCK_BUTTON_H
#define TEST_EMOB_LOCK_BUTTON_H

#include "mt310s2systeminterface.h"
#include <scpi.h>
#include <QObject>

class test_emob_lock_button : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void findSCPIObjects();
    void pushButton();
    void readLockState();
    void pressButtonUl1Channel();
    void readLockStateChannelUl1();

private:
    std::shared_ptr<cSCPI> m_scpiInterface;
    std::unique_ptr<Mt310s2SystemInterface> m_systemInterface;
};

#endif // TEST_EMOB_LOCK_BUTTON_H
