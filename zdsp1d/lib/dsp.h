#ifndef DSP_H
#define DSP_H

#include <QHash>

enum DspAcks {NBusy, InProgress, CmdError, ParError, CmdDone};

enum CmdType { CMD, CMD1i16, CMD2i16, CMD3i16 ,CMD1i32 , CMD1i161fi32 };
// 1fi32 kann 1 float oder 1 integer sein

struct sDspCmd { // wird zum ausdekodieren der dsp kommandos benötigt
    const char* Name; // name des befehls
    ushort	CmdCode; // der zugehörige befehlscode
    CmdType CmdClass; // der typ des befehls
    char modify; // !=0 -> verändern, diese befehle erhalten die prozessnr. (fd) als parameter 
};
    
sDspCmd* findDspCmd(const QString& cmdName);

class DspCmdWithParamsRaw { // hält einen 64bit dsp befehl incl. parameter
public:
    DspCmdWithParamsRaw(){w[0]=0;w[1]=0;};
    DspCmdWithParamsRaw(const unsigned short); // nur befehl 16bit
    DspCmdWithParamsRaw(const unsigned short,const unsigned short); // befehl 16bit und 16bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned long); // befehl und 32bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned short,const unsigned short,const unsigned short); // befehl und 3x  16bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned short,const unsigned short); // befehl und 2x  16bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned short,const unsigned long); // befehl und 1x  16bit uint und 1x 32bit uint ... kann auch ein float sein
//    void operator = (const DspCmdWithParamsRaw &tc) { w[0] = tc.w[0]; w[1] = tc.w[1];};
    unsigned long w[2]; // ein DSP-Kommando besteht aus 64Bit
private:
};    

#endif    
