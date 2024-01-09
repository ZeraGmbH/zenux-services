#ifndef RANGEADJINTERFACE_H
#define RANGEADJINTERFACE_H

#include "scpiconnection.h"
#include "permissionfunctions.h"
#include "adjustmentstatusinterface.h"
#include "justdatainterface.h"
#include <QDataStream>
#include <functional>
#include <memory>

// AdjustScpiValueFormatter(Factory) will go once we found a common format tested
// on all adjustment environments
struct AdjustScpiValueFormatter
{
    std::function<QString(double)> m_gainScpiFormatter;
    std::function<QString(double)> m_phaseScpiFormatter;
    std::function<QString(double)> m_offsetScpiFormatter;
    int m_correctionExportDigits;
};

class AdjustScpiValueFormatterFactory
{
public:
    static std::unique_ptr<AdjustScpiValueFormatter> createMt310s2AdjFormatter();
    static std::unique_ptr<AdjustScpiValueFormatter> createCom5003AdjFormatter();
};

class RangeAdjInterface : public ScpiConnection
{
    Q_OBJECT
public:
    RangeAdjInterface(cSCPI* scpiinterface,
                             std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter,
                             PermissionStructAdj permissions = PermissionStructAdj());
    virtual void initSCPIConnection(QString leadingNodes) override;
    JustDataInterface* getAdjInterface(QString name);

    double getGainCorrectionSingle(double par);
    double getPhaseCorrectionSingle(double par);
    double getOffsetCorrectionSingle(double par);

    void Serialize(QDataStream&); // zum schreiben aller justagedaten in flashspeicher
    void Deserialize(QDataStream&); // zum lesen aller justagedaten aus flashspeicher
    quint8 getAdjustmentStatus80Mask();
    void initJustData();
    void computeJustData();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    virtual double getGainCorrectionTotal(double par);
    virtual double getPhaseCorrectionTotal(double par);
    virtual double getOffsetCorrectionTotal(double par);

private:
    QString scpiQueryGainCorrectionTotal(const QString &scpiInput);
    QString scpiQueryGainCorrectionSingle(QString &scpiInput);
    QString scpiQueryPhaseCorrectionTotal(QString &scpiInput);
    QString scpiQueryPhaseCorrectionSingle(QString &scpiInput);
    QString scpiQueryOffsetCorrectionTotal(QString &scpiInput);
    QString scpiQueryOffsetCorrectionSingle(QString &scpiInput);
    QString scpiQueryStatus(QString &scpiInput);
    QString scpiCmdComputeJustData(QString &scpiInput);
    QString scpiCmdInitJustData(QString &scpiInput); // done in Adjustmentmodule - left for compatibility

    JustDataInterface m_gainCorrection;
    JustDataInterface m_phaseCorrection;
    JustDataInterface m_offsetCorrection;
    std::unique_ptr<AdjustScpiValueFormatter> m_scpiQueryFormatter;
    PermissionStructAdj m_permissions;
};

#endif
