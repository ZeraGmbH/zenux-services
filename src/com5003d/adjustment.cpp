#include "com5003dglobal.h"
#include "adjflash.h"
#include "justdata.h"
#include "adjxml.h"
#include "systeminfo.h"
#include "adjustment.h"
#include "micro-controller-io/atmel.h"
#include <F24LC256.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QTextStream>
#include <QDomDocument>
#include <QDomText>
#include <QDomNodeList>
#include <QFile>
#include <QDateTime>
#include <syslog.h>


cAdjustment::cAdjustment(cSystemInfo* sInfo, QString &devNode, quint8 adr)
    :m_pSystemInfo(sInfo), m_sDeviceNode(devNode), m_nI2CAdr(adr)
{
    m_AdjXMLList.append(this);
}


cAdjustment::~cAdjustment()
{
}


bool cAdjustment::exportJDataFlash()
{
   quint32 count=0;
   m_nChecksum = 0;
   QByteArray ba;

   QDataStream stream(&ba,QIODevice::ReadWrite);
   stream.setVersion(QDataStream::Qt_5_4);

   QDateTime DateTime;

   stream << count;
   stream << m_nChecksum; // checksumme

   // ab version v1.02
   stream << "ServerVersion";
   stream << ServerVersion;

   stream << m_pSystemInfo->getDeviceName().toStdString().c_str(); // leiterkarten name aus atmel gelesen
   stream << m_pSystemInfo->getDeviceVersion().toStdString().c_str(); // geräte name versionsnummern ...
   stream << m_pSystemInfo->getSerialNumber().toStdString().c_str(); // seriennummer
   stream << DateTime.toString(Qt::TextDate).toStdString().c_str(); // datum,uhrzeit
   for (int i = 0; i < m_AdjFlashList.count(); i++)
       m_AdjFlashList.at(i)->exportAdjData(stream);

   count = ba.count(); // um die länge zu bestimmen
   QByteArray ca(6, 0); // qbyte array mit 6 bytes

   QDataStream castream( &ca, QIODevice::WriteOnly );
   castream.setVersion(QDataStream::Qt_5_4);

   castream << count << m_nChecksum;

   QBuffer mem(&ba);
   mem.open(QIODevice::ReadWrite);
   mem.seek(0); // qbuffer auf den anfang positionieren
   mem.write(ca); // überschreibt die länge + checksumme (noch 0)

   m_nChecksum = qChecksum(ba.data(),ba.size()); // +crc-16
   QDataStream castream2( &ca, QIODevice::WriteOnly );
   castream2.setVersion(QDataStream::Qt_5_4);

   castream2 << count << m_nChecksum;

   mem.seek(0);
   mem.write(ca); // überschreibt die länge und jetzt die richtige checksumme

   mem.close(); // wird nicht mehr benötigt

   cF24LC256* Flash = new cF24LC256(m_sDeviceNode, m_nI2CAdr);
   int written = Flash->WriteData(ba.data(),ba.size(),0);
   if ( (count - written) > 0)
   {
        syslog(LOG_ERR,"error writing flashmemory\n");
        return false; // fehler beim flash schreiben
   }

   importJDataFlash();

   return true;
}



