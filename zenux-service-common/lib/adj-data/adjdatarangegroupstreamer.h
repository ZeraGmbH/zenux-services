#ifndef ADJDATARANGEGROUPSTREAMER_H
#define ADJDATARANGEGROUPSTREAMER_H

#include "adjdatarangegroup.h"
#include <QDataStream>

class AdjDataRangeGroupStreamer
{
public:
    static AdjDataRangeGroup Deserialize(QDataStream& qds);
    static void Serialize(AdjDataRangeGroup adjGroup, QDataStream& qds);
};

#endif // ADJDATARANGEGROUPSTREAMER_H
