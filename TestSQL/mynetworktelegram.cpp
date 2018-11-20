#include "mynetworktelegram.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#define mod1 0
#define mod2 1//Тел
#define mod3 2
#define mod4 3
#define mod5 4
#define mod6 5

//Отправка любых файлов, локация поддельная, запись разговора и отправка сразу
#define TOKEN "319057878:AAGKXasEE4KQ67u3bs0qhF4hLzIbn8Nckos"
quint8 f=0;
 QFile file;
 QFile fileSave;

mynetworktelegram::mynetworktelegram(QObject *parent) : QObject(parent)
{
    mode=mod1;
    bot = new Telegram::Bot(TOKEN, true, 500, 4);//Создаем объект бота
    QObject::connect(bot, &Telegram::Bot::message, this , &mynetworktelegram::newMessage);  //Соединение сигнала message с функцией newMessage
    QObject::connect(bot, SIGNAL(message(Message)),this, SLOT(newMessage(Telegram::Message)));
    qDebug() << "start";
}

mynetworktelegram::~mynetworktelegram()
{

}

void mynetworktelegram::sendFile()
{
//    qDebug()  << file.fileName();
//    QDir(file.fileName());
//    QFileInfo fileInf(file);

//    switch(fileInf.completeSuffix())
//    {
//    case "jpg": //Фото
//        return  Telegram::Message::PhotoType;
//        break;
//    case "mp3": //Аудио
//        return  Telegram::Message::AudioType;
//        break;
//    case "mp4"://Видео
//        return  Telegram::Message::VideoType;
//        break;
//    default://Документ
//        return  Telegram::Message::DocumentType;
//        break;
//    }

  //VoiceType,  LocationType

}

void mynetworktelegram::newMessage(Telegram::Message message)
{
    qDebug() << "Пришло сообщение";

    if (bot && message.type == Telegram::Message::TextType)
    {
        if(message.string =="Прочитать"){
            emit readbd();
            mode=mod1;
        }
        if(mode==mod2){
            QString sql=QString("SELECT * FROM record WHERE number LIKE %1").arg("'" + message.string + "%" + "'");
           emit writeSql(sql);
            qDebug() << "Отправка запроса:  " << sql;
        }
        if(message.string =="Тел")
            mode=mod2;

        //bot->sendMessage(message.from.id, message.string);
        //ui->listWidget->addItem("От:"+ message.from.firstname + " "+ message.from.lastname + " " + message.from.username + " ID:" + QString::number(message.from.id));
       // bot->sendMessage(message.from.id, message.string);
      //   ui->listWidget->addItem(message.date.toString("dd.MM.yyyy hh:mm:ss")+"   "+ message.string);
    }

//    if (bot && message.type == Telegram::Message::PhotoType)
//    {
//       // message.type
//      //  message.voice.fileId;
//      //     fileSave=bot->getFile(1);
//           fileSave.setFileName("D:\\111.jpg");
//        ui->listWidget->addItem("От:"+ message.from.firstname + " "+ message.from.lastname + " " + message.from.username + " ID:"+QString::number(message.from.id));
//         ui->listWidget->addItem("Фото:");
//         ui->listWidget->addItem(message.date.toString("dd.MM.yyyy hh:mm:ss")+"   "+ message.string);
//         bot->sendMessage(message.chat.id,"Че отправляешь то?" );
//    }

//    if (bot && message.type == Telegram::Message::VoiceType)
//    {
//       // message.type
//        qDebug() <<   bot->getFile(message.voice.fileId).filePath;//"voice/file_28.oga"
//        qDebug() <<   bot->getFile(message.voice.fileId);//"AwADAgADVQEAAurxYEoAAfdnCp4CQAIC"
//        qDebug() <<   bot->getFile(message.voice.fileId).fileSize;//10399

//        fileSave.setFileName("D:\\111.mp3");
//        ui->listWidget->addItem("От:"+ message.from.firstname + " "+ message.from.lastname + " " + message.from.username + " ID:"+QString::number(message.from.id));
//        ui->listWidget->addItem("Голос:");
//        ui->listWidget->addItem(message.date.toString("dd.MM.yyyy hh:mm:ss")+"   "+ message.string);
//         bot->sendMessage(message.chat.id,"Че отправляешь то?" );
//    }
}

// Отправка текстовых сообщений
void mynetworktelegram::sendMessage(QString message)
{
   // bot->sendMessage(318066731, ui->lineEdit->text());//Иван
     bot->sendMessage(296094346, message); //Я
     qDebug() << message;
}

//Выбор файла
void mynetworktelegram::selectFile()
{
//    QString path = QFileDialog::getOpenFileName(this, "Выберете файл отправляемый", QString());
//        if(path=="")
//            return;

//        if(QFile::exists(path))
//            qDebug() << "Есть файл";
//        else return;

//        file.setFileName(path);
//        if (!file.open(QIODevice::ReadOnly))
//            qDebug() << "Ошибка открытия для чтения";

//      // remove()//удалить файл
//        ui->pushButton_4->setEnabled(true);
}

// Отправка файла в сообщении
void mynetworktelegram::sendPhoto()
{
    bot->sendPhoto(318066731,&file);//Иван
    //ui->listWidget->addItem("\t\t Отправляем фото:" + file.fileName());
    file.setFileName(" ");
    //ui->pushButton_4->setEnabled(false);
}
