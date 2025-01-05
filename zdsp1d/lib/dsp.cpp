#include "dsp.h"
#include "dspvardefinitions.h"
#include <QHash>

// alle angaben aus dsp assembler programm 
/*--------------------------- user basics -----------------------------------*/ 
/* r0,r1 enthalten 64Bit Befehlscode incl. Daten, bits63-48 sind Kommando */ 
/* bits47-0 können 16Bit Adressen, 32Bit Adressen oder Daten enthalten */ 
/* folgende Liste aus C-Programm gibt Aufschluss über die exakte Belegung*/ 
  
/* #define DspCmd_InValid(_Dummy) {0<<16,0} */ 
/* Daten ab hier ungültig */ 
/* #define DspCmd_UserMemOffset(_UOffset) {0x1<<16,_UOffset} */ 
/* Adressoffset UserWorkSpace setzen (32bit adressoffset) */ 
/* #define DspCmd_DspMemOffset(_DOffset) {0x2<<16,_DOffset} */ 
/* Adressoffset DspWorkSpace setzen (32bit adressoffset) */ 
/* #define DspCmd_CopyData(_CH,_Phi,_Adr) {(3<<16)+_CH,(_Phi*N/360)<<16+(_Adr&0xFFFF)} */ 
/* Kopiere Daten von Wechselpuffer Kanal _CH phasenverschoben mit Phi(volle °) 
   nach Adresse Adr im Speicherblock ab Offset */  
/* #define DspCmd_CopyDiff(_CHA,_CHB,_Adr) {(4<<16)+_CHA,_CHB<<16+(_Adr&0xFFFF)} */ 
/* Kopiere Differenz von Wechselpuffer Kanal _CHA - _CHB nach Adresse Adr  
   im Speicherblock ab Offset */  
/* #define DspCmd_CopyProd(_CHA,_CHB,_Adr) {(5<<16)+_CHA,_CHB<<16+(_Adr&0xFFFF)} */ 
/* Kopiere Produkt von Wechselpuffer Kanal _CHA - _CHB nach Adresse Adr  
   im Speicherblock ab Offset */  
/* #define DspCmd_DiffC(_Adr1,_Adr2,_Adr3) {(6<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* Subtrahiere Kurve ab Adr2 von Kurve ab Adr1 und speichere die Differenzkurve ab Adr3 */  
/* #define DspCmd_Rms(_Adr1,_Adr2) {(7<<16),_Adr1<<16+(_Adr2&0xFFFF)}  */ 
/* Berechne den RMS-Wert der Kurve ab Adr1 und speichere den Wert auf Adr2 */  
/* #define DspCmd_PowerC(_Adr1,_Adr2,_Adr3) {(8<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* #define DspCmd_ProductC(_Adr1,_Adr2,_Adr3) {(8<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* Berechne die Leistung oder das Produkt der Kurven ab Adr1 und Adr2 und speichere 
   die Ergebniskurve ab Adr3 */  
/* define DspCmd_Dft(_n,_Adr1,_Adr2) {(9<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Berechne die DFT für die n-te Oberschwingung der Kurve ab Adr1 und speichere den  
   Real-,Imaginärwert ab Adr2 */  
/* #define DspCmd_SetVal(_Adr1,_Val) {(10<<16)+_Adr1,(unsigned long)(_Val)} */ 
/* Setze einen Floatingpointwert auf Adr1 */  	 
/* #define DspCmd_Rotate(_Adr1,_Phase) {(11<<16),_Adr1<<16+(unsigned long)(_Phase)} */ 
/* Verschiebe die Kurve ab Adr1 um den Winkel Phase im Speicher */ 
/* #define DspCmd_ApparentA(_Adr1,_Adr2,_Adr3) {(12<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* #define DspCmd_ProductV(_Adr1,_Adr2,_Adr3) {(12<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */  
/* Berechne die arithmetische Scheinleistung oder das Produkt der beiden Werte von  
   Adr1 und Adr2 und speichere das Resultat auf Adr3 */  
/* #define DspCmd_ApparentG(_Adr1,_Adr2,_Adr3) {(13<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* #define DspCmd_AddVG(_Adr1,_Adr2,_Adr3) {(13<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */  
/* Berechne die geometrische Scheinleistung bzw. die geom. Summme der beiden Werte von Adr1 
   und Adr2 und speichere das Resultat auf Adr3 */  
