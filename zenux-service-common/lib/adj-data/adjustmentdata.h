#ifndef ADJUSTMENTDATA_H
#define ADJUSTMENTDATA_H

#include "adjustmendataheader.h"
#include "adjustmentrangedata.h"
#include <QMap>
#include <memory>

class AdjustmentData
{
public:
    AdjustmentDataHeader& getAdjHeader();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    std::shared_ptr<AdjustmentRangeData> getRangeAdjData(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, std::shared_ptr<AdjustmentRangeData> adjData);
    QMap<QString, QStringList> getRangeInfos();

private:
    AdjustmentDataHeader m_adjHeader;



    QMap<QString, QMap<QString, std::shared_ptr<AdjustmentRangeData>>> m_rangeInfosMap;
};

#endif // ADJUSTMENTDATA_H
