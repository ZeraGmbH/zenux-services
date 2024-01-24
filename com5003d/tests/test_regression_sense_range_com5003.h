#ifndef TEST_REGRESSION_SENSE_RANGE_COM5003_H
#define TEST_REGRESSION_SENSE_RANGE_COM5003_H

#include "senserangecommon.h"
#include <QObject>

class test_regression_sense_range_com5003 : public QObject
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
private:
    cSCPI *m_scpi;
    SenseRangeCommon *m_range;
};

#endif // TEST_REGRESSION_SENSE_RANGE_COM5003_H
