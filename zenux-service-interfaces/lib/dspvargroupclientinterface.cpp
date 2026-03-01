#include "dspvargroupclientinterface.h"
#include <QIODevice>
#include <QTextStream>

int DspVarGroupClientInterface::m_instanceCount = 0;

DspVarGroupClientInterface::DspVarGroupClientInterface(const QString &groupName) :
    m_groupName(groupName)
{
    m_instanceCount++;
}

DspVarGroupClientInterface::~DspVarGroupClientInterface()
{
    for (int i = 0; i < DspVarList.size(); ++i)
        delete DspVarList.at(i);
    m_instanceCount--;
}

float* DspVarGroupClientInterface::data(QString name) // gibt einen zeiger zurück auf die var daten
{
    for (int i=0; i<DspVarList.size(); ++i) {
        DspVarClientInterface* pDspVar = DspVarList.at(i);
        if (pDspVar->Name() == name)
            return pDspVar->data();
    }
    return 0; // caller has to pay attention !!!!!
}

void DspVarGroupClientInterface::setVarData(QString datalist)
{
    const QStringList dataEntryList = datalist.split(";", Qt::SkipEmptyParts);
    for (const QString &dspVarEntry : dataEntryList) {
        QString varName = dspVarEntry.section(":",0,0);
        DspVarClientInterface* dspVar = findVar(varName);
        if(dspVar) {
            QString varValues = dspVarEntry.section(":",1,1);
            float *val = dspVar->data();
            int datatype = dspVar->datatype();
            QStringList valueList = varValues.split(",");
            for(QStringList::Iterator it2 = valueList.begin(); it2 != valueList.end(); ++it2,val++ ) {
                QString strValue = *it2;
                strValue.remove(';');
                if (datatype == dspDataTypeInt) {
                    uint vul = strValue.toUInt(); // test auf ulong
                    *((uint*) val) = vul;
                }
                else
                    *val = strValue.toFloat();
            }
        }
    }
}

DspVarClientInterface *DspVarGroupClientInterface::addDspVar(const QString &varName, int size,
                                                             DspDataType dataType, DspSegmentType dspSegmentType)
{
    DspVarClientInterface *var = new DspVarClientInterface(varName, size, dataType, dspSegmentType);
    DspVarList.append(var);
    return var;
}

QString DspVarGroupClientInterface::getName()
{
    return m_groupName;
}

quint32 DspVarGroupClientInterface::getUserMemSize()
{
    quint32 size = 0;
    for(int i = 0; i < DspVarList.size(); ++i)
        if (DspVarList.at(i)->m_dspSegmentType == moduleLocalSegment)
            size += DspVarList.at(i)->size();
    return size;
}

quint32 DspVarGroupClientInterface::getUserMemSizeGlobal()
{
    quint32 size = 0;
    for(int i = 0; i < DspVarList.size(); ++i)
        if (DspVarList.at(i)->m_dspSegmentType == moduleGlobalSegment)
            size += DspVarList.at(i)->size();
    return size;
}

QString DspVarGroupClientInterface::VarListLong()
{
    QString sReturn;
    QTextStream ts(&sReturn, QIODevice::WriteOnly);
    for(int i=0; i<DspVarList.size(); ++i) {
        const DspVarClientInterface *pDspVar = DspVarList.at(i);
        ts << QString("%1,%2,%3,%4,%5;").arg(m_groupName, pDspVar->Name()).arg(pDspVar->size()).arg(pDspVar->datatype()).arg(pDspVar->m_dspSegmentType);
    }
    return sReturn;
}

QString DspVarGroupClientInterface::VarListShort()
{
    QString sReturn;
    QTextStream ts(&sReturn, QIODevice::WriteOnly);
    for(int i=0; i<DspVarList.size(); ++i) {
        DspVarClientInterface *pDspVar = DspVarList.at(i);
        ts << QString("%1,%2;").arg(pDspVar->Name()).arg(pDspVar->size());
    }
    return sReturn;
}

QString DspVarGroupClientInterface::writeCommand()
{
    QString sReturn;
    QTextStream ts(&sReturn, QIODevice::WriteOnly );
    for(int i=0; i<DspVarList.count(); i++) {
        DspVarClientInterface* pVar = DspVarList.at(i);
        ts << pVar->Name();

        float* floatPointer = pVar->data();
        int type = pVar->datatype();

        if (type == dspDataTypeInt) {
            for (int j = 0; j < pVar->size(); j++, floatPointer++) {
                uint* lval = (uint*) floatPointer;
                ts << "," << *lval;
            }
        }
        else {
            for (int j = 0; j < pVar->size(); j++, floatPointer++)
                ts << "," << *floatPointer;
        }
        ts << ";";
    }
    return sReturn;
}

QVector<float>& DspVarGroupClientInterface::getData()
{
    vector.clear();
    for (int i=0; i<DspVarList.count(); i++) { // we fetch all data of all vars in this memory group
        DspVarClientInterface* pVar = DspVarList.at(i);
        float* fval = pVar->data();
        for (int j = 0; j < pVar->size(); j++, fval++)
            vector.append(*fval);
    }
    return vector;
}

int DspVarGroupClientInterface::getInstanceCount()
{
    return m_instanceCount;
}

const QList<DspVarClientInterface *> DspVarGroupClientInterface::getVars() const
{
    return DspVarList;
}

DspVarClientInterface *DspVarGroupClientInterface::findVar(const QString &varName)
{
    for(int i=0; i<DspVarList.size(); ++i) {
        DspVarClientInterface* pDspVar = DspVarList.at(i);
        if (pDspVar->Name() == varName)
            return pDspVar;
    }
    return nullptr;
}

void DspVarGroupClientInterface::setData(const QVector<float> &data)
{
    int valueCount = 0;
    for(DspVarClientInterface* dspVar : qAsConst(DspVarList)) {
        QVector<float> varData;
        for(int singleVal=0; singleVal<dspVar->size(); singleVal++) {
            varData.append(data[valueCount]);
            valueCount++;
        }
        dspVar->setData(varData);
    }
}
