#include "fileutils.h"
#include <QFileInfo>
#include <QDir>

bool FileUtils::dirExists(QString fullFileName)
{
    QFileInfo fi(fullFileName);
    QString dirName = fi.absolutePath();
    QDir dir(dirName);
    QString canonicalDirPath = dir.canonicalPath();
    if(!canonicalDirPath.isEmpty()) {
        QFileInfo dirFi(canonicalDirPath);
        return dirFi.isDir();
    }
    return false;
}
