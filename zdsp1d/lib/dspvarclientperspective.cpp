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
    if ( commaCount > 0 ) {
        m_sName = varDefinition.section(',', 0, 0).remove(' ');
        bool ok;
        int varSize = varDefinition.section(',',1,1).remove(' ').toInt(&ok); // der erste parameter ist die feldgrösse
        if ((ret = ret && ok))
            m_nSize = varSize;

        if (commaCount > 1) { // optional type
            int varType = varDefinition.section(',', 2, 2).remove(' ').toInt(&ok);
            if ((ret = ret && ok)) {
                if ( (ret = ret && ( (varType == eInt) || (varType == eFloat) )))
                    m_nType = varType;
            }
        }
        else
            m_nType = eFloat;

        if (commaCount > 2) { // optional segment
            int varSegment = varDefinition.section(',', 3, 3).remove(' ').toInt(&ok);
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

QString& DspVarClientPerspective::name()
{
    return m_sName;
}

int DspVarClientPerspective::size()
{
    return m_nSize;
}

int DspVarClientPerspective::type()
{
    return m_nType;
}

ulong DspVarClientPerspective::offs()
{
    return m_nOffsAdr;
}

int DspVarClientPerspective::segment()
{
    return m_nSegment;
}

void DspVarClientPerspective::SetOffs(long o)
{
    m_nOffsAdr = o;
}
