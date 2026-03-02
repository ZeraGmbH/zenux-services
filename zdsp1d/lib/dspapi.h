#ifndef DSPAPI_H
#define DSPAPI_H

#include "dspmemorysectioninternal.h"
#include "dspvarresolver.h"
#include <QString>
#include <QHash>
#include <functional>

static constexpr int dm32DspWorkSpaceBase21262 = 0x82800;
static constexpr int dm32DialogWorkSpaceBase21262 = 0x83800;
static constexpr int dm32UserWorkSpaceBase21262 = 0x84800;
static constexpr int dm32UserWorkSpaceGlobal21262 = 0x87000;
static constexpr int uwSpaceSize21262 = 14335;
static constexpr int dm32CmdListBase21262 = 0x84000;
static constexpr int IntCmdListLen21262 = 128;
static constexpr int CmdListLen21262 = 896;

static constexpr int dm32DspWorkSpaceBase21362 = 0xE0800;
static constexpr int dm32DialogWorkSpaceBase21362 = 0xE1800;
static constexpr int dm32UserWorkSpaceGlobal21362 = 0x9F000;
static constexpr int dm32UserWorkSpaceBase21362 = 0x98180;
static constexpr int uwSpaceSize21362 = 32383;
static constexpr int dm32CmdListBase21362 = 0xE2000;
static constexpr int CmdListLen21362 = 3584;
static constexpr int IntCmdListLen21362 = 512;

extern DspVarServer DialogWorkSpaceVar[];

enum DspAcks {NBusy, InProgress, CmdError, ParError, CmdDone};

enum CmdType { CMD, CMD1i16, CMD2i16, CMD3i16 ,CMD1i32 , CMD1i161fi32 };
// 1fi32 kann 1 float oder 1 integer sein

#define DSP_VAR_COUNT(VAR_ARRAY) sizeof(VAR_ARRAY)/sizeof(DspVarServer)

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
    static const QHash<QString, DspVarServerPtr> &getVarHash();
    static void initMemsection(DspMemorySectionInternal* memSection);
private:
    static void fillCmdHashOn1stCall();
    static void fillMemSectionHashOn1stCall();

    static QHash<QString, DspCmdDecodingDetails*> m_dspAvailableCmds;
    static QHash<QString, DspVarServerPtr> m_varHash;
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
