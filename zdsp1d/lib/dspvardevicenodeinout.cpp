#include "dspvardevicenodeinout.h"
#include <QStringList>
#include <QDataStream>

DspVarDeviceNodeInOut::DspVarDeviceNodeInOut(AbstractFactoryDeviceNodeDspPtr deviceNodeFactory) :
    m_deviceNodeFactory(deviceNodeFactory)
{
}

bool DspVarDeviceNodeInOut::doReadVarFromDsp(TDspVar *DspVar, int countVars, QByteArray *varRead)
{
    const int countBytes = countVars * 4;
    varRead->resize(countBytes);
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    if ((deviceNode->lseek(DspVar->adr) >= 0) &&
        (deviceNode->read(varRead->data(), countBytes) >= 0))
        return true;
    return false;
}

bool DspVarDeviceNodeInOut::doWriteDspVars(const QString &varsSemicolonSeparated, DspVarResolver *dspVarResolver)
{
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    for(const QString &varString : varEntries) {
        QByteArray byteArr;
        QDataStream stream(&byteArr, QIODevice::Unbuffered | QIODevice::ReadWrite);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

        const QStringList varNameVals = varString.split(",", Qt::SkipEmptyParts);
        if(varNameVals.count() < 2)
            return false;
        const QString &varName = varNameVals[0];

        int wordCount = 0;
        int type = dspVarResolver->getVarType(varName);
        for(int valIdx=1; valIdx<varNameVals.count(); valIdx++) {
            const QString &valueStr = varNameVals[valIdx];
            if(type == eUnknown) {
                if(!tryStreamIntegerValue(valueStr, stream) &&
                    !tryStreamFloatValue(valueStr, stream))
                    return false;
            }
            else if(type == eInt) {
                if(!tryStreamIntegerValue(valueStr, stream))
                    return false;
            }
            else
                if(!tryStreamFloatValue(valueStr, stream))
                    return false;
            wordCount++;
        }
        long adr = dspVarResolver->getVarAddress(varName);
        if (adr == -1)
            return false;
        AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
        if(wordCount>0 && !deviceNode->write(adr, byteArr.data(), wordCount*4))
            return false;
    }
    return true;
}

bool DspVarDeviceNodeInOut::tryStreamIntegerValue(const QString &strValue, QDataStream &stream)
{
    bool ok;
    qint32 lValue = strValue.toLong(&ok);
    if(ok) {
        stream << lValue;
        return true;
    }
    quint32 uValue = strValue.toULong(&ok);
    if(ok) {
        stream << uValue;
        return true;
    }
    return false;
}

bool DspVarDeviceNodeInOut::tryStreamFloatValue(const QString &strValue, QDataStream &stream)
{
    bool ok;
    float value = strValue.toFloat(&ok);
    if(ok)
        stream << value;
    return ok;
}
