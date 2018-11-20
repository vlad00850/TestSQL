#include "database.h"

database::database(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
   // open();
    window();
}

database::~database()
{
    db.close();
}

void database::open(QString PATH, QSqlDatabase *db1)
{
    qDebug() << PATH;
    db.setDatabaseName(PATH);
    if(db.open())
    {
        qDebug() << "Успешно открылась БД";
    }
    else {
        qDebug() << "Ошибка = " << db.lastError().text();
    }

    db1 = &db;
    qDebug() << db1->databaseName() << "Имя базы database";
}

void database::window()
{
    MainWindow *window = new MainWindow(db);
    window->show(); // Запуск созданной формы
    connect(window, SIGNAL(openBD(QString, QSqlDatabase*)), this, SLOT(open(QString,QSqlDatabase*)));// Сигнал открытия бд из класа MainWindow
}

