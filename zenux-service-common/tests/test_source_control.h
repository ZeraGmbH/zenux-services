#ifndef TEST_SOURCE_CONTROL_H
#define TEST_SOURCE_CONTROL_H

#include "mockmt310s2d.h"
#include "tinyzscpicmdinterface.h"
#include <resmanrunfacade.h>

class test_source_control : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void mt310s2Capabilities();
    void mt581s2Capabilities();

    void mt310s2InitialState();
    void mt581s2InitialState();

    void mt310s2InitialLoad();
    void mt581s2InitialLoad();

    void mt310s2RejectValidCapabilitesSet();
    void mt581s2RejectValidCapabilitesSet();

    void mt310s2RejectValidStateSet();
    void mt581s2RejectValidStateSet();

    void mt310s2RejectInValidLoadSet();
    void mt581s2RejectInValidLoadSet();

    void mt581s2AcceptValidLoadSetNoChange();
private:
    void setupServerAndClient(const QString &deviceD);
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mtXXXs2d;
    std::unique_ptr<TinyZScpiCmdInterface> m_pcbInterface;

    quint8 m_lastReply;
    QVariant m_lastAnswer;
};

#endif // TEST_SOURCE_CONTROL_H
