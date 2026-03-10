#ifndef DSPVARINSERVER_H
#define DSPVARINSERVER_H

#include "dspvarclientinterface.h"
#include <QString>
#include <memory>

struct DspVarInServer {
    bool setupFromCommaSeparatedString(const QString& varDefinition);
    static QString toHex(int val);

    QString m_clientVarGroupName;
    QString Name;                           // name der variablen
    ushort size = 0;                        // anzahl worte
    DspDataType type = dspDataTypeUnknown;
    ulong m_absoluteAddress = 0;            // used by writing vars by client
    // TODO: Name is incorrect: handled differently for user/aligned
    // see DspVarResolver::getVarOffset and ZdspClient::calcVarAdressesAndSizes
    ulong m_offsetToModuleBase = 0;         // used by command lists (paramters are 16Bit wide)
    DspSegmentType segment = moduleLocalSegment;
};

typedef std::shared_ptr<DspVarInServer> DspVarServerPtr;

#endif // DSPVARINSERVER_H
