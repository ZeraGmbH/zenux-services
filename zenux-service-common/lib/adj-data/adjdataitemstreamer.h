#ifndef ADJDATAITEMSTREAMER_H
#define ADJDATAITEMSTREAMER_H

#include "adjdataitem.h"
#include <QDataStream>
#include <memory>

class AdjDataItemStreamer
{
public:
    static void Deserialize(AdjDataItem &item, QDataStream& qds);
    static void Serialize(AdjDataItem item, QDataStream& qds);
};

#endif // ADJDATAITEMSTREAMER_H
