#ifndef CONTROLLERCLAMPCOMBINATIONS_H
#define CONTROLLERCLAMPCOMBINATIONS_H

#include "emobdefinitions.h"
#include "clamp.h"

struct HotpluggableUnits {
    QString m_plugChannelMName;
    ControllerTypes m_controllerType = EMOBUnknown;
    const cClamp::ClampTypes m_clampType = cClamp::undefined;
};

#endif // CONTROLLERCLAMPCOMBINATIONS_H
