#ifndef TEST_FILEUTILS_H
#define TEST_FILEUTILS_H

#include <QObject>

class test_fileutils : public QObject
{
    Q_OBJECT
private slots:
    void dirNotExist();
    void dirExistsForFileName();

    void dirFindExistingParent();
    void dirFindExistingParentOfFile();
};

#endif // TEST_FILEUTILS_H
