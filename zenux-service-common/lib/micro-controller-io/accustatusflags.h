#ifndef ACCUSTATUSFLAGS_H
#define ACCUSTATUSFLAGS_H

// Taken from systemcontroller / Thermal_and_Energy_Supervisor.h - no git repo yet

typedef enum {
    bp_Battery_Present                  = 0, // A battery module (with valid communication) is attached to the instrument
    bp_Battery_is_Charging              = 1, // The battery gets charged
    bp_Battery_Low_SoC_Warning          = 2, // The battery State-of-Charge is low an the user should be informed
    bp_Battery_Low_SoC_Shutdown_Alert   = 3, // The battery is depleted and the instrument will shut down automatically in the next time.
    // This shutdown of the Main-CPU (Linux) will be initiated by this systemcontroller-firmware.
    bp_Main_Supply_Present              = 7  // Main Supply is available
} en_BatteryStatusRegister;

#endif // ACCUSTATUSFLAGS_H
