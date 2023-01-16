#ifndef DEBUGSETTINGS_H
#define DEBUGSETTINGS_H

#include <xmlsettings.h>

class cDebugSettings: public XMLSettings
{
    Q_OBJECT
public:
    cDebugSettings(Zera::XMLConfig::cReader *xmlread, QString topXmlNode);
    quint8 getDebugLevel();
public slots:
    virtual void configXMLInfo(QString key) override;
private:
    quint8 m_nDebugLevel;
};

#endif // DEBUGSETTINGS_H
