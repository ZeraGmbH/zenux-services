#ifndef ADJDATARANGESTREAM_H
#define ADJDATARANGESTREAM_H

#include "adjdatarange.h"
#include <QDataStream>

class AdjDataRangeStream
{
public:
    static AdjDataRange fromStream(QDataStream& qds);
    static void toStream(AdjDataRange adjGroup, QDataStream& qds);
};

#endif // ADJDATARANGESTREAM_H
