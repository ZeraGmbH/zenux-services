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
                if ( (ret = ret && ( (varType == eInt) || (varType == eFloat) )))
                    type = (dType)varType;
            }
        }
        else
            type = eFloat;

        if (commaCount > 3) { // optional segment
            int varSegment = varDefinition.section(',', 4, 4).remove(' ').toInt(&ok);
            if ((ret = ret && ok)) {
                if ( (ret = ret && ( (varSegment == localSegment) || (varSegment == globalSegment) )))
                    segment = (segmentType)varSegment;
            }
        }
        else
            segment = localSegment;

    }
    return ret;
}

QString TDspVar::toHex(int val)
{
    return QString("0x%1").arg(QString("%1").arg(val, 6, 16, '0').toUpper());
}

TMemSection::TMemSection(sectionType section, long startAddress, int varCount, TDspVar *dspVars) :
    m_section(section),
    m_startAddress(startAddress),
    m_varCount(varCount),
    m_dspVars(dspVars)
{
}
