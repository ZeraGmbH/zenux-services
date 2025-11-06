#ifndef EMOBDEFINITIONS_H
#define EMOBDEFINITIONS_H

// value returned on readEmobConnectionState - see CPU5975 / EMOB_CTRL.h
enum reademoblockstate
{
    emobstate_unknown,
    emobstate_open,
    emobstate_locking,
    emobstate_locked,
    emobstate_error
};

#endif // EMOBDEFINITIONS_H
