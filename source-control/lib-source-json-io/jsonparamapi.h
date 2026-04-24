#ifndef JSONPARAMAPI_H
#define JSONPARAMAPI_H

#include <QJsonObject>

enum class phaseType{
    U,
    I
};

class JsonParamApi
{
public:
    JsonParamApi();
    QJsonObject getParams() const;
    void setParams(QJsonObject params);

    bool getOn() const;
    void setOn(bool on);

    double getRms(phaseType type, int phaseIdxBase0) const;
    void setRms(phaseType type, int phaseIdxBase0, double value);

    double getAngle(phaseType type, int phaseIdxBase0) const;
    void setAngle(phaseType type, int phaseIdxBase0, double value);

    bool getOn(phaseType type, int phaseIdxBase0) const;
    void setOn(phaseType type, int phaseIdxBase0, bool on);

    bool getFreqVarOn() const;

    double getFreqVal() const;
    void setFreqVal(double value);

private:
    static QString getPhaseName(phaseType type, int phaseIdxBase0);
    static QString getPhaseNamePrefix(phaseType type);
    QJsonObject m_params;
};

#endif // JSONPARAMAPI_H
