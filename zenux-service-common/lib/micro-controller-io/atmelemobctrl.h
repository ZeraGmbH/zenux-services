#ifndef ATMELEMOBCTRL_H
#define ATMELEMOBCTRL_H

#include <atmelcommon.h>
#include <atmelcommonversions.h>

class AtmelEmobCtrl : public ZeraMcontrollerBase, public AtmelCommonVersions
{
public:
    AtmelEmobCtrl(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel);
    virtual ~AtmelEmobCtrl() = default;
    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readPCBVersion(QString& answer) override;
};

#endif // ATMELEMOBCTRL_H
