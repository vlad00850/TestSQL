#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "QtSql/QSqlDatabase"

#include "QtSql/QSqlTableModel"
#include "QtSql"
#include "QAbstractItemModel"

#include "mainwindow.h"

class database : public QObject
{
    Q_OBJECT
public:
    explicit database(QObject *parent = 0);
    ~database();
    QSqlDatabase db;
    QSqlQueryModel SqlWrite;


signals:
    void dbTransmit(QSqlDatabase db);

public slots:
    void open(QString PATH,QSqlDatabase *db1);
    void window();

};

#endif // DATABASE_H
