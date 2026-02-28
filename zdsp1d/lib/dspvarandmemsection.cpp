#include "dspvarandmemsection.h"

bool TDspVar::Init(const QString& varDefinition)
{
    bool ret = true;
    int commaCount = varDefinition.count(',');
    if ( commaCount > 1 ) {
        m_clientHandleName = varDefinition.section(',', 0, 0).remove(' ');
        Name = varDefinition.section(',', 1, 1).remove(' ').toUpper();
        bool ok;
        int varSize = varDefinition.section(',', 2, 2).remove(' ').toInt(&ok);
        if ((ret = ret && ok))
            size = varSize; // int -> ushort!!!

        if (commaCount > 2) { // optional type
            int varType = varDefinition.section(',', 3, 3).remove(' ').toInt(&ok);
            if ((ret = ret && ok)) {
                if ( (ret = ret && ( (varType == dspDataTypeInt) || (varType == dspDataTypeFloat) )))
                    type = (dspDataType)varType;
            }
        }
        else
            type = dspDataTypeFloat;

        if (commaCount > 3) { // optional segment
            int varSegment = varDefinition.section(',', 4, 4).remove(' ').toInt(&ok);
            if ((ret = ret && ok)) {
                if ( (ret = ret && (
                                    varSegment == dspInternalSegment ||
                                    varSegment == moduleLocalSegment ||
                                    varSegment == moduleGlobalSegment
                                   )))
                    segment = (DspSegmentType)varSegment;
            }
        }
        else
            segment = moduleLocalSegment;

    }
    return ret;
}

QString TDspVar::toHex(int val)
{
    QString hexVal = QString("000000%1").arg(val, 0, 16).toUpper().right(6);
    return QString("0x") + hexVal;
}

TMemSection::TMemSection(long startAddress, int varCount, TDspVar *dspVars) :
    m_startAddress(startAddress),
    m_varCount(varCount),
    m_dspVars(dspVars)
{
}
