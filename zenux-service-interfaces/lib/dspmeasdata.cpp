#include "dspmeasdata.h"
#include <QIODevice>
#include <QTextStream>

int cDspMeasData::m_instanceCount = 0;

cDspMeasData::cDspMeasData(QString name) :
    m_handleName(name)
{
    m_instanceCount++;
}

cDspMeasData::~cDspMeasData()
{
    for (int i = 0; i < DspVarList.size(); ++i)
        delete DspVarList.at(i);
    m_instanceCount--;
}

float* cDspMeasData::data(QString name) // gibt einen zeiger zurück auf die var daten
{
    for (int i=0; i<DspVarList.size(); ++i) {
        cDspVar* pDspVar = DspVarList.at(i);
        if (pDspVar->Name() == name)
            return pDspVar->data();
    }
    return 0; // caller has to pay attention !!!!!
}

void cDspMeasData::setVarData(QString datalist)
{
    const QStringList dataEntryList = datalist.split(";", Qt::SkipEmptyParts);
    for (const QString &dspVarEntry : dataEntryList) {
        QString varName = dspVarEntry.section(":",0,0);
        cDspVar* dspVar = findVar(varName);
        if(dspVar) {
            QString varValues = dspVarEntry.section(":",1,1);
            float *val = dspVar->data();
            int datatype = dspVar->datatype();
            QStringList valueList = varValues.split(",");
            for(QStringList::Iterator it2 = valueList.begin(); it2 != valueList.end(); ++it2,val++ ) {
                QString strValue = *it2;
                strValue.remove(';');
                if (datatype == DSPDATA::dInt) {
                    uint vul = strValue.toUInt(); // test auf ulong
                    *((uint*) val) = vul;
                }
                else
                    *val = strValue.toFloat();
            }
        }
    }
}

cDspVar *cDspMeasData::addDspVar(const QString &name, int size, int type, int datatype)
{
    cDspVar *var = new cDspVar(name, size, type, datatype);
    DspVarList.append(var);
    return var;
}

QString cDspMeasData::getName()
{
    return m_handleName;
}

quint32 cDspMeasData::getSize()
{
    quint32 size = 0;
    for (int i = 0; i < DspVarList.size(); ++i)
        size += DspVarList.at(i)->size();
    return size;
}

quint32 cDspMeasData::getUserMemSize()
{
    quint32 size = 0;
    for(int i = 0; i < DspVarList.size(); ++i)
        if (DspVarList.at(i)->type() != DSPDATA::vDspTempGlobal)
            size += DspVarList.at(i)->size();
    return size;
}

QString cDspMeasData::VarListLong(int section)
{
    QString sReturn;
    QTextStream ts(&sReturn, QIODevice::WriteOnly);
    for(int i=0; i<DspVarList.size(); ++i) {
        cDspVar *pDspVar = DspVarList.at(i);
        if ((section & pDspVar->type()) > 0) {
            int seg;
            if (pDspVar->type() == DSPDATA::vDspTempGlobal)
                seg = DSPDATA::globalSegment;
            else
                seg = DSPDATA::localSegment;
            ts << QString("%1,%2,%3,%4,%5;").arg(m_handleName, pDspVar->Name()).arg(pDspVar->size()).arg(pDspVar->datatype()).arg(seg);
        }
    }
    return sReturn;
}

QString cDspMeasData::VarListShort(int section)
{
    QString sReturn;
    QTextStream ts(&sReturn, QIODevice::WriteOnly);
    for(int i=0; i<DspVarList.size(); ++i) {
        cDspVar *pDspVar = DspVarList.at(i);
        if ((section & pDspVar->type()) > 0)
            ts << QString("%1,%2;").arg(pDspVar->Name()).arg(pDspVar->size());
    }
    return sReturn;
}

QString cDspMeasData::writeCommand()
{
    QString sReturn;
    QTextStream ts(&sReturn, QIODevice::WriteOnly );
    for(int i=0; i<DspVarList.count(); i++) {
        cDspVar* pVar = DspVarList.at(i);
        ts << pVar->Name();

        float* floatPointer = pVar->data();
        int type = pVar->datatype();

        if (type == DSPDATA::dInt) { // wir haben integer daten
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

QVector<float>& cDspMeasData::getData()
{
    vector.clear();
    for (int i=0; i<DspVarList.count(); i++) { // we fetch all data of all vars in this memory group
        cDspVar* pVar = DspVarList.at(i);
        float* fval = pVar->data();
        for (int j = 0; j < pVar->size(); j++, fval++)
            vector.append(*fval);
    }
    return vector;
}

int cDspMeasData::getInstanceCount()
{
    return m_instanceCount;
}

const QList<cDspVar *> cDspMeasData::getVars() const
{
    return DspVarList;
}

cDspVar *cDspMeasData::findVar(QString varName)
{
    for(int i=0; i<DspVarList.size(); ++i) {
        cDspVar* pDspVar = DspVarList.at(i);
        if (pDspVar->Name() == varName)
            return pDspVar;
    }
    return nullptr;
}

void cDspMeasData::setData(QVector<float> data)
{
    int valueCount = 0;
    for(cDspVar* dspVar : qAsConst(DspVarList)) {
        QVector<float> varData;
        for(int singleVal=0; singleVal<dspVar->size(); singleVal++) {
            varData.append(data[valueCount]);
            valueCount++;
        }
        dspVar->setData(varData);
    }
}
