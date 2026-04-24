#include "jsonparamapi.h"

JsonParamApi::JsonParamApi()
{
}

QJsonObject JsonParamApi::getParams() const
{
    return m_params;
}

void JsonParamApi::setParams(QJsonObject params)
{
    m_params = params;
}

bool JsonParamApi::getOn() const
{
    return m_params.contains("on") && m_params["on"].toBool();
}

void JsonParamApi::setOn(bool on)
{
    m_params["on"] = on;
}

double JsonParamApi::getRms(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject().value("rms").toDouble(0.0);
}

void JsonParamApi::setRms(phaseType type, int phaseIdxBase0, double value)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject phaseValue = m_params[phaseName].toObject();
    phaseValue["rms"] = value;
    m_params[phaseName] = phaseValue;
}

double JsonParamApi::getAngle(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject().value("angle").toDouble(0.0);
}

void JsonParamApi::setAngle(phaseType type, int phaseIdxBase0, double value)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject phaseValue = m_params[phaseName].toObject();
    phaseValue["angle"] = value;
    m_params[phaseName] = phaseValue;
}

bool JsonParamApi::getOn(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject obj = m_params[phaseName].toObject();
    return obj.contains("on") && obj["on"].toBool();
}

void JsonParamApi::setOn(phaseType type, int phaseIdxBase0, bool on)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject phaseValue = m_params[phaseName].toObject();
    phaseValue["on"] = on;
    m_params[phaseName] = phaseValue;
}

bool JsonParamApi::getFreqVarOn() const
{
    return m_params["Frequency"].toObject().value("type") == "var";
}

double JsonParamApi::getFreqVal() const
{
    return m_params["Frequency"].toObject().value("val").toDouble();
}

void JsonParamApi::setFreqVal(double value)
{
    QJsonObject freqValue = m_params["Frequency"].toObject();
    freqValue["val"] = value;
    m_params["Frequency"] = freqValue;
}

QString JsonParamApi::getPhaseName(phaseType type, int phaseIdxBase0)
{
    return QString("%1%2").arg(getPhaseNamePrefix(type)).arg(phaseIdxBase0+1);
}

QString JsonParamApi::getPhaseNamePrefix(phaseType type)
{
    switch(type) {
    case phaseType::U:
        return "U";
    case phaseType::I:
        return "I";
    }
    return QString();
}
