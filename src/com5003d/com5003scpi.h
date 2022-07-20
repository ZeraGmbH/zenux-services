// header datei für scpi erweiterung für com5003 server

#ifndef SCPIDSPS_H
#define SCPIDSPS_H

enum SCPICmdType  {	nixCmd, // 0 is nix
			
			// die routinen für das system modell
    
                                          SetSyncPeriod,
                                          GetSyncPeriod,
			SetSyncSource,
			GetSyncSource,
			SetPSamples,
			GetPSamples,
			SetSampleMode,
			GetSampleMode,
			SetSampleFrequency,
			GetSampleFrequency,
			SetSerialNumber,
			GetSerialNumber,
			ControlerStartProgram,
			ControlerStartBootloader,
			ControlerFlashUpdate,
			ControlerEEpromUpdate,
			SetPCBVersion,
			GetPCBVersion,
			GetCTRLVersion,
			GetLCAVersion,
			GetI2CMasterAdress,
			GetI2CSlaveAdress,
			GetI2CDeviceNode,
			SetDebugLevel,
			GetDebugLevel,
			GetDeviceVersion,
			GetServerVersion,
			
			// die routinen für das status modell
			
			GetAdjustmentStatus,
			GetDeviceStatus,
			GetChannelStatus,
  
			// die routinen für das calculate model
			
			OutCValueCatalog, // katalog der verfügbaren korrekturwerte
			GetRejection, // abfrage aussteuerung 100%
			GetRValue, // abfrage bereichendwert
			GetCValue, // abfrage der korrekturwerte
			SetStatus, // setzen status wort
			GetStatus, // abfrage status wort
			SetCValueCCoefficient, //setzen korrektur koeffizienten
			GetCValueCCoefficient, // abfrage korrektur koeffizienten
			GetCValueCCoefficientName, // abfrgae der namen derselben
			SetCValueCNode, // setzen korrektur stützstellen
			GetCValueCNode, // abfrage korrektur stützstellen
			GetCValueCNodeName,
			CmpCCoefficient, // berechnung der koeffizienten aus stützstellen
			SetCalculateModel, //
			
			// die routinen für das mmemory modell
    
			File2Justdata,
			JustData2File,
			EEProm2JustData,
			JustData2EEProm,
			GetEEPromEnable,
			GetEEPromChksum,
			
			// die routinen für das sense model
    
			OutRangeCatalog,
			GetRange,
			SetRange,
			OutChannelCatalog,
			SetProtection,
			GetProtection
		    
		    };

class cbIFace { // call back interface 
public:    
    virtual ~cbIFace(){}; // rein virtuell
    virtual const char* SCPICmd( SCPICmdType, char*) = 0; 
    virtual const char* SCPIQuery( SCPICmdType, char*) = 0;
};

#endif