bool cAdjustment::importJDataFlash()
{
    QDateTime DateTime;
    m_nAdjStatus = 0; // status reset

    QByteArray ba(6, 0); // byte array for length and checksum
    cF24LC256* Flash = new cF24LC256(m_sDeviceNode, m_nI2CAdr);
    if ( (6 - Flash->ReadData(ba.data(),6,0)) >0 ) {
        syslog(LOG_ERR,"error reading flashmemory\n");
        delete Flash;
        return(false); // lesefehler
    }

    QDataStream bastream( &ba, QIODevice::ReadOnly );
    bastream.setVersion(QDataStream::Qt_5_4);
    uint count;
    quint16 chksumCMP = 0;
    bastream >> count >> m_nChecksum; // länge der flashdaten u. checksumme
    // syslog(LOG_ERR,"flash length is %d, chksum is %d\n",count, m_nChecksum);
    uint flashSize = static_cast<uint>(Flash->size());
    if ( count > flashSize ) {
        syslog(LOG_ERR,"error reading flashmemory, count %i > flash %i\n",
               count, flashSize);
        delete Flash;
        return(false); // lesefehler
    }

    QByteArray ba2(count, 0); // byte array zur aufnahme der gesamten daten
    if ( (count - Flash->ReadData(ba2.data(),count,0)) >0 ) {
        syslog(LOG_ERR,"error reading flashmemory\n");
        delete Flash;
        return(false); // lesefehler
    }

    QBuffer mem;
    mem.setBuffer(&ba2);
    mem.open(QIODevice::ReadWrite);
    mem.seek(0);

    QByteArray ca(6, 0); // qbyte array mit 6 bytes
    QDataStream castream( &ca, QIODevice::WriteOnly );
    castream.setVersion(QDataStream::Qt_5_4);
    castream << count << chksumCMP;

    mem.write(ca); // 0 setzen der checksumme

    chksumCMP = qChecksum(ba2.data(),ba2.size());
    // syslog(LOG_ERR,"computed flash chksum is %d\n",chksumCMP);
    if (chksumCMP != m_nChecksum) {
        syslog(LOG_ERR,"invalid checksum encountered reading flashmemory: expected 0x%04X / found 0x%04X\n",
               chksumCMP, m_nChecksum);
        return(false); // daten fehlerhaft
    }

    // jetzt die daten noch einsortieren
    QString SVersion;
    QDataStream ba2stream( &ba2, QIODevice::ReadOnly );
    ba2stream.setVersion(QDataStream::Qt_5_4);

    char flashdata[100];
    char* s = flashdata;
    ba2stream >> count >> chksumCMP;

    ba2stream >> s; //
    if (QString(s) != "ServerVersion") {
        syslog(LOG_ERR,"flashmemory read, ServerVersion not found\n");
        return false; // datensatz falsch
    }

    ba2stream >> s;
    SVersion = QString(s);

    ba2stream >> s; // we take the device name

    QString sysDevName = m_pSystemInfo->getDeviceName();
    if (QString(s) != sysDevName) {
        syslog(LOG_ERR,"flashmemory read, contains wrong pcb name: flash %s / µC %s\n",
               s, qPrintable(sysDevName));
        return false; // wrong pcb name
    }

    ba2stream >> s;

    QString qs = QString(s);

    bool enable = false;
    pAtmel->getEEPROMAccessEnable(enable);

    QString sDV = m_pSystemInfo->getDeviceVersion();
    if (qs != sDV) {
        // test ob sich nur die hinteren nummern der lca bzw. ctrl version geändert haben
        // indem die hinteren stellen der nummern aus sDeviceVersion nach s übertragen werden
        // und anschliessend nochmal verglichen wird

        QString ss, sd, ss2, sd2;
        ss = qs.section(';',2,2); // LCA: x.xx
        ss2 = '.' +ss.section('.',1,1); // .xx
        sd = m_pSystemInfo->getDeviceVersion().section(';',2,2); // LCA: x.yy
        sd2 = '.' +sd.section('.',1,1); // .yy
        ss.replace(ss2,sd2); // tausch .xx durch .yy
        qs.replace(qs.section(';',2,2), ss); // LCA: x.yy -> s

        ss = qs.section(';',3,3); // CTRL: x.xx
        ss2 = '.' +ss.section('.',1,1); // .xx
        sd = m_pSystemInfo->getDeviceVersion().section(';',3,3); // CTRL: x.yy
        sd2 = '.' +sd.section('.',1,1); // .yy
        ss.replace(ss2,sd2); // tausch .xx durch .yy
        qs.replace(qs.section(';',3,3), ss); // CTRL: x.yy -> s

        if (qs != sDV)
        {
            syslog(LOG_ERR,"flashmemory read, contains wrong versionnumber: flash %s / µC %s\n",
                   qPrintable(qs), qPrintable(sDV));
            m_nAdjStatus += Adjustment::wrongVERS;
            if (!enable) {
                return false; // wrong version number
            }
        }
    }

    ba2stream >> s;
    QString sysSerNo = m_pSystemInfo->getSerialNumber();
    if (QString(s) != sysSerNo)
    {
        syslog(LOG_ERR,"flashmemory read, contains wrong serialnumber: flash %s / µC: %s\n",
               s, qPrintable(sysSerNo));
        m_nAdjStatus += Adjustment::wrongSNR;
        if (!enable) {
            return false; // wrong serial number
        }
    }

    ba2stream >> s;
    DateTime.fromString(QString(s),Qt::TextDate); // datum und uhrzeit übernehmen

    while (!ba2stream.atEnd()) {
        bool done = false;
        ba2stream >> s;
        QString  JDataSpecifier = s; // Type:Channel:Range
        for (int i = 0; i < m_AdjFlashList.count(); i++) {
            done |= m_AdjFlashList.at(i)->importAdjData(JDataSpecifier, ba2stream); // we call all participants
        }
        if (!done)
        {
            syslog(LOG_ERR,"flashmemory read, contains strange data\n");
            return false;
        }
    }

    return(true);
}


