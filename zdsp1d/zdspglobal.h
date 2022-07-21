// globale definition

#ifndef ZDSPGLOBAL_H
#define ZDSPGLOBAL_H


// v1.01 erste lauffähige version
// v1.02 ergänzt : 
// 	{"INC", 54, CMD1i16, 0 },
//	{"COPYINDDATA", 55, CMD3i16, 0 },
//	{"TestSyncPPSSkipEQ",56,CMD,0},
//	{"ResetSyncPPS",57,CMD,0}};
//v1.03 ergänzt :
//	es existieren im dsp ab version 3.05 2 sätze von kommando listen welche immer im wechsel geladen
// 	werden sollten. dies wird vom dsp server ab dieser version unterstüzt. die beiden versionen müssen
//	unbedingt zusammenpassen. wenn also ein dsp mit v3.05 und grösser vorhanden ist, muss unbedingt
// 	ein server mit v1.03 und grösser vorhanden sein und umgekehrt.
//v1.04 ergänzt :
//	es gab probleme im zusammenhang mit interrupts. ein einlaufender interrupt vom dsp konnte nicht bearbeitet werden wenn der select aufruf blockierte . input oder verbindungsaufbau liess das system weiter laufen. es wurde deshalb ein timeout (50 usec) eingeführt damit der select aufruf in jedem fall zurückkehrt. behandlung der flagge gotSIGIO wurde gändert, weil u.U. interrupts nicht bearbeitet wurden und sich der dsp dann aufhing.
//v1.05 ergänzt :
//	neuer befehl für dsp (SQRT) eingeführt .... läuft aber nur mit dsp ab V3.06
//v1.06 fehler bereinigt @ gencmd für cmdi16if32 fehler bei 1. parameter wurde nicht gemeldet
//v1.07 ergänzt :
//  neue befehle für dsp
//  {"COPYVAL",61,CMD2i16,0},
//  {"CMPCLK",62,CMD3i16,0},
//  {"CLKMODE",63,CMD1i16,0},
//  {"GETSTIME",64,CMD1i16,0},
//  {"TESTTIMESKIPLT",65,CMD2i16,0}
//  eingeführt. ..... läuft aber erst mit dsp ab version 3.07
//  ADSP21362 unterstützung eingebaut, automatische umschaltung über magicId
//v1.08 ergänzt :
// neue befehle für dsp
// {"SUBVCC",66,CMD3i16,0}}
//  eingeführt. ..... läuft aber erst mit dsp ab version 3.08
//v1.09:
//  ressourcemanager support added, linux event loop changed to qt event loop(qcoreapllication)
//  dsp var decoding uses nhash tables now (qhash)
//  server configuration with xml implemented incl. booting dsp, setting sampling system
//  new dsp command {"DSPINTPOST,68,CMD1i16,0}
//  serialized dsp interrupts support implemented
//  meas. modules merging DSPINTPOST added at end of list
//v1.10:
//  new dsp commands added:
//  {"COPYDATAIND",70,CMD3i16,0}, {"INTERPOLATIONIND",71,CMD3i16,0},
//  {"COPYMEM",72,CMD3i16,0 }, {"GENADR",73,CMD3i16,0 }
//  added fout routing field to "POWVALS4FOUT" to dialogworkspace
//v1.11:
//  new network connection zdsp1d.socket

#define DSPDeviceNode "/dev/zFPGA1dsp1"
#define ServerBasisName "zdsp1d"
#define ServerVersion "V1.11"
#define InpBufSize 4096
#define defaultXSDFile "/etc/zera/zdsp1d/zdsp1d.xsd"

#define MaxDebugLevel 7
#define DEBUG1 (m_nDebugLevel & 1) // alle fehlermeldungen loggen
#define DEBUG2 (m_nDebugLevel & 2) // alle i2c aktivitäten loggen
#define DEBUG3 (m_nDebugLevel & 4) // alle client an-,abmeldungen

enum ServerErrors
{
    noError,
    forkError,
    parameterError,
    pipeError,
    xsdfileError,
    xmlfileError,
    dspDeviceError,
    dspBootFileError,
    dspBootError,
    dspSetSamplingError,
    rmConnectionError
};

// wenn ZDSP1DDEBUG -> kein fork()
//#define ZDSP1DDEBUG 1

#endif
