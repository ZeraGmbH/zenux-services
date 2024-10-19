#ifndef DISKIOPERPROCESS_H
#define DISKIOPERPROCESS_H

#include "abstractlogvaluegetter.h"
#include "diskreadwritevalues.h"
#include <QVector>

class DiskIoForAllProcesses : public AbstractLogValueGetter<DiskValuesProcesses>
{
public:
    bool canGetValue() override;
    DiskValuesProcesses getValue() override;
private:
    DiskValuesProcesses m_lastValues;
};

#endif // DISKIOPERPROCESS_H
