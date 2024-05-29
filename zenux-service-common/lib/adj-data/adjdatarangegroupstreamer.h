#ifndef ADJDATARANGEGROUPSTREAMER_H
#define ADJDATARANGEGROUPSTREAMER_H

#include "adjdatarangegroup.h"
#include <QDataStream>

class AdjDataRangeGroupStreamer
{
public:
    static AdjDataRangeGroup fromStream(QDataStream& qds);
    static void toStream(AdjDataRangeGroup adjGroup, QDataStream& qds);
};

#endif // ADJDATARANGEGROUPSTREAMER_H
