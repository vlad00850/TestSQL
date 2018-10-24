#ifndef SMSDIALOG_H
#define SMSDIALOG_H

#include <QDialog>
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlQuery"

#include <QStandardItemModel>


#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

#include "QStandardItem"
#include "QSqlTableModel" //удалить

#include "time.h"
#include "ctime"
#include <string>
#include "QtConcurrent/QtConcurrent"

#include <QCloseEvent>
#include <synchapi.h> //для Sleep();

namespace Ui {
class smsDialog;
}

class smsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit smsDialog(QSqlDatabase db, QString PATH, QWidget *parent = 0);
    ~smsDialog();

    QStandardItemModel *model;
    QSqlQueryModel *contact ;
    QSqlQueryModel *contact2;
    QStandardItemModel *model2;
    QStandardItem *item;
    QString FileDir;//папка с выбранными файлами
    bool isClosed; // закрытие приложения

private:
    Ui::smsDialog *ui;
    QSqlDatabase _db;
    QString _PATH;

private slots:
    void SelectDataBase();
    void Load();
    void Zapis(QSqlQuery query, int j);
    void closeEvent(QCloseEvent *);
    void Clear();
    void bdbd();
   // void progressBar(int step);
    void inizilizeProgBar(int range);
    QString dateCreate(QString time1); //сделать статической?
    QStringList horizontalHeaderMy();
    void FindRecord(QString textSort);//Поиск записи
    void InOutCall(); //Входящие-исходящие

    void getId(QModelIndex index);
    void writeSql();

    void contacts();
    void on_findOk_clicked();

signals:
    void signProgresBar(int step);
    void signInizilizeProgBar(int range);

};

#endif // SMSDIALOG_H