/* #define DspCmd_ClkOut(_Dummy) {(14<<16),0)} */ 
/* Berechne den Taktausgangsteiler incl. Tiefpassfilterung der 4 Istwerte */ 
/* #define DspCmd_Average1(_n,_Adr1,_Adr2) {(15<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Führe die Mittelwertbildung Average1 aus bei der n Float-Werte ab Adr1 gemittelt 
   und ab Adr2 bearbeitet bzw. gespeichert werden */  
/* #define DspCmd_Integral(_n,_Adr1,_Adr2) {(16<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Integriere n Messwerte ab Adr1 und speichere das Ergebnis auf Adr2 */ 
/* #define DspCmd_AddV(_Adr1,_Adr2,_Adr3) {(17<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* Addiere die beiden Float Werte von Adr1 und Adr2 und speichere das Resultat auf Adr3 */  
/* #define DspCmd_CopyUD(_n,_Adr1,_ADR1) {(18<<16)+_n,_Adr1<<16+(_ADR1&0xFFFF)} */ 
/* Kopiere n Float Werte ab Adr1 im Userworkspace nach ADR1 im DSPWorkspace */  
/* #define DspCmd_CopyDU(_n,_Adr1,_ADR1) {(19<<16)+_n,_Adr1<<16+(_ADR1&0xFFFF)} */  
/* Kopiere n Float Werte ab ADR1 im DSPWorkspace nach Adr1 im Userworkspace */  
/* #define DspCmd_CmpAverage1(_n,_Adr1,_Adr2) {(20<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Führe die Berechnung der Mittelwertbildung Average1 aus bei der n */ 
/* float Summenwerte ab Adr1 durch den Divisor auf Adr=Adr1+n geteilt */ 
/* werden und ab Adr2 gespeichert werden */  
/* #define DspCmd_ClearN(n,_Adr1) {(21<<16)+_n,_Adr1<<16} */ 
/* lösche n float-werte ab Adresse Adr1 , für filterbereich von average 1*/  	 
/* muss die doppelte anzahl genommen werden !!!!!!!!!!*/ 
/* #define DspCmd_PowerX(_Adr1,_Adr2,_Adr3) {(22<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* #define DspCmd_ProductX(_Adr1,_Adr2,_Adr3) {(22<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* Berechne die Leistung oder das Produkt der Kurven ab Adr1 und Adr2 und speichere 
   das Ergebnis auf Adr3 */  
/* #define DspCmd_InterPolation(_n,_Adr1,_Adr2) {(23<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Berechne eine neue Kurve mittels linearer Interpolation. Die alte Kurve liegt ab */ 
/* Adr1 im UserWorkspace und hat die Länge nSamples. Die neue Kurve mit der Länge n soll */ 
/* ab Adr2 im UserWorkspace abgelegt werden. */ 
/* #define DspCmd_FFTReal(_n,_Adr1,_Adr2) {(24<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Berechne eine n Punkte FFT. Eingangsdaten real liegen ab Adr1 im UserWorkspace */ 
/* die imag. Eingangsdaten Adr1+_n werden 0 gesetzt */ 
/* Die FFT Ausgangsdaten sollen ab Adr2 im UserWorkspace abgelegt werden. */ 
/* #define DspCmd_FFTComplex(_n,_Adr1,_Adr2) {(25<<16)+_n,_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Berechne eine n Punkte FFT. Eingangsdaten real und imaginär liegen ab Adr1 im UserWorkspace */ 
/* Die FFT Ausgangsdaten sollen ab Adr2 im UserWorkspace abgelegt werden. */ 
/* #define DspCmd_THDN(_Adr1,_Adr2) {(26<<16),_Adr1<<16+(_Adr2&0xFFFF)} */ 
/* Berechne den Klirrfaktor (THD+Noise) der Kurve ab Adr1 und lege das Ergebnis */ 
/* auf Adr2 im UserWorkspace ab. */ 
/* #define DspCmd_SummC(_Adr1,_Adr2,_Adr3) {(27<<16)+_Adr1,_Adr2<<16+(_Adr3&0xFFFF)} */ 
/* Addiere Kurve ab Adr2 zu Kurve ab Adr1 und speichere die Summenkurve ab Adr3 */  
/* #define DspCmd_MulCV(_Adr1,_Val) {(28<<16)+_Adr1,(unsigned long)(_Val)} */ 
/* multipliziere die Kurve ab Adr1 mit Val, ergebnis wieder aud Adr1 */  	 
 
