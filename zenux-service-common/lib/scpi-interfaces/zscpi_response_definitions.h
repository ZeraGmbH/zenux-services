#ifndef ZSCPI_RESPONSE_DEFINITIONS_H
#define ZSCPI_RESPONSE_DEFINITIONS_H

#include <QString>

namespace ZSCPI
{
enum eSCPICmdResult { ack, nak, busy, erraut, errval, errxml,
                      errpath, errexec,
                      resultcount };

const QString scpiAnswer[resultcount] = {
    "ack",     ///< acknowledge (command recognized and executed)
    "nak",     ///< not acknowledge (kommando not recognized)
    "busy",    ///< device busy
    "erraut",  ///< error authorization (command recognized but client is not auth. )
    "errval",  ///< error value (command recognized but incorrect parameter)
    "errxml",  ///< error xml (used xml file was incorrect)
    "errpath", ///< error path
    "errexec", ///< error execution
};

}

#endif // ZSCPI_RESPONSE_DEFINITIONS_H
