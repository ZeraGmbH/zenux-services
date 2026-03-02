#include "dspapi.h"
#include "dspmemorysectioninternal.h"
#include "dspcmdextrachecks.h"
#include <QHash>

// Details found at [1]
// [1] https://github.com/ZeraGmbH/SHARC-DSP-Software/blob/main/NewGen32.asm#L2603

static DspCmdDecodingDetails DspCmd[79] =

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
{"AVERAGE1", 15, CMD3i16, 0, DspCmdExtraChecks::AVERAGE1 },
{"INTEGRAL", 16, CMD3i16, 0 },
{"ADDVVV", 17, CMD3i16, 0 },
{"COPYUD", 18, CMD3i16, 0 },
{"COPYDU", 19, CMD3i16, 0 },
{"CMPAVERAGE1", 20, CMD3i16, 0 },
{"CLEARN", 21, CMD2i16, 0, DspCmdExtraChecks::CLEARN },
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
{"COPYMEM", 72, CMD3i16, 0, DspCmdExtraChecks::COPYMEM },
{"GENADR",73,CMD3i16,0 },
{"COPYDATAWDC", 74, CMD3i16, 0 },
{"INTEGRALPOS", 75, CMD3i16, 0 },
{"INTEGRALNEG", 76, CMD3i16, 0 },
{"SUBNVC", 77, CMD3i16, 0 },
{"XCOPYMEM", 78, CMD3i16, 0 }
};

#define DSP_VAR_COUNT(VAR_ARRAY) sizeof(VAR_ARRAY) / sizeof(DspVarInServer)

static const DspVarInServer DspWorkspaceVar[] =
{
    {"DspWorkspace", "FREQENCY",1,dspDataTypeFloat,0,0, dspInternalSegment},                // 1 wert gemessene frequenz
    {"DspWorkspace", "FREQUENCYVALUE",4,dspDataTypeFloat,0,0, dspInternalSegment},          // 4 werte f. freq. ausgänge
    {"DspWorkspace", "MAXIMUMSAMPLE",32,dspDataTypeFloat,0,0, dspInternalSegment},          // 32 werte maximumspeicher
    {"DspWorkspace", "FREQUENCYVALUEFILTER",4,dspDataTypeFloat,0,0, dspInternalSegment},    // 4 gefilterte freq. ausgänge
    {"DspWorkspace", "FREQUENCYSCALE",4,dspDataTypeFloat,0,0, dspInternalSegment},          // 4 freq. skalierungswerte
    {"DspWorkspace", "KREISFREQKOEFF",1,dspDataTypeFloat,0,0, dspInternalSegment},          // kreisfrequenz korrektur koeffizient
    {"DspWorkspace", "ETHERRORS",1,dspDataTypeInt,0,0, dspInternalSegment},                 // ethernet fehler speicher
    {"DspWorkspace", "ETHSYNCLOSTCOUNT",1,dspDataTypeInt,0,0, dspInternalSegment},          // ethernet synclost counter
    {"DspWorkspace", "ETHDATACOUNT",2,dspDataTypeInt,0,0, dspInternalSegment},              // anzahl sync. verluste mu
    {"DspWorkspace", "NCHANNELS",1,dspDataTypeInt,0,0, dspInternalSegment},                 // anzahl messkanäle sampling system
    {"DspWorkspace", "NSMEAS",1,dspDataTypeInt,0,0, dspInternalSegment},                    // anzahl samples für 1 messperiode
    {"DspWorkspace", "NSPERIOD",1,dspDataTypeInt,0,0, dspInternalSegment},                  // anzahl samples für eine signalperiode
    {"DspWorkspace", "_NSPERIOD",1,dspDataTypeFloat,0,0, dspInternalSegment},               // 1/ anzahl samples
    {"DspWorkspace", "SYNCASDU",1,dspDataTypeInt,0,0, dspInternalSegment},                  // ob und auf welchen datensatz synchronisiert wird
    {"DspWorkspace", "TMCH0",1,dspDataTypeFloat,0,0, dspInternalSegment}                    // periodendauer messsignal kanal0
};

DspMemorySectionInternal dm32DspWorkspace = DspMemorySectionInternal(0, DSP_VAR_COUNT(DspWorkspaceVar), DspWorkspaceVar);


