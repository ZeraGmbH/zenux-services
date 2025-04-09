#include "zenuxdevicedetection.h"
#include <QFile>

QString ZenuxDeviceDetection::getDeviceNameFromKernelParam()
{
    QString zenuxDevice = QStringLiteral("unknown");
    QString procKernelParamFilename = QStringLiteral("/proc/cmdline");
    QFile file(procKernelParamFilename);
    if(file.open(QIODevice::ReadOnly)) {
        const QStringList kernelParams = QString::fromLatin1(file.readAll()).split(QStringLiteral(" "));
        QString paramSearch = QStringLiteral("zera_device=");
        for(auto param : kernelParams) {
            if(param.contains(paramSearch)) {
                zenuxDevice = param.replace(paramSearch, QString()).trimmed();
                break;
            }
        }
        file.close();
    }
    return zenuxDevice;
}
