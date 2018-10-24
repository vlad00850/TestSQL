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

#include "audio.h"
#include "database.h"
#include "smsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlDatabase db1, QWidget *parent = 0);
    ~MainWindow();
    QSqlQueryModel *SqlWrite;
    QSqlDatabase db;
    QSqlTableModel *SqlTabMod;
    smsDialog *formsms;

private:
    Ui::MainWindow *ui;
    audio *Audio;

private slots:
    void SelectDataBase(); //Выбор БД
    void InOutCall();//Входящий или исходящий сортировка
    void Zapis(int i, QSqlQuery q);//Вывод на экран записей БД
    void FindRecord(QString textSort);//Сортировка
    void click_findOk();//при изменении текста, значение передается в фун-ию сортировки
    void bdbd(); //Перенос из модели в базу данных
    void bdbdThread();
    void migrationMeizu(); //миграция базы данных contacts2 и RecorderDb в одну модель

    void setMusicRange(qint64 time);//При воспроизведении передвигается ползунок

    void getId(QModelIndex row);// метод получения имени файла ( указать папку с файлами)

  // QString dateCreate(QString time1); //метод получения и изменения времени

    QStringList FileFinder(const QDir& dir);//Рекурсивная функция поиска файлов и запись в переменнную типа LIST
    void on_btn_add_clicked();//Выбор папки хранения аудиофайлов
    void inizilizeProgBar(int range);//Инициализация линии прогресса записи в БД
    void payPrint(QAbstractItemModel *modelTable);//Раскрашиваение элементов
    void payPrintThread();
    void payPrintEnd(QStandardItemModel *model2,int colZapGreen,int _zapInBD);
    void smsCreate();  // Создание формы "SMS"


    void onItemActivated(QModelIndex index);
    QStringList horizontalHeaderMy();//Заголовок таблицы

    void writeSql(); //для запросов в БД

    void currentTrackLabel(int index);//Для вывода в Label - currentTrack трека который воспроизводится
    void closeEvent(QCloseEvent *);

    void MainWindow2(QSqlDatabase db1); // Принимает БД
    void slotError(QString text);

signals:
    void signError(QString error);
    void signpayPrintEnd(QStandardItemModel *model2,int colZapGreen,int _zapInBD);
    void signProgresBarHidden(bool a);
    void signProgresBar(int step);
    void signInizilizeProgBar(int range);
    void combobox2click();
    void openBD(QString PATH,QSqlDatabase* db);
};

#endif // MAINWINDOW_H