static const DspVarInServer DialogWorkSpaceVar[] =
{
    // Parameter details for DSPCMDPAR are found at
    // https://github.com/ZeraGmbH/SHARC-DSP-Software/blob/f4003f707849076a91010435994aa61bf9e6cfb9/NewGen32.asm#L352
    {"DialogWorkSpace", "DSPCMDPAR",10,dspDataTypeInt,0,0, dspInternalSegment},             // 10 werte cmds, paramter ... ctrl -> dsp
    {"DialogWorkSpace", "DSPACK",1,dspDataTypeInt,0,0, dspInternalSegment},                 // semaphore ackn. dsp -> cntr.
    {"DialogWorkSpace", "CTRLCMDPAR",20,dspDataTypeInt,0,0, dspInternalSegment},            // 20 werte cmds, paramter ... dsp -> ctrl
    {"DialogWorkSpace", "CTRLACK",1,dspDataTypeInt,0,0, dspInternalSegment},                // semaphore ackn. ctrl. -> dsp
    {"DialogWorkSpace", "FREQUENCYNORM",4,dspDataTypeFloat,0,0, dspInternalSegment},        // 4 freq. normierungswerte
    {"DialogWorkSpace", "GAINCORRECTION",32,dspDataTypeFloat,0,0, dspInternalSegment},      // 32 verstärkungskorrekturwerte
    {"DialogWorkSpace", "PHASECORRECTION",32,dspDataTypeFloat,0,0, dspInternalSegment},     // 32 phasenkorrekturwerte
    {"DialogWorkSpace", "OFFSETCORRECTION",32,dspDataTypeFloat,0,0, dspInternalSegment},    // 32 offsetkorrekturwerte
    {"DialogWorkSpace", "BUSYMAX",1,dspDataTypeFloat,0,0, dspInternalSegment},              // akuelle auslastung [%]
    {"DialogWorkSpace", "BUSY",1,dspDataTypeFloat,0,0, dspInternalSegment},                 // max. auslastung seit reset
    {"DialogWorkSpace", "VNR",1,dspDataTypeFloat,0,0, dspInternalSegment},                  // versionsnummer
    {"DialogWorkSpace", "GAINCORRECTION2",32,dspDataTypeFloat,0,0, dspInternalSegment},     // 32 verstärkungskorrekturwerte 2.stufe
    {"DialogWorkSpace", "PHASECORRECTION2",32,dspDataTypeFloat,0,0, dspInternalSegment},    // 32 phasenkorrekturwerte 2. stufe
    {"DialogWorkSpace", "OFFSETCORRECTION2",32,dspDataTypeFloat,0,0, dspInternalSegment},   // 32 offsetkorrekturwerte 2. stufe
    {"DialogWorkSpace", "ETHDESTSOURCEADRESS",3,dspDataTypeInt,0,0, dspInternalSegment},    // 3*32bit -> 2*48bit
    {"DialogWorkSpace", "ETHPRIORITYTAGGED",1,dspDataTypeInt,0,0, dspInternalSegment},
    {"DialogWorkSpace", "ETHTYPEAPPID",1,dspDataTypeInt,0,0, dspInternalSegment},
    {"DialogWorkSpace", "ETHROUTINGTAB",16,dspDataTypeInt,0,0, dspInternalSegment},         // 8*4 = 2Byte/routing, max. 32 routings = 64byte = 16worte
    {"DialogWorkSpace", "INTERRUPTERROR",1,dspDataTypeInt,0,0, dspInternalSegment},
    {"DialogWorkSpace", "POWVALS4FOUT",48,dspDataTypeFloat,0,0, dspInternalSegment},        // 48 leistungs werte für frequenzausgänge
    {"DialogWorkSpace", "SUBDC",1,dspDataTypeInt,0,0, dspInternalSegment}};                 // 32 bit 1/kanal wenn gesetzt -> subdc wenn copydata, copydiff

DspMemorySectionInternal dm32DialogWorkSpace = DspMemorySectionInternal(0, DSP_VAR_COUNT(DialogWorkSpaceVar), DialogWorkSpaceVar);


