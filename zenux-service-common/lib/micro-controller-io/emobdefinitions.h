#ifndef EMOBDEFINITIONS_H
#define EMOBDEFINITIONS_H

// Value returned on readEmobConnectionState - see CPU5975 / EMOB_CTRL.h
typedef enum t_EMOB_Status_Verriegelung {
    EMOB_Status_Verriegelung_unknown,
    EMOB_Status_Verriegelung_offen,
    EMOB_Status_Verriegelung_laueft,
    EMOB_Status_Verriegelung_geschlossen,
    EMOB_Status_Verriegelung_Fehler
} t_EMOB_Status_Verriegelung;

// Mask returned on readEmobPruefgroessenState - see CPU5975 / CPU5975_Commands.h
// Values are bit positions!!!
enum en_Instrument_Error_Status {
    Instrument_ErrorStatus_Supply_Lost,
    Instrument_ErrorStatus_Device_Error,
    Instrument_ErrorStatus_Sensor_Error,
    Instrument_ErrorStatus_Cable_Error,
    Instrument_ErrorStatus_Overcurrent,
    Instrument_ErrorStatus_Overtemperature,
    Instrument_ErrorStatus_Verrieglerfehler,
    Instrument_ErrorStatus_Problem_at_neighbour_EMOB
};

// Mask returned on readEmobPruefgroessenState - see CPU5975 / CPU5975_Commands.h
enum en_Instrument_Pruefgroessenstatus {
    bp_Meas_Status_AC_High_Voltage_detected,
    bp_Meas_Status_DC_High_Voltage_detected,
};

#endif // EMOBDEFINITIONS_H
