#ifndef TEST_ADJ_DATA_CACHE_H
#define TEST_ADJ_DATA_CACHE_H

#include <QObject>

class test_adj_data_cache : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();
    void pathCreatedOnFirstUse();
};

#endif // TEST_ADJ_DATA_CACHE_H
