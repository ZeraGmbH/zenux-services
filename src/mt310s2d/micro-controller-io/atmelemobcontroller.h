#ifndef ATMELEMOBCONTROLLER_H
#define ATMELEMOBCONTROLLER_H

#include "atmelbaseinterface.h"

class AtmelEmobController : public ZeraMcontrollerBase, public AtmelBaseInterface
{
public:
    AtmelEmobController(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readCTRLVersion(QString& answer) override;
};

#endif // ATMELEMOBCONTROLLER_H
