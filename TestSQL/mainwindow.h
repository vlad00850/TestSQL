#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlQuery"

#include "time.h"
#include "ctime"
#include <string>
#include "QtConcurrent/QtConcurrent"
#include <QCloseEvent>
#include <synchapi.h> //для Sleep();

#include "QMediaPlayer"
#include "QMediaPlaylist"

#include <QStandardItemModel>
#include "QStandardItem"
#include "QtSql/QSqlTableModel"
#include "QAbstractItemModel"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

#include "audioplayeer.h"

#include "smsdialog.h"
#include "mynetworktelegram.h"

#include "database.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSqlQueryModel *SqlWrite;
    QSqlDatabase db;
    QSqlTableModel *SqlTabMod;
    smsDialog *formsms;
    mynetworktelegram *teleg;
    databaseMy *myDataBase;

private:
    Ui::MainWindow *ui;
    audioplayeer *audioPlay;

private slots:
    void SelectDataBase(); //Выбор БД
    void InOutCall();//Входящий или исходящий сортировка
    void FindRecord(QString textSort);//Сортировка
    void click_findOk();//при изменении текста, значение передается в фун-ию сортировки
    void bdbd(); //Перенос из модели в базу данных
    void migrationMeizu(); //миграция базы данных contacts2 и RecorderDb в одну модель
    void migrationMeizuSomes(); //множественная миграция

    void setMusicRange(qint64 time);//При воспроизведении передвигается ползунок

    void getId(QModelIndex row);// метод получения имени файла ( указать папку с файлами)

  // QString dateCreate(QString time1); //метод получения и изменения времени

    QStringList FileFinderAmrMp3(const QDir& dir);//Рекурсивная функция поиска аудиофайлов и запись в переменнную типа QStringList
    QStringList FileFinderBd(const QDir& dir);//Рекурсивная функция поиска файлов базы данных и запись в переменнную типа QStringList
    void on_btn_add_clicked();//Выбор папки хранения аудиофайлов
    void inizilizeProgBar(int range);//Инициализация линии прогресса записи в БД
    void payPrint(QAbstractItemModel *modelTable);//Раскрашиваение элементов
    void payPrintThread();
    void payPrintEnd(QStandardItemModel *model2,int colZapGreen,int _zapInBD);
    void smsCreate();  // Создание формы "SMS"


    void onItemActivated(QModelIndex index);
    QStringList horizontalHeaderMy();//Заголовок таблицы

    void writeSql(QString sql="teleg"); //для запросов в БД

    void currentTrackLabel(int index);//Для вывода в Label - currentTrack трека который воспроизводится
    void closeEvent(QCloseEvent *);

    void MainWindow2(QSqlDatabase db1); // Принимает БД
    void slotError(QString text);
    void slotMessage(QString message);
    void startNetwork();

    void readAndSendTeleg();

signals:
    void signError(QString error);
    void signpayPrintEnd(QStandardItemModel *model2,int colZapGreen,int _zapInBD);
    void signProgresBarHidden(bool a);
    void signProgresBar(int step);
    void signInizilizeProgBar(int range);
    void combobox2click();
    void openBD(QString PATH,QSqlDatabase* db);

    void sendMessageTeleg(QString message);
};

#endif // MAINWINDOW_H
