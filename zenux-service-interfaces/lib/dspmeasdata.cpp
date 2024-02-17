#include <QTextStream>

#include "dspmeasdata.h"


cDspMeasData::cDspMeasData(QString name) :
    m_handleName(name)
{
}

cDspMeasData::~cDspMeasData()
{
    if (DspVarList.count() > 0)
    {
        cDspVar* pDspVar;
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            pDspVar = DspVarList.at(i);
            delete pDspVar;
        }
    }
}

float* cDspMeasData::data(QString name) // gibt einen zeiger zurück auf die var daten
{
    if (DspVarList.count() > 0)
    {
        cDspVar* pDspVar;
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            pDspVar = DspVarList.at(i);
            if (pDspVar->Name() == name)
                return pDspVar->data();
        }
    }
    return 0; // caller has to pay attention !!!!!
}

void cDspMeasData::addVarItem(cDspVar* varItem)
{
    DspVarList.append(varItem);
}

quint32 cDspMeasData::getSize()
{
    quint32 size = 0;

    if (DspVarList.count() > 0)
    {
        for (int i = 0; i < DspVarList.size(); ++i)
            size += DspVarList.at(i)->size();
    }
    return size;
}

quint32 cDspMeasData::getSize(QString name)
{
    quint32 size = 0;

    if (DspVarList.count() > 0)
    {
        cDspVar* pDspVar;
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            pDspVar = DspVarList.at(i);
            if (pDspVar->Name() == name)
                return pDspVar->size();
        }
    }

    return size;
}


quint32 cDspMeasData::getumemSize()
{
    quint32 size = 0;

    if (DspVarList.count() > 0)
    {
        for (int i = 0; i < DspVarList.size(); ++i)
            if (DspVarList.at(i)->type() != DSPDATA::vDspTempGlobal)
                size += DspVarList.at(i)->size();
    }

    return size;
}

QString &cDspMeasData::VarListLong(int section)
{
    sReturn = "";
    QTextStream ts(&sReturn, QIODevice::WriteOnly);
    cDspVar *pDspVar;
    for (int i = 0; i < DspVarList.size(); ++i)
    {
        pDspVar = DspVarList.at(i);
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

QString &cDspMeasData::VarListShort(int section)
{
    sReturn = "";
    QTextStream ts(&sReturn, QIODevice::WriteOnly);
    cDspVar *pDspVar;
    for (int i = 0; i < DspVarList.size(); ++i) {
        pDspVar = DspVarList.at(i);
        if ((section & pDspVar->type()) > 0)
            ts << QString("%1,%2;").arg(pDspVar->Name()).arg(pDspVar->size());
    }
    return sReturn;
}

QString& cDspMeasData::writeCommand()
{
    sReturn="";
    QTextStream ts(&sReturn, QIODevice::WriteOnly );

    for (int i = 0; i < DspVarList.count(); i++)
    {
        cDspVar* pVar = DspVarList.at(i);
        ts << pVar->Name();

        float* fval = pVar->data();
        int type = pVar->datatype();

        if (type == DSPDATA::dInt)  // wir haben integer daten
        {
            ulong* lval = (ulong*) fval;
            for (int j = 0; j < pVar->size(); j++, lval++)
                ts << "," << *lval;
        }
        else
        {
            for (int j = 0; j < pVar->size(); j++, fval++)
                ts << "," << *fval;
        }

        ts << ";";
    }

    return sReturn;

}


QVector<float>& cDspMeasData::getData()
{
    vector.clear();
    for (int i = 0; i < DspVarList.count(); i++) // we fetch all data of all vars in this memory group
    {
        cDspVar* pVar = DspVarList.at(i);
        float* fval = pVar->data();
        for (int j = 0; j < pVar->size(); j++, fval++)
            vector.append(*fval);
    }
    return vector;
}

void cDspMeasData::setData(QVector<float> data)
{
    int valueCount = 0;
    for(cDspVar* dspVar : qAsConst(DspVarList)) {
        QVector<float> varData;
        for(int singleVal = 0; singleVal<dspVar->size(); singleVal++) {
            varData.append(data[valueCount]);
            valueCount++;
        }
        dspVar->setData(varData);
    }
}
