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

    QAbstractItemModel*getAbstractModel() const;

    QSqlDatabase getDb() const;

signals:
    void signProgresBar(int step);

public slots:
    bool open(QString PATH);
    QAbstractItemModel* openTable(QString path);
    QStringList horizontalHeaderMy();
    QAbstractItemModel* migrationMeizuBD(QAbstractItemModel* contactsModel, QAbstractItemModel* RecorderDbModel);
    void bdbd(QAbstractItemModel* tabelModel, QString nameBD);
};

#endif // DATABASE_H
