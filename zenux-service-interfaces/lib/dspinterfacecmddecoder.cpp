#include "dspinterfacecmddecoder.h"

void DspInterfaceCmdDecoder::setVarData(cDspMeasData *memgroup, QString datalist, DSPDATA::dType type)
{
    // we expect something like Name1:0.2,0.4,0,3;Name2:1,2,3; ......
    const QStringList dataEntryList = datalist.split(";", Qt::SkipEmptyParts);
    for (const QString &dspVarEntry : dataEntryList) {
        QString varName = dspVarEntry.section(":",0,0);
        QString varValues = dspVarEntry.section(":",1,1);
        float *val = memgroup->data(varName);
        if (val != 0) {
            QStringList DataList = varValues.split(",");
            for ( QStringList::Iterator it2 = DataList.begin(); it2 != DataList.end(); ++it2,val++ ) {
                QString strValue = *it2;
                strValue.remove(';');
                if (type == DSPDATA::dInt) {
                    uint vul = strValue.toUInt(); // test auf ulong
                    *((uint*) val) = vul;
                }
                else
                    *val = strValue.toFloat();
            }
        }
    }
}
