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

// MUST BE IN SYNC WITH https://github.com/ZeraGmbH/SHARC-DSP-Software/blob/d326afe47f08293dfd37d376e16daa80fd654850/NewGen32.asm#L344
static constexpr int DSP_MAX_PENDING_INTERRUPT_COUNT = 20;

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

    static void initMemsection(DspMemorySectionInternal* memSection, long sectionOffsetForCopyDuCopyUd);
    static int alignInternalMemRegionsFor21262();
    static int alignInternalMemRegionsFor21362();

    static void clearFor1stCall(); // currently for tests only

private:
    static void fillCmdHashOn1stCall();
    static void fillMemSectionHashOn1stCall();
    static void setInitialVariableSize(DspMemorySectionInternal &memSection, const QString &variableName, int size);

    static QHash<QString, DspCmdDecodingDetails*> m_dspAvailableCmds;
    static QHash<QString, DspVarServerPtr> m_varHash;
};


class DspCmdWithParamsCompiled { // hält einen 64bit dsp befehl incl. parameter
public:
    DspCmdWithParamsCompiled(){w[0]=0;w[1]=0;};
    DspCmdWithParamsCompiled(const unsigned short); // nur befehl 16bit
    DspCmdWithParamsCompiled(const unsigned short,const unsigned short); // befehl 16bit und 16bit uint
    DspCmdWithParamsCompiled(const unsigned short, const unsigned long); // befehl und 32bit uint
    DspCmdWithParamsCompiled(const unsigned short, const unsigned short,const unsigned short,const unsigned short); // befehl und 3x  16bit uint
    DspCmdWithParamsCompiled(const unsigned short, const unsigned short,const unsigned short); // befehl und 2x  16bit uint
    DspCmdWithParamsCompiled(const unsigned short, const unsigned short,const unsigned long); // befehl und 1x  16bit uint und 1x 32bit uint ... kann auch ein float sein

    unsigned long w[2]; // ein DSP-Kommando besteht aus 64Bit
};

#endif
