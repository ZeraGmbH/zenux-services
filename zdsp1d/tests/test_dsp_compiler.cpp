#include "test_dsp_compiler.h"
#include "testdspcompilersupport.h"
#include <QTest>

QTEST_MAIN(test_dsp_compiler)

void test_dsp_compiler::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_dsp_compiler::unknownCommand()
{
    DspVarResolver varResolver;
    DspCmdCompiler compiler(&varResolver, 0);

    bool ok;
    DspCmdWithParamsRaw raw = compile(compiler, "FOO", ok);
    QCOMPARE(ok, false);
    QCOMPARE(raw.w[0], 0);
    QCOMPARE(raw.w[1], 0);
}

DspCmdWithParamsRaw test_dsp_compiler::compile(DspCmdCompiler &compiler, const QString &cmdLine, bool &ok)
{
    return compiler.compileOneCmdLineAligned(cmdLine, ok, 0, 0, std::make_shared<TestDspCompilerSupport>());
}

void test_dsp_compiler::addDspVariable(DspVarResolver &resolver, const QString &varName, ushort size,
                                       dType varType, segmentType segmType, sectionType sectType)
{
    TDspVar var;
    var.Name = varName;
    var.size = size;
    var.type = varType;
}
