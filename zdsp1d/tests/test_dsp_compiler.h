#ifndef TEST_DSP_COMPILER_H
#define TEST_DSP_COMPILER_H

#include "dspcmdcompiler.h"
#include <QObject>

class test_dsp_compiler : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void unknownCommand();

private:
    DspCmdWithParamsRaw compile(DspCmdCompiler &compiler, const QString &cmdLine, bool &ok);
    void addDspVariable(DspVarResolver &resolver, const QString &varName, ushort size,
                        dType varType=eInt, segmentType segmType=localSegment, sectionType sectType=userSection);
};

#endif // TEST_DSP_COMPILER_H
