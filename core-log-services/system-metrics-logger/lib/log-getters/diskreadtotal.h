#ifndef DISKREADTOTAL_H
#define DISKREADTOTAL_H

#include "abstractlogvaluegetter.h"
#include "diskiototalcalculator.h"
#include <memory>

class DiskReadTotal : public AbstractLogValueGetter
{
public:
    DiskReadTotal(std::unique_ptr<DiskIoTotalCalculator> diskTotalCalc);
    bool canGetValue() override;
    float getValue() override;

private:
    std::unique_ptr<DiskIoTotalCalculator> m_diskTotalCalc;
};

#endif // DISKREADTOTAL_H
