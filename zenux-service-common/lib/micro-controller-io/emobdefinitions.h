#ifndef EMOBDEFINITIONS_H
#define EMOBDEFINITIONS_H

// A full list is found at CPU5975/Ausstattungsmerkmale.h. We don't need to
// know all the details. So reduce types to our needs:
enum ControllerTypes {
    EMOBUnknown,
    EMOB,
    MT650e
};

// value returned on readEmobConnectionState - see CPU5975 / EMOB_CTRL.h
enum reademoblockstate
{
    emobstate_unknown,
    emobstate_open,
    emobstate_locking,
    emobstate_locked,
    emobstate_error
};

enum errorInstrumentStatus {
    Instrument_Status_Supply_Lost,
    Instrument_Status_Device_Error,
    Instrument_Status_Sensor_Error,
    Instrument_Status_Cable_Error,
    Instrument_Status_Overcurrent,
    Instrument_Status_Overtemperature,
    Instrument_Status_Verrieglerfehler,
    Instrument_Status_Problem_at_neighbour_EMOB
};
#endif // EMOBDEFINITIONS_H
