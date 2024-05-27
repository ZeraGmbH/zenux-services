#ifndef ADJDATAITEMSTREAMER_H
#define ADJDATAITEMSTREAMER_H

#include "adjdataitem.h"
#include <QDataStream>
#include <memory>

class AdjDataItemStreamer
{
public:
    AdjDataItemStreamer(int order);
    AdjDataItem Deserialize(QDataStream& qds);
private:
    int m_order;
};

#endif // ADJDATAITEMSTREAMER_H
