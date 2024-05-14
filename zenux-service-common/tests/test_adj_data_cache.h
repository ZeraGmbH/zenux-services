#ifndef TEST_ADJ_DATA_CACHE_H
#define TEST_ADJ_DATA_CACHE_H

#include <QObject>

class test_adj_data_cache : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();

    // create cache
    void cachePathCreatedOnReadValidAdj();
    void cachePathNotCreatedOnReadinvalidAdj();
    void cacheFileCreatedOnReadValidAdj();
    void cacheFileNotCreatedOnReadInvalidAdj();
    void cacheFileCreatedSameContentAsEeprom();
    void cacheFileCreatedSameContentAsEepromOnOverwrite();

    // read cache
    void cacheFileNotReadOnInvalidEeprom();
    void cacheFileIsEmpty();
    void cacheFileIsTooShort();
    void cacheFileBitFlipped();
    void cacheFileReadSuccessfully();

    // no cache requested
    void nocachePathCreatedOnReadValidAdj();
    void nocacheFileCreatedOnReadValidAdj();
};

#endif // TEST_ADJ_DATA_CACHE_H
