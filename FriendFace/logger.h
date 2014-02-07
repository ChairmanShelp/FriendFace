#ifndef LOGGER_H
#define LOGGER_H

#include <QListWidget>
#include <QString>

using namespace std;

class Logger
{
public:
    void log(const QString& msg);
    static Logger& getInstance();
    void setLogWidget(QListWidget* logWidget);

protected:
    Logger();

private:
    QListWidget* logWidget;
    static Logger instance;
};

#endif // LOGGER_H
