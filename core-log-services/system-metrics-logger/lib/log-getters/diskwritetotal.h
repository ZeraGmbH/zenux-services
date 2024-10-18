#ifndef DISKWRITETOTAL_H
#define DISKWRITETOTAL_H

#include "abstractlogvaluegetter.h"
#include "diskiototalcalculator.h"
#include <memory>

class DiskWriteTotal : public AbstractLogValueGetter<float>
{
public:
    DiskWriteTotal(std::unique_ptr<DiskIoTotalCalculator> diskTotalCalc);
    bool canGetValue() override;
    float getValue() override;

private:
    std::unique_ptr<DiskIoTotalCalculator> m_diskTotalCalc;
};

#endif // DISKWRITETOTAL_H
