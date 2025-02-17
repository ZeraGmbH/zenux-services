#include "autojournalloggerfacade.h"
#include "autojournalloggerfacadeprivate.h"

AutoJournalLoggerFacade::AutoJournalLoggerFacade(AbstractLogCreatorPtr logGenerator,
                                                 QByteArray codedumpDir,
                                                 int portNo) :
    m_loggerFacadePrivate(new AutoJournalLoggerFacadePrivate(codedumpDir, portNo, std::move(logGenerator)))
{
}

AutoJournalLoggerFacade::~AutoJournalLoggerFacade()
{
    delete m_loggerFacadePrivate;
}
