#include "cputemp.h"
#include "systeminfofilelocator.h"
#include <QDir>

float CpuTemp::getValue()
{
    QString tempFileLocation = findTempFileLocation();
    if(!tempFileLocation.isEmpty()) {
        QFile temperatureFile(tempFileLocation);
        if (temperatureFile.open(QIODevice::ReadOnly)) {
            float temperature = temperatureFile.readAll().trimmed().toInt();
            temperature = temperature / 1000;
            return temperature;
        }
    }
    return -273.0;
}

QString CpuTemp::findTempFileLocation()
{
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
    qWarning("Could not find matching thermal-zone!");
    return "";
}