static const DspVarInServer UserWorkSpaceVar[] =
{
    {"UserWorkSpace", "UWSPACE", 0, dspDataTypeFloat,0,0, dspInternalSegment}
};

DspMemorySectionInternal dm32UserWorkSpace = DspMemorySectionInternal(0, DSP_VAR_COUNT(UserWorkSpaceVar), UserWorkSpaceVar);


static const DspVarInServer CmdListVar[] =
{
    {"CmdList", "INTCMDLIST", 0, dspDataTypeInt, 0, 0, dspInternalSegment},    // interrupt kommando
    {"CmdList", "CMDLIST", 0, dspDataTypeInt, 0, 0, dspInternalSegment},       // cycl. kommando liste
    {"CmdList", "ALTINTCMDLIST", 0, dspDataTypeInt, 0, 0, dspInternalSegment}, // alternative kommando listen
    {"CmdList", "ALTCMDLIST", 0, dspDataTypeInt, 0, 0, dspInternalSegment}};

DspMemorySectionInternal dm32CmdList = DspMemorySectionInternal(0, DSP_VAR_COUNT(CmdListVar), CmdListVar);


static const DspVarInServer ChannelNr[] =
{
    {"ChannelNr", "CH0",1,dspDataTypeInt,0,0,dspInternalSegment}, {"ChannelNr", "CH1",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH2",1,dspDataTypeInt,0,0,dspInternalSegment}, {"ChannelNr", "CH3",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH4",1,dspDataTypeInt,0,0,dspInternalSegment}, {"ChannelNr", "CH5",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH6",1,dspDataTypeInt,0,0,dspInternalSegment}, {"ChannelNr", "CH7",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH8",1,dspDataTypeInt,0,0,dspInternalSegment}, {"ChannelNr", "CH9",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH10",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH11",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH12",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH13",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH14",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH15",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH16",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH17",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH18",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH19",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH20",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH21",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH22",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH23",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH24",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH25",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH26",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH27",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH28",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH29",1,dspDataTypeInt,0,0,dspInternalSegment},
    {"ChannelNr", "CH30",1,dspDataTypeInt,0,0,dspInternalSegment},{"ChannelNr", "CH31",1,dspDataTypeInt,0,0,dspInternalSegment}};

DspMemorySectionInternal symbConsts1 = DspMemorySectionInternal(0, DSP_VAR_COUNT(ChannelNr), ChannelNr);


QHash<QString, DspCmdDecodingDetails*> DspStaticData::m_dspAvailableCmds;
QHash<QString, DspVarServerPtr> DspStaticData::m_varHash;

DspCmdDecodingDetails *DspStaticData::findDspCmd(const QString &cmdName)
{
    fillCmdHashOn1stCall();
    QHash<QString, DspCmdDecodingDetails*>::const_iterator iter = m_dspAvailableCmds.constFind(cmdName);
    if(iter != m_dspAvailableCmds.cend())
        return iter.value();
    return nullptr;
}

const QHash<QString, DspVarServerPtr> &DspStaticData::getVarHash()
{
    fillMemSectionHashOn1stCall();
    return m_varHash;
}

void DspStaticData::clearFor1stCall()
{
    m_varHash.clear();
    m_dspAvailableCmds.clear();
}

void DspStaticData::fillCmdHashOn1stCall()
{
    if(m_dspAvailableCmds.isEmpty())
        for(int i=0; i<sizeof(DspCmd)/sizeof(DspCmdDecodingDetails); i++)
            m_dspAvailableCmds[DspCmd[i].Name] = &DspCmd[i];
}

void DspStaticData::fillMemSectionHashOn1stCall()
{
    if(m_varHash.isEmpty()) {
        QList<DspMemorySectionInternal*> fixedSectionList;
        fixedSectionList.append(&dm32DspWorkspace);
        fixedSectionList.append(&dm32DialogWorkSpace);
        fixedSectionList.append(&dm32UserWorkSpace);
        fixedSectionList.append(&dm32CmdList);
        fixedSectionList.append(&symbConsts1);
        for(DspMemorySectionInternal* memSection : qAsConst(fixedSectionList)) {
            initMemsection(memSection);
            for (int i=0; i<memSection->getVarCount(); i++) {
                DspVarServerPtr dspVar = memSection->getDspVar(i);
                m_varHash[dspVar->Name] = dspVar;
            }
        }
    }
}

