#include "procinterruptdecoder.h"
#include "systeminfofilelocator.h"
#include <QFile>

InterruptValues ProcInterruptDecoder::getCurrentInterrupts()
{
    QStringList procInterruptInfoLines = getInterruptInfo().split("\n");
    InterruptValues currentInterrupts;
    for(const QString &line : procInterruptInfoLines) {
        if(line.contains("zFPGA1dsp1")) {
            currentInterrupts.fpgaInterrupts = decodeSingleInterruptValue(line);
            currentInterrupts.isEmpty = false;
        }
    }
    return currentInterrupts;
}

QString ProcInterruptDecoder::getInterruptInfo()
{
    QFile file(SystemInfoFileLocator::getProcInterruptFileName());
    QString procInterruptInfo;
    if(file.open(QFile::ReadOnly))
        procInterruptInfo = file.readAll();
    else
        qWarning("Cannot read interrupt file");
    return procInterruptInfo;
}

quint32 ProcInterruptDecoder::decodeSingleInterruptValue(QString infoLine)
{
    QStringList parts = infoLine.split(" ", Qt::SkipEmptyParts);
    return parts[1].toUInt();
}
