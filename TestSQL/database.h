#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "QtSql/QSqlDatabase"

#include "QtSql/QSqlTableModel"
#include "QtSql"
#include "QAbstractItemModel"
#include "QStandardItemModel"

class databaseMy : public QObject
{
    Q_OBJECT
public:
    explicit databaseMy(QObject *parent = 0);
    ~databaseMy();
    QSqlDatabase db;
    QSqlQueryModel SqlWrite;
    QAbstractItemModel* abstractModel;
    QStandardItemModel* contactsStandartModel;
    QStandardItemModel* contactsStandartSomeModels, *recorderStandartSomeModels; //Для множенственного добавления в баз contacts и RecorderDB

    QAbstractItemModel*getAbstractModel() const;

    QSqlDatabase getDb() const;
    bool isClosedBdBd;
    void setIsClosedBdBd(bool value);

signals:
    void signProgresBar(int step);
    void sendMessage(QString text);

public slots:
    bool open(QString PATH);
    QAbstractItemModel* openTable(QString path);
    QStringList horizontalHeaderMy();
    QAbstractItemModel* migrationMeizuBD(QAbstractItemModel* contactsModel, QAbstractItemModel* RecorderDbModel);
    void bdbd(QAbstractItemModel* tabelModel, QString nameBD);
    void openSomeTables(QStandardItemModel* contacts, QStandardItemModel* record, QStringList vecPath);

};

#endif // DATABASE_H
