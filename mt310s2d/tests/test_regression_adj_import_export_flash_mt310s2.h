#ifndef TEST_REGRESSION_ADJ_IMPORT_EXPORT_FLASH_MT310S2_H
#define TEST_REGRESSION_ADJ_IMPORT_EXPORT_FLASH_MT310S2_H

#include "mockforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_flash_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void directExportFlashNoMock();
    void directExportFlash();
    void scpiWriteFlashInitial();

    // modify -> export
    // import -> check changes in XML
    // all tests for clamps / com5003
private:
    void setupServers(AtmelPermissionTemplate *permissionQueryHandler);
    bool writeFile(QString filename, QByteArray data);
    QByteArray readFile(QString filename);

    std::unique_ptr<MockForSenseInterfaceMt310s2> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    AtmelPermissionTemplatePtrU m_permissionMock;
};

#endif // TEST_REGRESSION_ADJ_IMPORT_EXPORT_FLASH_MT310S2_H
