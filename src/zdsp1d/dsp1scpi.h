// header datei für scpi erweiterung für dsp server

#include <QString>

#ifndef SCPIDSPS_H
#define SCPIDSPS_H

enum SCPICmdType  {	nixCmd, // 0 is nix
			
			// die routinen für das system modell
						
                                        	SetDspCommandStat,
			GetDspCommandStat,
			SetEN61850DestAdr,
			GetEN61850DestAdr,
			SetEN61850SourceAdr,
			GetEN61850SourceAdr,
			SetEN61850EthTypeAppId,
			GetEN61850EthTypeAppId,
		              SetEN61850PriorityTagged,
			GetEN61850PriorityTagged,
			SetEN61850EthSync,
			GetEN61850EthSync,
			SetEN61850DataCount,
			GetEN61850DataCount,
			SetEN61850SyncLostCount,
			GetEN61850SyncLostCount,
			TriggerIntListHKSK,
			TriggerIntListALL,
			ResetMaxima,
            TestDsp,
			ResetDsp,
			BootDsp,
			SetDspBootPath,
			GetDspBootPath,
			GetPCBSerialNumber,
			GetDeviceVersion,
			GetServerVersion,  
			SetSamplingSystem,
			GetSamplingSystem,
			SetCommEncryption,
                                          GetCommEncryption,
					  
			// die routinen für das status modell
			ResetDeviceLoadMax,
			GetDeviceLoadMax,
			GetDeviceLoadAct,
			GetDspStatus,
			GetDeviceStatus,
  
			// die routinen für das measure modell
    
			Fetch,
			Initiate,
			UnloadCmdList,
			LoadCmdList,
			SetRavList,
			GetRavList,
			SetCmdIntList,
			GetCmdIntList,
			SetCmdList,
			GetCmdList,
			Measure,
    
			// die routinen für das memory modell
    
			DspMemoryRead,
			DspMemoryWrite  };

class cbIFace { // call back interface 
public:    
    virtual ~cbIFace(){}; // rein virtuell
    virtual QString SCPICmd( SCPICmdType, QChar*) = 0;
    virtual QString SCPIQuery( SCPICmdType) = 0;
};

#endif
