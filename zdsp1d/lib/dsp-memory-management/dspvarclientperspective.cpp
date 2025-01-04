#include "dspvarclientperspective.h"
#include "dspvardefinitions.h"

DspVarClientPerspective::DspVarClientPerspective() :
    m_nSize(0)
{
}

bool DspVarClientPerspective::Init(const QString& varDefinition)
{
    bool ret = true;
    int commaCount = varDefinition.count(',');
    if ( commaCount > 1 ) {
        m_clientHandleName = varDefinition.section(',', 0, 0).remove(' ');
        m_sName = varDefinition.section(',', 1, 1).remove(' ').toUpper();
        bool ok;
        int varSize = varDefinition.section(',', 2, 2).remove(' ').toInt(&ok);
        if ((ret = ret && ok))
            m_nSize = varSize;

        if (commaCount > 2) { // optional type
            int varType = varDefinition.section(',', 3, 3).remove(' ').toInt(&ok);
            if ((ret = ret && ok)) {
                if ( (ret = ret && ( (varType == eInt) || (varType == eFloat) )))
                    m_nType = varType;
            }
        }
        else
            m_nType = eFloat;

        if (commaCount > 3) { // optional segment
            int varSegment = varDefinition.section(',', 4, 4).remove(' ').toInt(&ok);
            if ((ret = ret && ok)) {
                if ( (ret = ret && ( (varSegment == localSegment) || (varSegment == globalSegment) )))
                    m_nSegment = varSegment;
            }
        }
        else
            m_nSegment = localSegment;

    }
    return ret;
}

QString DspVarClientPerspective::getClientHandleName() const
{
    return m_clientHandleName;
}

QString DspVarClientPerspective::name() const
{
    return m_sName;
}

int DspVarClientPerspective::size() const
{
    return m_nSize;
}

int DspVarClientPerspective::type() const
{
    return m_nType;
}

ulong DspVarClientPerspective::offs() const
{
    return m_nOffsAdr;
}

int DspVarClientPerspective::segment() const
{
    return m_nSegment;
}

void DspVarClientPerspective::SetOffs(long o)
{
    m_nOffsAdr = o;
}