bool cAdjustment::exportJDataXML(QString &file)
{
    QDateTime DateTime;

    QString filename = file + ".xml";

    QFile adjfile(filename);
    if ( !adjfile.open( QIODevice::WriteOnly ) )
    {
        syslog(LOG_ERR,"justdata export, could not open xml file\n");
        return false;
    }

    QString s = QString("%1AdjustmentData").arg(LeiterkartenName);
    QDomDocument justdata (s);

    QDomElement pcbtag = justdata.createElement( "PCB" );
    justdata.appendChild( pcbtag );

    QDomElement tag = justdata.createElement( "Type" );
    pcbtag.appendChild( tag );
    QDomText t = justdata.createTextNode(LeiterkartenName);
    tag.appendChild( t );

    tag = justdata.createElement( "VersionNumber" );
    pcbtag.appendChild( tag );
    t = justdata.createTextNode( m_pSystemInfo->getDeviceVersion() );
    tag.appendChild( t );

    tag = justdata.createElement( "SerialNumber" );
    pcbtag.appendChild( tag );
    t = justdata.createTextNode(m_pSystemInfo->getSerialNumber());
    tag.appendChild( t );

    tag = justdata.createElement( "Date" );
    pcbtag.appendChild( tag );
    QDate d=DateTime.date();
    t = justdata.createTextNode(d.toString(Qt::TextDate));
    tag.appendChild( t );

    tag = justdata.createElement( "Time" );
    pcbtag.appendChild( tag );
    QTime ti=DateTime.time();
    t = justdata.createTextNode(ti.toString(Qt::TextDate));
    tag.appendChild( t );

    QDomElement adjtag = justdata.createElement( "Adjustment" );
    pcbtag.appendChild( adjtag );

    QDomElement chksumtag = justdata.createElement("Chksum");
    adjtag.appendChild(chksumtag);
    t = justdata.createTextNode(QString("0x%1").arg(m_nChecksum,0,16));
    chksumtag.appendChild(t);

    for (int i = 0; i < m_AdjXMLList.count(); i++)
        m_AdjXMLList.at(i)->exportAdjData(justdata, adjtag);

    QString xml = justdata.toString();

    QTextStream stream( &adjfile );
    stream << xml;
    adjfile.close();
    return true;
}


