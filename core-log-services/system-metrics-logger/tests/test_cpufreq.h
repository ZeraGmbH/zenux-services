#ifndef TEST_CPUFREQ_H
#define TEST_CPUFREQ_H

#include <QObject>

class test_cpufreq : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();
    void test_freq_valid();
    void test_freq_invalid_dir();
};

#endif // TEST_CPUFREQ_H
