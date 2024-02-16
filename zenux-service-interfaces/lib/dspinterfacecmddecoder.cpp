#include "dspinterfacecmddecoder.h"

void DspInterfaceCmdDecoder::setVarData(cDspMeasData *memgroup, QString datalist, DSPDATA::dType type)
{
    QStringList DataEntryList, DataList; // werte zuorden

    bool ok;

    // we expect something like Name1:0.2,0.4,0,3;Name2:1,2,3; ......

    DataEntryList = datalist.split(";"); // wir haben jetzt eine stringliste mit allen werten

    for ( QStringList::Iterator it = DataEntryList.begin(); it != DataEntryList.end(); ++it )
    {
        QString snamedata, sname, sdata;

        snamedata = *it;
        if (!snamedata.isEmpty())
        {
            sname = snamedata.section(":",0,0);
            sdata = snamedata.section(":",1,1);

            float *val = memgroup->data(sname);
            if (val != 0)
            {
                DataList = sdata.split(",");
                for ( QStringList::Iterator it2 = DataList.begin(); it2 != DataList.end(); ++it2,val++ )
                {
                    QString s;
                    s = *it2;
                    s.remove(';');
                    if (type == DSPDATA::dInt)
                    {
                        ulong vul = s.toULong(&ok); // test auf ulong
                        *((ulong*) val) = vul;
                    }
                    else
                        *val = s.toFloat();
                }
            }
        }
    }
}
