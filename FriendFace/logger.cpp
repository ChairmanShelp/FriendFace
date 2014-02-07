#include "logger.h"

Logger Logger::instance;

Logger::Logger() : logWidget(0)
{
}

void Logger::log(const QString& msg)
{
    if(logWidget != 0)
    {
        logWidget->insertItem(0, msg);
    }
}

Logger& Logger::getInstance()
{
    return Logger::instance;
}

void Logger::setLogWidget(QListWidget* logWidget)
{
    this->logWidget = logWidget;
}
