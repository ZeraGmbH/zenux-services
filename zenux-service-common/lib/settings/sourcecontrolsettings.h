#ifndef SOURCECONTROLSETTINGS_H
#define SOURCECONTROLSETTINGS_H

#include <xmlsettings.h>

class SourceControlSettings : public XMLSettings
{
    Q_OBJECT
public:
    SourceControlSettings(Zera::XMLConfig::cReader *xmlread);
    QString getSourceCapFile();
public slots:
    void configXMLInfo(const QString &key) override;
private:
    QString  m_sourceCapFile;
};

#endif // SOURCECONTROLSETTINGS_H
