#include "fileutils.h"
#include <QFileInfo>
#include <QDir>

bool FileUtils::dirExists(QString fullFileName)
{
    QFileInfo fi(fullFileName);
    QString dirName = fi.absolutePath();
    QDir dir(dirName);
    QString canonicalPath = dir.canonicalPath();
    return !canonicalPath.isEmpty();
}
