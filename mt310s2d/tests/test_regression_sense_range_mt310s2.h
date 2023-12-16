#ifndef TEST_REGRESSION_SENSE_RANGE_MT310S2_H
#define TEST_REGRESSION_SENSE_RANGE_MT310S2_H

#include "senserange.h"
#include <QObject>

class test_regression_sense_range_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void checkName();
    void checkAlias();
    void checkAvail();
    void checkUrValue();
    void checkRejection();
    void checkOvRejection();
    void checkAdcRejection();
    void checkAtmelSelectionCode();
    void checkMask();

    void checkGetJustData();
private:
    cSCPI *m_scpi;
    JustRangeTripletOffsetGainPhaseMt310s2 *m_justData;
    cSenseRange *m_range;
};

#endif // TEST_REGRESSION_SENSE_RANGE_MT310S2_H
