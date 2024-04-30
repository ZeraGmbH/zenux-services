#ifndef MT310S2RANGESSTRUCT_H
#define MT310S2RANGESSTRUCT_H

#include <QMap>

struct rangeInfosStruct {
    bool availability;
    double rValue;
    double rejection;
    double overejection;
};

class Mt310s2RangesStruct
{
public:
    Mt310s2RangesStruct();
    QMap<QString , rangeInfosStruct> getVoltageRangeInfo();
    QMap<QString , rangeInfosStruct> getCurrentRangeInfo();

private:
    void insertVoltageRangesInfo();
    void insertCurrentRangesInfo();
    QMap<QString , rangeInfosStruct> m_voltageRangeInfoMap;
    QMap<QString , rangeInfosStruct> m_currentRangeInfoMap;
};

#endif // MT310S2RANGESSTRUCT_H
