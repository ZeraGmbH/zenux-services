#include "cputemp.h"
#include "systeminfofilelocator.h"

#include <QDir>

CpuTemp::CpuTemp()
{
}

int CpuTemp::getTemperature()
{
    int ret = -1;

    QFile temperatureFile(findTempFileLocation());
    if (temperatureFile.open(QIODevice::ReadOnly)) {
        int temperature = temperatureFile.readAll().trimmed().toInt();
        temperature = temperature / 1000;
        return temperature;
    }
    return ret;
}

float CpuTemp::getValue()
{
    return getTemperature();
}

QString CpuTemp::findTempFileLocation()
{
    QString ret = "";

    QDir thermalDir(SystemInfoFileLocator::getSysTempRootPath());
    thermalDir.setNameFilters(QStringList("thermal_zone*"));
    QFileInfoList infoList = thermalDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(auto &entry : infoList) {
        QFile thermalFile(entry.absoluteFilePath() + "/type");
        if (thermalFile.open(QIODevice::ReadOnly)) {
            QString typeContent = thermalFile.readAll();
            if (typeContent == "x86_pkg_temp\n" || typeContent == "imx_thermal_zone\n")
                return entry.absoluteFilePath() + "/temp";
        }
    }

    return ret;
}
