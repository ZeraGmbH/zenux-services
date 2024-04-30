#include "mt310s2rangesstruct.h"

Mt310s2RangesStruct::Mt310s2RangesStruct()
{
    insertVoltageRangesInfo();
    insertCurrentRangesInfo();
}

void Mt310s2RangesStruct::insertVoltageRangesInfo()
{
    rangeInfosStruct rangeInfos;

    rangeInfos.availability = true;
    rangeInfos.rValue = 250.0;
    rangeInfos.rejection = 4415057.0;
    rangeInfos.overejection = 5518821.0;
    m_voltageRangeInfoMap.insert("250V", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 8.0;
    rangeInfos.rejection = 3355443.0;
    rangeInfos.overejection = 4194304.0;
    m_voltageRangeInfoMap.insert("8V", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 0.1;
    rangeInfos.rejection = 4026532.0;
    rangeInfos.overejection = 5033165.0;
    m_voltageRangeInfoMap.insert("100mV", rangeInfos);
}

void Mt310s2RangesStruct::insertCurrentRangesInfo()
{
    rangeInfosStruct rangeInfos;

    rangeInfos.availability = true;
    rangeInfos.rValue = 10.0;
    rangeInfos.rejection = 3197613.0;
    rangeInfos.overejection = 3997016.0;
    m_currentRangeInfoMap.insert("10A", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 5.0;
    rangeInfos.rejection = 3197613.0;
    rangeInfos.overejection = 3997016.0;
    m_currentRangeInfoMap.insert("5A", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 2.5;
    rangeInfos.rejection = 3997016.0;
    rangeInfos.overejection = 4996270.0;
    m_currentRangeInfoMap.insert("2.5A", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 1.0;
    rangeInfos.rejection = 4177527.0;
    rangeInfos.overejection = 5221909.0;
    m_currentRangeInfoMap.insert("1.0A", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 0.5;
    rangeInfos.rejection = 4177527.0;
    rangeInfos.overejection = 5221909.0;
    m_currentRangeInfoMap.insert("500mA", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 0.25;
    rangeInfos.rejection = 4177527.0;
    rangeInfos.overejection = 5221909.0;
    m_currentRangeInfoMap.insert("250mA", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 0.1;
    rangeInfos.rejection = 4177527.0;
    rangeInfos.overejection = 5221909.0;
    m_currentRangeInfoMap.insert("100mA", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 0.05;
    rangeInfos.rejection = 4177527.0;
    rangeInfos.overejection = 5221909.0;
    m_currentRangeInfoMap.insert("50mA", rangeInfos);

    rangeInfos.availability = true;
    rangeInfos.rValue = 0.025;
    rangeInfos.rejection = 4177527.0;
    rangeInfos.overejection = 5221909.0;
    m_currentRangeInfoMap.insert("25mA", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 8.0;
    rangeInfos.rejection = 3355443.0;
    rangeInfos.overejection = 4194304.0;
    m_currentRangeInfoMap.insert("8V", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 5.0;
    rangeInfos.rejection = 4194304.0;
    rangeInfos.overejection = 5242880.0;
    m_currentRangeInfoMap.insert("5V", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 2.0;
    rangeInfos.rejection = 2835586.0;
    rangeInfos.overejection = 3544483.0;
    m_currentRangeInfoMap.insert("2V", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 1.0;
    rangeInfos.rejection = 2835586.0;
    rangeInfos.overejection = 3544483.0;
    m_currentRangeInfoMap.insert("1V", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.5;
    rangeInfos.rejection = 3544482.0;
    rangeInfos.overejection = 4430603.0;
    m_currentRangeInfoMap.insert("500mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.2;
    rangeInfos.rejection = 2684355.0;
    rangeInfos.overejection = 3355444.0;
    m_currentRangeInfoMap.insert("200mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.1;
    rangeInfos.rejection = 3355443.0;
    rangeInfos.overejection = 4194304.0;
    m_currentRangeInfoMap.insert("100mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.05;
    rangeInfos.rejection = 3355443.0;
    rangeInfos.overejection = 4194304.0;
    m_currentRangeInfoMap.insert("50mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.02;
    rangeInfos.rejection = 2684355.0;
    rangeInfos.overejection = 3355444.0;
    m_currentRangeInfoMap.insert("20mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.01;
    rangeInfos.rejection = 3355443.0;
    rangeInfos.overejection = 4194304.0;
    m_currentRangeInfoMap.insert("10mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.005;
    rangeInfos.rejection = 3355443.0;
    rangeInfos.overejection = 4194304.0;
    m_currentRangeInfoMap.insert("5mV", rangeInfos);

    rangeInfos.availability = false;
    rangeInfos.rValue = 0.002;
    rangeInfos.rejection = 2684355.0;
    rangeInfos.overejection = 3355444.0;
    m_currentRangeInfoMap.insert("2mV", rangeInfos);
}

QMap<QString, rangeInfosStruct> Mt310s2RangesStruct::getVoltageRangeInfo()
{
    return m_voltageRangeInfoMap;
}

QMap<QString, rangeInfosStruct> Mt310s2RangesStruct::getCurrentRangeInfo()
{
    return m_currentRangeInfoMap;
}
