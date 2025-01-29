#ifndef AUTOJOURNALLOGGERFACADE_H
#define AUTOJOURNALLOGGERFACADE_H

#include <QByteArray>

// PIMPL: Avoid linking simple-parser public
class AutoJournalLoggerFacadePrivate;

class AutoJournalLoggerFacade
{
public:
    AutoJournalLoggerFacade(QByteArray codedumpDir = "/var/lib/zenux-coredumps/", int portNo = 5000);
    virtual ~AutoJournalLoggerFacade();
private:
    AutoJournalLoggerFacadePrivate* m_loggerFacadePrivate;
};

#endif // AUTOJOURNALLOGGERFACADE_H