void DspStaticData::setInitialVariableSize(DspMemorySectionInternal &memSection, const QString variableName, int size)
{
    for (int i=0; i<memSection.getVarCount(); ++i) {
        DspVarServerPtr dspVar = memSection.getDspVar(i);
        if (dspVar->Name == variableName) {
            dspVar->size = size;
            return;
        }
    }
    qCritical("setInitialVariableSize: Variable %s not found", qPrintable(variableName));
}

void DspStaticData::initMemsection(DspMemorySectionInternal *memSection)
{
    long offs = 0;
    for (int i = 0; i< (memSection->getVarCount()); i++) {
        DspVarServerPtr dspVar = memSection->getDspVar(i);
        if (dspVar->segment == dspInternalSegment) { // initialize only dsp system variables
            dspVar->offs = offs;
            dspVar->adr = memSection->m_startAddress + offs;
            offs += dspVar->size;
        }
    }
}

static constexpr int dm32UserWorkSpaceGlobal21262 = 0x87000;
static constexpr int dm32DspWorkSpaceBase21262 = 0x82800;
static constexpr int dm32DialogWorkSpaceBase21262 = 0x83800;
static constexpr int uwSpaceSize21262 = 14335;
static constexpr int dm32UserWorkSpaceBase21262 = 0x84800;
static constexpr int dm32CmdListBase21262 = 0x84000;
static constexpr int IntCmdListLen21262 = 128;
static constexpr int CmdListLen21262 = 896;

static constexpr int dm32UserWorkSpaceGlobal21362 = 0x9F000;
static constexpr int dm32DspWorkSpaceBase21362 = 0xE0800;
static constexpr int dm32DialogWorkSpaceBase21362 = 0xE1800;
static constexpr int dm32UserWorkSpaceBase21362 = 0x98180;
static constexpr int dm32CmdListBase21362 = 0xE2000;
static constexpr int CmdListLen21362 = 3584;
static constexpr int IntCmdListLen21362 = 512;
static constexpr int uwSpaceSize21362 = 32383;

int DspStaticData::alignInternalMemRegionsFor21262()
{
    dm32DspWorkspace.m_startAddress = dm32DspWorkSpaceBase21262;
    dm32DialogWorkSpace.m_startAddress = dm32DialogWorkSpaceBase21262;
    dm32UserWorkSpace.m_startAddress = dm32UserWorkSpaceBase21262;
    dm32CmdList.m_startAddress = dm32CmdListBase21262;

    setInitialVariableSize(dm32UserWorkSpace, "UWSPACE", uwSpaceSize21262);
    setInitialVariableSize(dm32CmdList, "INTCMDLIST", IntCmdListLen21262);
    setInitialVariableSize(dm32CmdList, "CMDLIST", CmdListLen21262);
    setInitialVariableSize(dm32CmdList, "ALTINTCMDLIST", IntCmdListLen21262);
    setInitialVariableSize(dm32CmdList, "ALTCMDLIST", CmdListLen21262);

    return dm32UserWorkSpaceGlobal21262;
}

int DspStaticData::alignInternalMemRegionsFor21362()
{
    dm32DspWorkspace.m_startAddress = dm32DspWorkSpaceBase21362;
    dm32DialogWorkSpace.m_startAddress = dm32DialogWorkSpaceBase21362;
    dm32UserWorkSpace.m_startAddress = dm32UserWorkSpaceBase21362;
    dm32CmdList.m_startAddress = dm32CmdListBase21362;

    setInitialVariableSize(dm32UserWorkSpace, "UWSPACE", uwSpaceSize21362);
    setInitialVariableSize(dm32CmdList, "INTCMDLIST", IntCmdListLen21362);
    setInitialVariableSize(dm32CmdList, "CMDLIST", CmdListLen21362);
    setInitialVariableSize(dm32CmdList, "ALTINTCMDLIST", IntCmdListLen21362);
    setInitialVariableSize(dm32CmdList, "ALTCMDLIST", CmdListLen21362);

    return dm32UserWorkSpaceGlobal21362;
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

