#include "zscpi_response_definitions.h"

namespace ZSCPI
{
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