/* #define DspCmd_StartChain(_act,_HKSK_Nr) {(29<<16),_act<<16+(_HKSK_Nr&0xFFFF)} */ 
/* definiert Start der Kette mit Haupt-,Subketten Nr.(je 8bit) und ob Kette aktiv ist */ 
/* #define DspCmd_StopChain(_HKSK_Nr) {(30<<16),(_HKSK_Nr&0xFFFF)} */ 
/* definiert Stop der Kette mit Haupt-,Subketten Nr.(je 8bit) */ 
/* #define DspCmd_ActivateChain(_HKSK_Nr) {(31<<16),(_HKSK_Nr&0xFFFF)} */ 
/* aktiviert die Kette mit der Haupt-,Subketten Nr.(je 8bit) */ 
/* #define DspCmd_DeactivateChain(_HKSK_Nr) {(32<<16),(_HKSK_Nr&0xFFFF)} */ 
/* deaktiviert die Kette mit der Haupt-,Subketten Nr.(je 8bit) */ 
 
/* #define DspCmd_TestVCSkipEQ(_Adr1,Wert) {(33<<16)+_Adr1,Wert} */ 
/* testet eine variable(_Adr1) auf Gleichheit mit der Konstanten Wert */ 
/* wenn gleich wird der nächste befehl geskippt */
/* #define DspCmd_TestVCSkipGT(_Adr1,Wert) {(34<<16)+_Adr1,Wert} */ 
/* testet eine variable(_Adr1) auf grösser mit der Konstanten Wert */ 
/* wenn kleiner,gleich wird der nächste befehl geskippt */ 
/* #define DspCmd_TestVCSkipLT(_Adr1,Wert) {(35<<16)+_Adr1,Wert} */ 
/* testet eine variable(_Adr1) auf kleiner mit der Konstanten Wert */ 
/* wenn grösser,gleich wird der nächste befehl geskippt */ 
 
/* #define DspCmd_TestVVSkipEQ(_Adr1,_Adr2) {(36<<16),_Adr1<<16+_Adr2} */ 
/* testet eine variable(_Adr1) auf Gleichheit mit der variablen(_Adr2) */ 
/* wenn ungleich wird der nächste befehl geskippt */ 
/* #define DspCmd_TestVVSkipGT(_Adr1,_Adr2) {(37<<16),_Adr1<<16+_Adr2} */ 
/* testet eine variable(_Adr1) auf grösser mit der variablen(_Adr2) */ 
/* wenn kleiner,gleich wird der nächste befehl geskippt */ 
/* #define DspCmd_TestVVSkipLT(_Adr1,_Adr2) {(38<<16),_Adr1<<16+_Adr2} */ 
/* testet eine variable(_Adr1) auf kleiner mit der variablen(_Adr2) */ 
/* wenn grösser,gleich wird der nächste befehl geskippt */ 
 
/* #define DspCmd_WriteIntCmd(_HKSK_Nr) {(39<<16),_HKSK_Nr} */ 
/* schreibt ein interruptkommando mit parameter in den DSPCmdPar Buffer */ 
/* _HKSK_Nr=0 -> komplette kette bearbeiten */ 
/* #define DspCmd_CtrlIntTrigger() {(40<<16),0} */ 
/* triggert den Int der sonst vom Controler kommt per software */ 
/* #define DspCmd_DSPIntTrigger(_Cmd,_HKSK_Nr) {(41<<16),_Cmd<<16+_HKSK_Nr} */ 
/* löst einen interrupt beim controler aus gibt ein kennung ab (cmd) und die ProzessNr */ 
 
/* #define DspCmd_Hanning(_Adr1,_Adr2) {(42<<16),_Adr1<<16+_Adr2} */ 
/* multipliziert die Daten ab _Adr1 imit einer Hanningfunktion und legt */ 
/* die Daten ab _Adr2 im speicher ab. _Adr2=_Adr1 ist möglich. */ 
/* #define DspCmd_SetPort(_Adr1,_Dat) {(43<<16),_Adr1<<16+_Dat} */
/* schreibt über ParallelPort die Daten _Dat auf Adresse _Adr */


