#ifndef TEST_SOURCE_CONTROL_H
#define TEST_SOURCE_CONTROL_H

#include "mockmt310s2d.h"
#include "tinyserverscpicmdinterface.h"
#include <resmanrunfacade.h>

class test_source_control : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void mt310s2Capabilities();
    void mt581s2Capabilities();
private:
    void setupServerAndClient(const QString &deviceD);
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mtXXXs2d;
    std::unique_ptr<TinyServerScpiCmdInterface> m_pcbInterface;

    quint8 m_lastReply;
    QVariant m_lastAnswer;
};

#endif // TEST_SOURCE_CONTROL_H
