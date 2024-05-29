#ifndef ADJDATAITEMSTREAM_H
#define ADJDATAITEMSTREAM_H

#include "adjdataitem.h"
#include <QDataStream>

class AdjDataItemStream
{
public:
    static void fromStream(AdjDataItem &item, QDataStream& qds);
    static void toStream(AdjDataItem item, QDataStream& qds);
};

#endif // ADJDATAITEMSTREAM_H