static sDspCmd DspCmd[] =
{{"INVALID", 0, CMD ,0},
{"USERMEMOFFSET", 1, CMD1i32, 0 },
{"DSPMEMOFFSET", 2, CMD1i32, 0 },
{"COPYDATA", 3, CMD3i16, 0 },
{"COPYDIFF", 4, CMD3i16, 0 },
{"COPYPROD", 5, CMD3i16, 0 },
{"DIFFCCC", 6, CMD3i16, 0 },
{"RMS", 7, CMD2i16, 0 },
{"MULCCC", 8, CMD3i16, 0 },
{"DFT", 9, CMD3i16, 0 },
{"SETVAL", 10, CMD1i161fi32, 0 },
{"ROTATE", 11, CMD1i161fi32, 0 },
{"MULVVV", 12, CMD3i16, 0 },
{"ADDVVG", 13, CMD3i16, 0 }, // wie apparentg (allgemeingültiger)
{"CLKOUT", 14, CMD, 0 },
{"AVERAGE1", 15, CMD3i16, 0 },
{"INTEGRAL", 16, CMD3i16, 0 },
{"ADDVVV", 17, CMD3i16, 0 },
{"COPYUD", 18, CMD3i16, 0 },
{"COPYDU", 19, CMD3i16, 0 },
{"CMPAVERAGE1", 20, CMD3i16, 0 },
{"CLEARN", 21, CMD2i16, 0 },
{"MULCCV", 22, CMD3i16, 0 }, // wie powerx (allgemeingültiger)
{"INTERPOLATION", 23, CMD3i16, 0 },
{"FFTREAL", 24, CMD3i16, 0 },
{"FFTCOMPLEX", 25, CMD3i16, 0 },
{"THDN", 26, CMD2i16, 0 },
{"ADDCCC", 27, CMD3i16, 0 },
{"MULCV", 28, CMD1i161fi32, 0 },
{"STARTCHAIN", 29, CMD3i16, 1 },
{"STOPCHAIN", 30, CMD2i16, 1 },
{"ACTIVATECHAIN", 31, CMD2i16, 1 },
{"DEACTIVATECHAIN", 32, CMD2i16, 1 },
{"TESTVCSKIPEQ", 33, CMD1i161fi32, 0 },
{"TESTVCSKIPGT", 34, CMD1i161fi32, 0 },
{"TESTVCSKIPLT", 35, CMD1i161fi32, 0 },
{"TESTVVSKIPEQ", 36, CMD2i16, 0 },
{"TESTVVSKIPGT", 37, CMD2i16, 0 },
{"TESTVVSKIPLT", 38, CMD2i16, 0 },
{"WRITEINTCMD", 39, CMD2i16, 1 },
{"CTRLINTTRIGGER", 40, CMD, 0 },
{"DSPINTTRIGGER", 41, CMD2i16, 1 },
{"HANNING", 42, CMD2i16, 0 },
{"WRITEPORT", 43, CMD2i16, 0 },
{"SINUS", 44, CMD3i16, 0 },
{"COSINUS", 45, CMD3i16, 0 },
{"AUTOKORRELATION", 46, CMD3i16, 0 },
{"SUBVVV", 47, CMD3i16, 0 },
{"NORMVC", 48, CMD1i161fi32, 0 },
{"ARCTAN", 49, CMD3i16, 0 },
{"DIVVVV", 50, CMD3i16, 0 },
{"SYMPHI", 51, CMD2i16, 0 },
{"BREAK", 52, CMD1i16,0 },
{"FLATTOP", 53, CMD2i16, 0 },
{"INC", 54, CMD1i16, 0 },
{"COPYINDDATA", 55, CMD3i16, 0 },
{"TESTSYNCPPSSKIPEQ", 56, CMD, 0},
{"RESETSYNCPPS", 57, CMD, 0},
{"RMSN", 58, CMD3i16, 0},
{"MULNCC", 59, CMD3i16, 0},
{"SQRT", 60, CMD2i16, 0},
{"COPYVAL", 61, CMD2i16, 0},
{"CMPCLK", 62, CMD3i16, 0},
{"CLKMODE", 63, CMD1i16, 0},
{"GETSTIME", 64, CMD1i16, 0},
{"TESTTIMESKIPNEX", 65, CMD2i16, 0},
{"SUBVCC", 66, CMD3i16, 0},
{"SUBVVG", 67, CMD3i16, 0},
{"DSPINTPOST", 68, CMD, 0},
{"SETPEAK", 69, CMD2i16, 0},
{"COPYDATAIND", 70, CMD3i16, 0},
{"INTERPOLATIONIND", 71, CMD3i16, 0},
{"COPYMEM", 72, CMD3i16, 0 },
{"GENADR",73,CMD3i16,0 },
{"COPYDATAWDC", 74, CMD3i16, 0 },
{"INTEGRALPOS", 75, CMD3i16, 0 },
{"INTEGRALNEG", 76, CMD3i16, 0 },
{"SUBNVC", 77, CMD3i16, 0 }};


