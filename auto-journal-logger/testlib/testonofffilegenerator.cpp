#include "testonofffilegenerator.h"
#include "autojournallogger.h"
#include "onofffilewatcher.h"
#include <QDir>
#include <QFile>

void TestOnOffFileGenerator::createOnFile()
{
    QDir dir;
    dir.mkpath(OnOffFileWatcher::getOnOffDir());

    QFile file(OnOffFileWatcher::getOnOffFullPath());
    file.open(QIODevice::WriteOnly);
}

void TestOnOffFileGenerator::deleteOnFile()
{
    QFile::remove(OnOffFileWatcher::getOnOffFullPath());
}

QString TestOnOffFileGenerator::getTestLogPath()
{
    return "/tmp/";
}

void TestOnOffFileGenerator::deleteLogFile()
{
    AutoJournalLogger logger(getTestLogPath());
    QFile::remove(logger.getLogFilePath());
}

