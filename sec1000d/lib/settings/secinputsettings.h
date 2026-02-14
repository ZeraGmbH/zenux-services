#ifndef SECINPUTSETTINGS
#define SECINPUTSETTINGS

#include <xmlsettings.h>
#include <QHash>

namespace InputSettings
{
enum configstate
{
    setnumber,
    setinputname1 = 16,
    setinputmuxer1 = 48
};
}

struct cInputInfo
{
    QString m_sName;
    quint8 m_nMux;
};

class SecInputSettings : public XMLSettings
{
    Q_OBJECT

public:
    SecInputSettings(Zera::XMLConfig::cReader *xmlread);

    bool hasInput(QString name);
    qint8 mux(QString name);

public slots:
    void configXMLInfo(const QString &key) override;

private:
    quint16 m_nCount;
    QString actName;
    QHash<QString, quint8> muxInfoHash;
};

#endif // SECINPUTSETTINGS

