#ifndef TEST_FIRMWAREUPDATENOTIFICATIONFILE_H
#define TEST_FIRMWAREUPDATENOTIFICATIONFILE_H

#include <QObject>

class test_firmwareupdatenotificationfile : public QObject
{
    Q_OBJECT
private slots:
    void testFileGeneratedAndDeleted();
};

#endif // TEST_FIRMWAREUPDATENOTIFICATIONFILE_H
