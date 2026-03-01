#ifndef DSPAPI_H
#define DSPAPI_H

#include "dspmemorysectioninternal.h"
#include "dspvarresolver.h"
#include <QString>
#include <QHash>
#include <functional>

enum DspAcks {NBusy, InProgress, CmdError, ParError, CmdDone};

enum CmdType { CMD, CMD1i16, CMD2i16, CMD3i16 ,CMD1i32 , CMD1i161fi32 };
// 1fi32 kann 1 float oder 1 integer sein

struct DspCmdDecodingDetails {
    const char* Name; // name des befehls
    ushort CmdCode; // der zugehörige befehlscode
    CmdType CmdClass; // der typ des befehls
    char modify; // !=0 -> verändern, diese befehle erhalten die prozessnr. (fd) als parameter
    std::function<bool (const QStringList &paramNames, const short i16Params[],
                        DspVarResolver* varResolver)> m_cmdExtraCheckFunction = nullptr;
};

class DspStaticData
{
public:
    static DspCmdDecodingDetails* findDspCmd(const QString& cmdName);
    static const QHash<QString, DspVarServer*> &getVarHash();
    static void initMemsection(DspMemorySectionInternal* memSection);
private:
    static void fillCmdHashOn1stCall();
    static void fillMemSectionHashOn1stCall();

    static QHash<QString, DspCmdDecodingDetails*> m_dspAvailableCmds;
    static QHash<QString, DspVarServer*> m_varHash;
};


class DspCmdWithParamsRaw { // hält einen 64bit dsp befehl incl. parameter
public:
    DspCmdWithParamsRaw(){w[0]=0;w[1]=0;};
    DspCmdWithParamsRaw(const unsigned short); // nur befehl 16bit
    DspCmdWithParamsRaw(const unsigned short,const unsigned short); // befehl 16bit und 16bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned long); // befehl und 32bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned short,const unsigned short,const unsigned short); // befehl und 3x  16bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned short,const unsigned short); // befehl und 2x  16bit uint
    DspCmdWithParamsRaw(const unsigned short, const unsigned short,const unsigned long); // befehl und 1x  16bit uint und 1x 32bit uint ... kann auch ein float sein

    unsigned long w[2]; // ein DSP-Kommando besteht aus 64Bit
};

#endif
