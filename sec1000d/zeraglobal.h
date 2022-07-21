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
#define ERRXMLFORMATString "errxmlformat"
#define ERRXMLDOCTYPEString "errxmldoctype"
#define ERRXMLSERIALString "errxmlserial"
#define ERRXMLVERSIONString "errxmlversion"
#define ERRXMLNODEString "errxmlnode_"
#define ERRMMEMString "errmmem" // fehler massenspeicher
#define ERRPATHString "errpath" // falsche pfad angabe bzw. pfad existiert nicht
#define ERREXECString "errexec" // fehler bei der ausführung eines kommandos


#endif
