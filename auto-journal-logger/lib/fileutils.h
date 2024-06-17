#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils
{
public:
    static bool dirExistsForFileName(QString fullFileName);
    static QString getExistingParentNonRootPath(const QString &path);
    static QString getLocalHostname();
};

#endif // FILEUTILS_H