TDspVar DspWorkspaceVar[] =
{
    {"DspWorkspace", "FREQENCY",1,eFloat,0,0, localSegment},                // 1 wert gemessene frequenz
    {"DspWorkspace", "FREQUENCYVALUE",4,eFloat,0,0, localSegment},          // 4 werte f. freq. ausgänge
    {"DspWorkspace", "MAXIMUMSAMPLE",32,eFloat,0,0, localSegment},          // 32 werte maximumspeicher
    {"DspWorkspace", "FREQUENCYVALUEFILTER",4,eFloat,0,0, localSegment},    // 4 gefilterte freq. ausgänge
    {"DspWorkspace", "FREQUENCYSCALE",4,eFloat,0,0, localSegment},          // 4 freq. skalierungswerte
    {"DspWorkspace", "KREISFREQKOEFF",1,eFloat,0,0, localSegment},          // kreisfrequenz korrektur koeffizient
    {"DspWorkspace", "ETHERRORS",1,eInt,0,0, localSegment},                 // ethernet fehler speicher
    {"DspWorkspace", "ETHSYNCLOSTCOUNT",1,eInt,0,0, localSegment},          // ethernet synclost counter
    {"DspWorkspace", "ETHDATACOUNT",2,eInt,0,0, localSegment},              // anzahl sync. verluste mu
    {"DspWorkspace", "NCHANNELS",1,eInt,0,0, localSegment},                 // anzahl messkanäle sampling system
    {"DspWorkspace", "NSMEAS",1,eInt,0,0, localSegment},                    // anzahl samples für 1 messperiode
    {"DspWorkspace", "NSPERIOD",1,eInt,0,0, localSegment},                  // anzahl samples für eine signalperiode
    {"DspWorkspace", "_NSPERIOD",1,eFloat,0,0, localSegment},               // 1/ anzahl samples
    {"DspWorkspace", "SYNCASDU",1,eInt,0,0, localSegment},                  // ob und auf welchen datensatz synchronisiert wird
    {"DspWorkspace", "TMCH0",1,eFloat,0,0, localSegment}                    // periodendauer messsignal kanal0
};

static constexpr int dm32DspWorkSpaceBase21262 = 0x82800;
static constexpr int dm32DialogWorkSpaceBase21262 = 0x83800;
static constexpr int dm32UserWorkSpaceBase21262 = 0x84800;
static constexpr int dm32CmdListBase21262 = 0x84000;
static constexpr int CmdListLen21262 = 896;
static constexpr int IntCmdListLen21262 = 128;
static constexpr int uwSpaceSize21262 = 14335;

TMemSection dm32DspWorkspace = TMemSection(
    systemSection,
    dm32DspWorkSpaceBase21262,
    sizeof(DspWorkspaceVar)/sizeof(TDspVar),
    DspWorkspaceVar);


