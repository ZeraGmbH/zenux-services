#ifndef HOTPLUGGABLECONTROLLERCONTAINER_H
#define HOTPLUGGABLECONTROLLERCONTAINER_H

#include "atmelemobctrl.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <QMap>
#include <QVector>
#include <memory>

class HotPluggableControllerContainer
{
public:
    HotPluggableControllerContainer(QString i2cDevNodeName, quint8 i2cAdrCtrl, quint8 i2cAdrMux, quint8 debuglevel);
    void actualizeEmobControllers(const cSenseSettings* senseSettings, quint16 bitmaskAvailable);
    QVector<AtmelCommonVersionsPtr> getCurrentControllers();
private:
    QString m_i2cDevNodeName;
    quint8 m_i2cAdrCtrl;
    quint8 m_i2cAdrMux;
    quint8 m_debuglevel;
    QMap<int /* BitNo in pluggable channel mask */, AtmelCommonVersionsPtr> m_Controllers;
};

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
