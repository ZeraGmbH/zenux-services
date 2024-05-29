#ifndef ADJDATARANGEGROUPSTREAM_H
#define ADJDATARANGEGROUPSTREAM_H

#include "adjdatarangegroup.h"
#include <QDataStream>

class AdjDataRangeGroupStream
{
public:
    static AdjDataRangeGroup fromStream(QDataStream& qds);
    static void toStream(AdjDataRangeGroup adjGroup, QDataStream& qds);
};

#endif // ADJDATARANGEGROUPSTREAM_H
