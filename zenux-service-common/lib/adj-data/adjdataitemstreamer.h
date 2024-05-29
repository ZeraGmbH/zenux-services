#ifndef ADJDATAITEMSTREAMER_H
#define ADJDATAITEMSTREAMER_H

#include "adjdataitem.h"
#include <QDataStream>

class AdjDataItemStreamer
{
public:
    static void fromStream(AdjDataItem &item, QDataStream& qds);
    static void toStream(AdjDataItem item, QDataStream& qds);
};

#endif // ADJDATAITEMSTREAMER_H