TDspVar DialogWorkSpaceVar[] =
{
    // Parameter details for DSPCMDPAR are found at
    // https://github.com/ZeraGmbH/SHARC-DSP-Software/blob/f4003f707849076a91010435994aa61bf9e6cfb9/NewGen32.asm#L352
    {"DialogWorkSpace", "DSPCMDPAR",10,eInt,0,0, localSegment},             // 10 werte cmds, paramter ... ctrl -> dsp
    {"DialogWorkSpace", "DSPACK",1,eInt,0,0, localSegment},                 // semaphore ackn. dsp -> cntr.
    {"DialogWorkSpace", "CTRLCMDPAR",20,eInt,0,0, localSegment},            // 20 werte cmds, paramter ... dsp -> ctrl
    {"DialogWorkSpace", "CTRLACK",1,eInt,0,0, localSegment},                // semaphore ackn. ctrl. -> dsp
    {"DialogWorkSpace", "FREQUENCYNORM",4,eFloat,0,0, localSegment},        // 4 freq. normierungswerte
    {"DialogWorkSpace", "GAINCORRECTION",32,eFloat,0,0, localSegment},      // 32 verstärkungskorrekturwerte
    {"DialogWorkSpace", "PHASECORRECTION",32,eFloat,0,0, localSegment},     // 32 phasenkorrekturwerte
    {"DialogWorkSpace", "OFFSETCORRECTION",32,eFloat,0,0, localSegment},    // 32 offsetkorrekturwerte
    {"DialogWorkSpace", "BUSYMAX",1,eFloat,0,0, localSegment},              // akuelle auslastung [%]
    {"DialogWorkSpace", "BUSY",1,eFloat,0,0, localSegment},                 // max. auslastung seit reset
    {"DialogWorkSpace", "VNR",1,eFloat,0,0, localSegment},                  // versionsnummer
    {"DialogWorkSpace", "GAINCORRECTION2",32,eFloat,0,0, localSegment},     // 32 verstärkungskorrekturwerte 2.stufe
    {"DialogWorkSpace", "PHASECORRECTION2",32,eFloat,0,0, localSegment},    // 32 phasenkorrekturwerte 2. stufe
    {"DialogWorkSpace", "OFFSETCORRECTION2",32,eFloat,0,0, localSegment},   // 32 offsetkorrekturwerte 2. stufe
    {"DialogWorkSpace", "ETHDESTSOURCEADRESS",3,eInt,0,0, localSegment},    // 3*32bit -> 2*48bit
    {"DialogWorkSpace", "ETHPRIORITYTAGGED",1,eInt,0,0, localSegment},
    {"DialogWorkSpace", "ETHTYPEAPPID",1,eInt,0,0, localSegment},
    {"DialogWorkSpace", "ETHROUTINGTAB",16,eInt,0,0, localSegment},         // 8*4 = 2Byte/routing, max. 32 routings = 64byte = 16worte
    {"DialogWorkSpace", "INTERRUPTERROR",1,eInt,0,0, localSegment},
    {"DialogWorkSpace", "POWVALS4FOUT",48,eFloat,0,0, localSegment},        // 48 leistungs werte für frequenzausgänge
    {"DialogWorkSpace", "SUBDC",1,eInt,0,0, localSegment}};                 // 32 bit 1/kanal wenn gesetzt -> subdc wenn copydata, copydiff

TMemSection dm32DialogWorkSpace = TMemSection(
    systemSection,
    dm32DialogWorkSpaceBase21262,
    sizeof(DialogWorkSpaceVar)/sizeof(TDspVar),
    DialogWorkSpaceVar);


TDspVar UserWorkSpaceVar[] =
{
    {"UserWorkSpace", "UWSPACE",uwSpaceSize21262,eFloat,0,0, localSegment}
};

TMemSection dm32UserWorkSpace = TMemSection(
    systemSection,
    dm32UserWorkSpaceBase21262,
    sizeof(UserWorkSpaceVar)/sizeof(TDspVar),
    UserWorkSpaceVar);


TDspVar CmdListVar[] =
{
    {"CmdList", "INTCMDLIST",IntCmdListLen21262,eInt,0,0, localSegment},    // interrupt kommando
    {"CmdList", "CMDLIST",CmdListLen21262,eInt,0,0, localSegment},          // cycl. kommando liste
    {"CmdList", "ALTINTCMDLIST",IntCmdListLen21262,eInt,0,0, localSegment}, // alternative kommando listen
    {"CmdList", "ALTCMDLIST",CmdListLen21262,eInt,0,0, localSegment}};

TMemSection dm32CmdList = TMemSection(
    systemSection,
    dm32CmdListBase21262,
    sizeof(CmdListVar)/sizeof(TDspVar),
    CmdListVar);


TDspVar ChannelNr[] =
{
    {"ChannelNr", "CH0",1,eInt,0,0,localSegment}, {"ChannelNr", "CH1",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH2",1,eInt,0,0,localSegment}, {"ChannelNr", "CH3",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH4",1,eInt,0,0,localSegment}, {"ChannelNr", "CH5",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH6",1,eInt,0,0,localSegment}, {"ChannelNr", "CH7",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH8",1,eInt,0,0,localSegment}, {"ChannelNr", "CH9",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH10",1,eInt,0,0,localSegment},{"ChannelNr", "CH11",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH12",1,eInt,0,0,localSegment},{"ChannelNr", "CH13",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH14",1,eInt,0,0,localSegment},{"ChannelNr", "CH15",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH16",1,eInt,0,0,localSegment},{"ChannelNr", "CH17",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH18",1,eInt,0,0,localSegment},{"ChannelNr", "CH19",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH20",1,eInt,0,0,localSegment},{"ChannelNr", "CH21",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH22",1,eInt,0,0,localSegment},{"ChannelNr", "CH23",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH24",1,eInt,0,0,localSegment},{"ChannelNr", "CH25",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH26",1,eInt,0,0,localSegment},{"ChannelNr", "CH27",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH28",1,eInt,0,0,localSegment},{"ChannelNr", "CH29",1,eInt,0,0,localSegment},
    {"ChannelNr", "CH30",1,eInt,0,0,localSegment},{"ChannelNr", "CH31",1,eInt,0,0,localSegment}};

