// header datei dsp.h
// definitionen rund um den dsp

#ifndef DSP_H
#define DSP_H

#include <QString>
#include <QList>
#include <QDataStream>
#include <QHash>

#include "parse.h"

// enum zum lesen von dsp port adressen über ioctl
// Serial Port Interface,  Serial Interface, DSP Ctrl Register, what ?, the device name
enum IOCTL {SPI, Serial, DSPCtrl, DSPStat, DSPCfg, VersionNr, MagicId};

enum DspAcks {NBusy, InProgress, CmdError, ParError, CmdDone, CmdTimeout}; 

const int MAGIC_ID21262 = 0xAA55BB44;
const int MAGIC_ID21362 = 0xAA55CC33;
const int DSP_RUNNING = 0x80;

const int dm32DspWorkSpaceBase21262 = 0x82800;
const int dm32DspWorkSpaceBase21362 = 0xE0800;
const int dm32DialogWorkSpaceBase21262 = 0x83800;
const int dm32DialogWorkSpaceBase21362 = 0xE1800;
const int dm32UserWorkSpaceBase21262 = 0x84800;
const int dm32UserWorkSpaceBase21362 = 0x98180;
const int dm32UserWorkSpaceGlobal21262 = 0x87000;
const int dm32UserWorkSpaceGlobal21362 = 0x9F000;
const int dm32CmdListBase21262 = 0x84000;
const int dm32CmdListBase21362 = 0xE2000;

const int CmdListLen21262 = 896;
const int CmdListLen21362 = 3584;
const int IntCmdListLen21262 = 128;
const int IntCmdListLen21362 = 512;

const int uwSpaceSize21262 = 14335;
const int uwSpaceSize21362 = 32383;

enum CmdType { CMD, CMD1i16, CMD2i16, CMD3i16 ,CMD1i32 , CMD1i161fi32 };
// 1fi32 kann 1 float oder 1 integer sein

struct sDspCmd { // wird zum ausdekodieren der dsp kommandos benötigt
    const char* Name; // name des befehls
    ushort	CmdCode; // der zugehörige befehlscode
    CmdType CmdClass; // der typ des befehls
    char modify; // !=0 -> verändern, diese befehle erhalten die prozessnr. (fd) als parameter 
};
    
sDspCmd* findDspCmd(QString&);
enum dType { eInt, eFloat, eUnknown};

enum sectionType { systemSection, userSection};
enum segmentType { localSegment, globalSegment};

struct sDspVar { // dient ebenfalls der dekodierung 
    QString Name; // name der variablen
    ushort size;  // anzahl worte
    dType type; // 
    ulong adr; // die abs. adresse auf welcher sich die variable befindet
    ulong offs; // der offset innerhalb der memory section
    segmentType segment; // segment info, nur relevant für client sections
};


struct sMemSection { // beschreibt eine dsp memory section
    sectionType Section;
    long StartAdr;
    int n; // anzahl der sdspvar elemente
    sDspVar *DspVar;
};


class cDspVarResolver { // der löst die Variablen anhand der memsections und namen auf 
public:
    cDspVarResolver();
    void setVarHash();
    void addSection(sMemSection*); // sections werden im konstruktor erstmal fest eingebunden
    long offs(QString&, ulong, ulong); // gibt die offs. adresse einer variablen zurück, -1 wenn es die variable nicht gibt, zum generiren der dsp kommandos
    long adr(QString&); // gibt die abs. adresse einer variablen im dsp zurück, -1 wenn es die variable nicht gibt, zum schreiben in den dsp
    sDspVar* vadr(QString&); //  gibt einen zeiger auf sDspVar zurück, zum lesen von daten aus dem dsp
    int type(QString &s);

private:
    QHash<QString, sDspVar*> mVarHash;
    cParse VarParser;
//    sDspVar *SearchedVar; // zeiger auf die gesuchte variable;
//    long offs(QString&, sMemSection**, int *);
    void initMemsection(sMemSection* psec); // zum initialisieren einer memory section
    void setQHash(sMemSection* psec); // zum setzen der qhash
//    sMemSection *sec;
    QList<sMemSection*> MemSectionList;
};


class cDspClientVar { // zur verwaltung der dsp variablen auf client/server ebene
public:
    cDspClientVar();
    bool Init(QString&); // legt eine variable an 
    QString& name();
    void SetOffs(long);
    int size();
    int type();
    ulong offs();
    int segment();

private:    
    QString m_sName; // eine dsp variable hat einen namen
    int m_nOffsAdr; // hat eine rel. start adresse
    int m_nSize; // und eine anzahl von elementen
    int m_nType; // der typ der variablen (efloat oder eint)
    int m_nSegment; //
};


class cDspCmd { // hält einen 64bit dsp befehl incl. parameter 
public:
    cDspCmd(){w[0]=0;w[1]=0;}; 
    cDspCmd(const unsigned short); // nur befehl 16bit
    cDspCmd(const unsigned short,const unsigned short); // befehl 16bit und 16bit uint
    cDspCmd(const unsigned short, const unsigned long); // befehl und 32bit uint
    cDspCmd(const unsigned short, const unsigned short,const unsigned short,const unsigned short); // befehl und 3x  16bit uint
    cDspCmd(const unsigned short, const unsigned short,const unsigned short); // befehl und 2x  16bit uint
    cDspCmd(const unsigned short, const unsigned short,const unsigned long); // befehl und 1x  16bit uint und 1x 32bit uint ... kann auch ein float sein
//    void operator = (const cDspCmd &tc) { w[0] = tc.w[0]; w[1] = tc.w[1];};
    unsigned long w[2]; // ein DSP-Kommando besteht aus 64Bit
private:
};    

#endif    
