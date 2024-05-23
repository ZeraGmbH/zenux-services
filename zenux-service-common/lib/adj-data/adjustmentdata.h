#ifndef ADJUSTMENTDATA_H
#define ADJUSTMENTDATA_H

#include "adjustmendataheader.h"
#include "adjustmentrangedata.h"
#include <memory>


class AdjustmentData
{
public:
    AdjustmentData();
public:
    AdjustmentDataHeader& getAdjHeader();
    std::shared_ptr<AdjustmentRangeData> getRangeAdjData(QString channelName, QString rangeName);
    void addChannelRange(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, std::shared_ptr<AdjustmentRangeData> adjData);

private:
    AdjustmentDataHeader m_adjHeader;
};

#endif // ADJUSTMENTDATA_H