TMemSection symbConsts1 = TMemSection(
    systemSection,
    0,
    sizeof(ChannelNr)/sizeof(TDspVar),
    ChannelNr);

QHash<QString, sDspCmd*> DspStaticData::m_dspCmdHash;
QHash<QString, TDspVar*> DspStaticData::m_varHash;

sDspCmd *DspStaticData::findDspCmd(const QString &cmdName)
{
    fillCmdHashOn1stCall();
    QHash<QString, sDspCmd*>::const_iterator iter = m_dspCmdHash.constFind(cmdName);
    if(iter != m_dspCmdHash.cend())
        return iter.value();
    return nullptr;
}

const QHash<QString, TDspVar *> &DspStaticData::getVarHash()
{
    fillMemSectionHashOn1stCall();
    return m_varHash;
}

void DspStaticData::fillCmdHashOn1stCall()
{
    if(m_dspCmdHash.isEmpty())
        for(int i=0; i<sizeof(DspCmd)/sizeof(sDspCmd); i++)
            m_dspCmdHash[DspCmd[i].Name] = &DspCmd[i];
}

void DspStaticData::fillMemSectionHashOn1stCall()
{
    if(m_varHash.isEmpty()) {
        QList<TMemSection*> fixedSectionList;
        fixedSectionList.append(&dm32DspWorkspace);
        fixedSectionList.append(&dm32DialogWorkSpace);
        fixedSectionList.append(&dm32UserWorkSpace);
        fixedSectionList.append(&dm32CmdList);
        fixedSectionList.append(&symbConsts1);
        for(TMemSection* memSection : qAsConst(fixedSectionList)) {
            initMemsection(memSection);
            for (int i=0; i<memSection->DspVar.count(); i++)
                m_varHash[memSection->DspVar[i].Name] = &(memSection->DspVar[i]);
        }
    }
}

void DspStaticData::initMemsection(TMemSection *memSection)
{
    if (memSection->Section == systemSection) { // wir initialisieren nur system sections
        long offs = 0;
        for (int i = 0; i< (memSection->DspVar.count()); i++) {
            memSection->DspVar[i].offs = offs;
            memSection->DspVar[i].adr = memSection->StartAdr + offs;
            offs += memSection->DspVar[i].size;
        }
    }
}


DspCmdWithParamsRaw::DspCmdWithParamsRaw(const unsigned short CMD) // nur befehl 16bit
{
   w[0]=CMD<<16;w[1]=0; // eigentlich....aber
}

DspCmdWithParamsRaw::DspCmdWithParamsRaw(const unsigned short CMD,const unsigned short P1) // befehl 16bit und 16bit uint
{
    w[0]=CMD<<16;w[1]=P1;
}    

DspCmdWithParamsRaw::DspCmdWithParamsRaw(const unsigned short CMD, const unsigned long P1) // befehl und 32bit uint
{
     w[0]=CMD<<16;w[1]=P1;
}

DspCmdWithParamsRaw::DspCmdWithParamsRaw(const unsigned short CMD, const unsigned short P1,const unsigned short P2,const unsigned short P3) // befehl und 3x  16bit uint
{
    w[0]=(CMD<<16)+P1;w[1]=(P2<<16)+P3;
}    
 
DspCmdWithParamsRaw::DspCmdWithParamsRaw(const unsigned short CMD, const unsigned short P1,const unsigned short P2) // befehl und 2x  16bit uint
{
    w[0]=CMD<<16;w[1]=(P1<<16)+P2;
}    

DspCmdWithParamsRaw::DspCmdWithParamsRaw(const unsigned short CMD, const unsigned short P1,const unsigned long P2) // befehl und 1x  16bit uint und 1x 32bit uint
{
    w[0]=(CMD<<16)+P1;w[1]=P2;
}