bool cAdjustment::importJDataXML(QString &file)
{
    QDateTime DateTime;

    QString filename = file + ".xml";

    QFile adjfile(filename);
    if ( !adjfile.open( QIODevice::ReadOnly ) )
    {
        syslog(LOG_ERR,"justdata import, could not open xml file\n");
        return false;
    }

    QDomDocument justdata( "TheDocument" );
    if ( !justdata.setContent( &adjfile ) )
    {
        adjfile.close();
        syslog(LOG_ERR,"justdata import, format error in xml file\n");
        return false;
    }

    adjfile.close();

    QDomDocumentType TheDocType=justdata.doctype ();

    if  (TheDocType.name() != QString("%1AdjustmentData").arg(LeiterkartenName))
    {
        syslog(LOG_ERR,"justdata import, wrong xml documentype\n");
        return false;
    }

    QDomElement rootElem = justdata.documentElement();
    QDomNodeList nl=rootElem.childNodes();

    bool VersionNrOK=false;
    bool SerialNrOK=false;
    bool DateOK=false;
    bool TimeOK=false;

    for (int i=0; i<nl.length() ; i++)
    {
        QDomNode n = nl.item(i);
        QDomElement e = n.toElement();
        if ( e.isNull() )
        {
            syslog(LOG_ERR,"justdata import, format error in xml file\n");
            return false;
        }

        QString tName=e.tagName();
        if (tName == "SerialNumber")
        {
            if (  !(SerialNrOK = (e.text() == m_pSystemInfo->getSerialNumber() )) )
            {
               syslog(LOG_ERR,"justdata import, wrong serialnumber in xml file\n");
               return false;
            }

        }

        else

        if (tName == "VersionNumber")
        {
           if ( ! ( VersionNrOK= (e.text() == m_pSystemInfo->getDeviceVersion()) ) )
           {
               syslog(LOG_ERR,"justdata import, wrong versionnumber in xml file\n");
               return false;
           }

        }

        else

        if (tName=="Date")
        {
            QDate d=QDate::fromString(e.text(),Qt::TextDate);
            DateTime.setDate(d);
            DateOK=true;

        }

        else

        if (tName=="Time")
        {
            QTime t=QTime::fromString(e.text(),Qt::TextDate);
            DateTime.setTime(t);
            TimeOK=true;
        }

        else

        if (tName == "Adjustment")
        {
            if ( VersionNrOK && SerialNrOK && DateOK && TimeOK)
            {
                //QDomNodeList nl2=e.elementsByTagName ("Adjustment") ;
                QDomNodeList nl2=e.childNodes();
                for (qint32 j=0;j<nl2.length();j++)
                {
                    bool done;

                    n=nl2.item(j);
                    done = false;

                    for (int i = 0; i < m_AdjXMLList.count(); i++)
                        done |= m_AdjXMLList.at(i)->importAdjData(n); // we call all participants

                    if (!done)
                    {
                        syslog(LOG_ERR,"justdata import, xml file contains strange data\n");
                        return false;
                    }

                }
            }
            else
            {
                syslog(LOG_ERR,"justdata import, xml file contains strange data\n");
                return false;
            }
        }

    }

    return true;
}


void cAdjustment::exportAdjData(QDomDocument &doc, QDomElement &qde)
{
}


bool cAdjustment::importAdjData(QDomNode &node)
{
    bool ok;

    if (node.toElement().tagName() != "Chksum") // data not for us
        return false;

    m_nChecksum = node.toElement().text().toInt(&ok);
    return true;
}


void cAdjustment::addAdjFlashObject(cAdjFlash* obj)
{
    m_AdjFlashList.append(obj);
}


void cAdjustment::addAdjXMLObject(cAdjXML* obj)
{
    m_AdjXMLList.append(obj);
}


quint8 cAdjustment::getAdjustmentStatus()
{
    quint8 stat = 255;
    quint8 ret;

    for (int i = 0; i < m_AdjFlashList.count(); i++)
        stat &= m_AdjFlashList.at(i)->getAdjustmentStatus();

    ret = m_nAdjStatus; // here we already stored ev. wrong serial or wrong version

    if ((stat & JustData::Justified)== 0)
        ret += Adjustment::notAdjusted;

    return ret;
}


quint16 cAdjustment::getChecksum()
{
    return m_nChecksum;
}

