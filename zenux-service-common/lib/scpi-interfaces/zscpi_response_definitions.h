#ifndef ZSCPI_RESPONSE_DEFINITIONS_H
#define ZSCPI_RESPONSE_DEFINITIONS_H

#include <QString>

namespace ZSCPI
{
enum eSCPICmdResult { ack, nak, busy, erraut, errval, errxml,
                      errpath, errexec,
                      resultcount };

extern const QString scpiAnswer[resultcount];

}

#endif // ZSCPI_RESPONSE_DEFINITIONS_H
