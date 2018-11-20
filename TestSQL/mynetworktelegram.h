#ifndef MYNETWORKTELEGRAM_H
#define MYNETWORKTELEGRAM_H

#include <QObject>
#include "qttelegrambot.h"

class mynetworktelegram : public QObject
{
    Q_OBJECT
public:
    explicit mynetworktelegram(QObject *parent = 0);
    ~mynetworktelegram();
    void sendFile(); // Отправка файла
    Telegram::Bot *bot;

    int mode;

signals:
    void writeSql(QString sql);
        void readbd();

public slots:
    void newMessage(Telegram::Message message);
    //   QString messageStr(Telegram::Message message);
    void sendMessage(QString message);
    void selectFile();
    void sendPhoto();

};

#endif // MYNETWORKTELEGRAM_H
