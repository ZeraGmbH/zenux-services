#ifndef TEST_REGRESSION_SENSE_RANGE_MT310S2_H
#define TEST_REGRESSION_SENSE_RANGE_MT310S2_H

#include "mt310s2senserange.h"
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
    void checkTypeOrMask();
    void checkGetJustData();
private:
    cSCPI *m_scpi;
    RangeAdjustmentInterface *m_justData;
    Mt310s2SenseRange *m_range;
};

#endif // TEST_REGRESSION_SENSE_RANGE_MT310S2_H
