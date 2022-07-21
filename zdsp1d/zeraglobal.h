// header datei zeraglobal.h
// hier werden global alle status-, event- usw. informationen definiert 

#ifndef ZERAGLOBAL_H
#define ZERAGLOBAL_H

// rückmeldungen für kommandos zwischen due's  und dee's (erweiterbar)
//-------------------------------------------------------------------------------------------------
#define ACKString "ack"  // acknowledge (kommando verstanden und ausgeführt)
#define NACKString "nak"  // not acknowledge (kommando nicht verstanden)
#define BUSYString "busy" // was wohl (kommando verstanden, device jedoch busy)
#define ERRCONString "errcon"  // error connection (kommando verstanden, keine verbindung zum device) 
#define ERRAUTString "erraut" // error autorisierung (kommando verstanden, client hat aber keine berechtigung)
#define ERRVALString "errval" // kommando verstanden, parameter jedoch fehlerhaft
#define ERRXMLString "errxml" // die verwendete xml datei war fehlerhaft
#define ERRMMEMString "errmmem" // fehler massenspeicher
#define ERRPATHString "errpath" // falsche pfad angabe bzw. pfad existiert nicht
#define ERREXECString "errexec" // fehler bei der ausführung eines kommandos
#define ERRTIMOString "errtimo" // timeout bei der ausführung eines kommandos

// mögliche messwertypen, werden genutzt rauszufinden ob eine vom messkanal geforderte eigenschaft verfügbar ist 
// ein bereich kann u.U. mehrere 
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
enum SenseMeasType {Uabs=1, Iabs=2, Udiff=4, Idiff=8, ECT=16}; 

// mögliche statusbits eines messkanals (erweiterbar)
//---------------------------------------------------------------------
// ein range ist RangeVirtuell wenn er nur zu linearisierungszwecken verwendet werden soll
enum SenseRangeStatus {RangeJustified=1, RangeDefect=2, RangeVirtuell=4 } ; // mehr gibt es erstmal nicht


enum SenseChannelStatus { ChannelNormalOperation=0, ChannelBusy=1, ChannelOverload=2 } ;

#endif
