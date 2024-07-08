#include "autojournalloggerfacade.h"
#include "autojournalloggerfacadeprivate.h"

AutoJournalLoggerFacade::AutoJournalLoggerFacade(QByteArray codedumpDir, int portNo) :
    m_loggerFacadePrivate(new AutoJournalLoggerFacadePrivate(codedumpDir, portNo))
{
}

AutoJournalLoggerFacade::~AutoJournalLoggerFacade()
{
    delete m_loggerFacadePrivate;
}
