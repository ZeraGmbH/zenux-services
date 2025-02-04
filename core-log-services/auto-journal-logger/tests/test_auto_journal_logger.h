#ifndef TEST_AUTO_JOURNAL_LOGGER_H
#define TEST_AUTO_JOURNAL_LOGGER_H

#include <QObject>

class test_auto_journal_logger : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void interfaceRegression();
    void wrongParamCount();
    void destinationPathEmpty();
    void destinationParentDirNotValid();
    void versionFileNotValid();
    void versionFileValid();
private:
    static QStringList searchFile(const QString &baseDirectory, const QString &fileName);
};

#endif // TEST_AUTO_JOURNAL_LOGGER_H
