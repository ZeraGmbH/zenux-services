#ifndef ADJDATARANGEGROUPSTREAMER_H
#define ADJDATARANGEGROUPSTREAMER_H

#include "adjdatarangegroup.h"
#include <QDataStream>
#include <memory>

class AdjDataRangeGroupStreamer
{
public:
    static std::shared_ptr<AdjDataRangeGroup> Deserialize(QDataStream& qds);
};

#endif // ADJDATARANGEGROUPSTREAMER_H
