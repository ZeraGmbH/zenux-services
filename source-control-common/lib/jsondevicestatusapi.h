#ifndef JSONDEVICESTATUS_H
#define JSONDEVICESTATUS_H

#include <QJsonObject>

class JsonDeviceStatusApi
{
public:
    JsonDeviceStatusApi();
    void reset();
    void clearWarningsErrors();
    void setBusy(bool busy);
    bool getBusy() const;
    void addError(const QString &error);
    QStringList getErrors() const;
    void addWarning(const QString &warning);
    QStringList getWarnings() const;
    void setDeviceInfo(const QString &strDeviceInfo);
    QString getDeviceInfo() const;
    const QJsonObject &getJsonStatus() const;

private:
    QStringList getArray(const QString &key) const;
    void appendToArray(const QString &key, const QString &value);
    QJsonObject m_jsonStatus;
};

#endif // JSONDEVICESTATUS_H
