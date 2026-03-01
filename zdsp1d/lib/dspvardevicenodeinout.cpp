#include "dspvardevicenodeinout.h"
#include "zscpi_response_definitions.h"
#include <QIODevice>
#include <QStringList>
#include <QDataStream>
#include <QTextStream>

DspVarDeviceNodeInOut::DspVarDeviceNodeInOut(AbstractFactoryZdspSupportPtr zdspSupportFactory) :
    m_zdspSupportFactory(zdspSupportFactory)
{
}

DspVarServer *DspVarDeviceNodeInOut::readOneDspVar(const QString &nameCommaLen,
                                              QByteArray *varRead,
                                              DspVarResolver *dspVarResolver)
{
    const QStringList listNameLen = nameCommaLen.split(",", Qt::SkipEmptyParts);
    if(listNameLen.count() < 2)
        return nullptr; // wrong parameter format
    QString name = listNameLen[0];
    DspVarServer *dspVar = dspVarResolver->getDspVar(name);
    if (!dspVar)
        return nullptr; // fehler, den namen gibt es nicht

    QString countSection = listNameLen[1];
    bool ok;
    const int countVars = countSection.toInt(&ok);
    if (!ok || countVars < 1 )
        return nullptr; // fehler in der anzahl der elemente

    DspVarDeviceNodeInOut dspInOut(m_zdspSupportFactory);
    if(dspInOut.readVarFromDsp(dspVar, countVars, varRead))
        return dspVar;
    return nullptr;
}

bool DspVarDeviceNodeInOut::readOneDspVarInt(const QString &varName, int &intval, DspVarResolver *dspVarResolver)
{ // einen int (32bit) wert lesen
    bool ret = false;
    QByteArray ba;
    QString ss = QString("%1,1").arg(varName);
    DspVarDeviceNodeInOut dspInOut(m_zdspSupportFactory);
    if(dspInOut.readOneDspVar(ss, &ba, dspVarResolver)) {
        // 1 wort ab name (s) lesen
        intval = *((int*) (ba.data()));
        ret = true;
    }
    return ret;
}

QString DspVarDeviceNodeInOut::readDspVarList(const QString &variablesString, DspVarResolver *dspVarResolver)
{
    QString ret;
    QTextStream ts(&ret, QIODevice::WriteOnly);
    ts.setRealNumberPrecision(8);
    QByteArray ba;
    const QStringList varEntries = variablesString.split(";", Qt::SkipEmptyParts);
    for(int i=0; i<varEntries.count(); i++) {
        QString nameCommaLen = varEntries[i]; // format '<name>,<count>'
        DspVarServer *dspVar = readOneDspVar(nameCommaLen, &ba, dspVarResolver);
        if(!dspVar)
            return ZSCPI::scpiAnswer[ZSCPI::errexec];

        int n = ba.size() / 4;
        ts << dspVar->Name << ":";
        switch(dspVar->type)
        {
        case dspDataTypeInt :
        {
            ulong *ul = (ulong*) ba.data();
            for (int j = 0; j < n-1; j++,ul++)
                ts << (*ul) << "," ;
            ts << *ul << ";" ;
            break;
        }
        case dspDataTypeUnknown:
        case dspDataTypeFloat :
        {
            float *f = (float*) ba.data();
            for (int j = 0; j < n-1; j++,f++)
                ts << (*f) << "," ;
            ts << *f << ";" ;
            break;}
        }
    }
    return ret;
}

bool DspVarDeviceNodeInOut::readVarFromDsp(DspVarServer *DspVar, int countVars, QByteArray *varRead)
{
    const int countBytes = countVars * 4;
    varRead->resize(countBytes);
    AbstractDspDeviceNodePtr deviceNode = m_zdspSupportFactory->getDspDeviceNode();
    if ((deviceNode->lseek(DspVar->adr) >= 0) &&
        (deviceNode->read(varRead->data(), countBytes) >= 0))
        return true;
    return false;
}

bool DspVarDeviceNodeInOut::writeDspVars(const QString &varsSemicolonSeparated, DspVarResolver *dspVarResolver)
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
            if(type == dspDataTypeUnknown) {
                if(!tryStreamIntegerValue(valueStr, stream) &&
                    !tryStreamFloatValue(valueStr, stream))
                    return false;
            }
            else if(type == dspDataTypeInt) {
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
        AbstractDspDeviceNodePtr deviceNode = m_zdspSupportFactory->getDspDeviceNode();
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
