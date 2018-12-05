#include "database.h"

databaseMy::databaseMy(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
}

databaseMy::~databaseMy()
{
    db.close();
}
QSqlDatabase databaseMy::getDb() const //Получение базы данных
{
    return db;
}

void databaseMy::setIsClosedBdBd(bool value) //Значение работы метода bdbd()
{
    isClosedBdBd = value;
}

QAbstractItemModel*databaseMy::getAbstractModel() const
{
    return abstractModel;
}

bool databaseMy::open(QString PATH)
{
    //qDebug() << PATH;
    db.setDatabaseName(PATH);
    if(db.open())
    {
        qDebug() << "Успешно открылась БД:" << db.databaseName();
        return true;
    }
    else {
        qDebug() << "Ошибка = " << db.lastError().text();
         return false;
    }
}

QAbstractItemModel* databaseMy::openTable(QString path)
{
    int i=0;
    QSqlTableModel *SqlTabMod = new QSqlTableModel();
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *item = new QStandardItem();
    QSqlQuery query;

    // model->clear();
   // qDebug() << QDir(path).dirName();
    if(QDir(path).dirName() == "CallVoiceRecorder.db")
    {
        // query.exec("SELECT _id, NameFile, PhoneSubscr, NameSubscr, DateTimeRec, CallType, DurationRec, FileSize FROM CALL_RECORDS");
        SqlTabMod->setTable("CALL_RECORDS");
        SqlTabMod->select();
        SqlTabMod->removeColumn(2);//Удаление всей колонки
        SqlTabMod->removeColumn(5);
        SqlTabMod->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        SqlTabMod->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя файла"));
        SqlTabMod->setHeaderData(2, Qt::Horizontal, QObject::tr("Размер байт"));
        SqlTabMod->setHeaderData(3, Qt::Horizontal, QObject::tr("Номер тел."));
        SqlTabMod->setHeaderData(4, Qt::Horizontal, QObject::tr("Имя контакта"));
        SqlTabMod->setHeaderData(5, Qt::Horizontal, QObject::tr("Продолжительность с."));
        SqlTabMod->setHeaderData(6, Qt::Horizontal, QObject::tr("Дата"));
        SqlTabMod->setHeaderData(7, Qt::Horizontal, QObject::tr("Тип вызова"));

        //Прокрутка для tableView - SqlTabOtpusk
        while (SqlTabMod->canFetchMore())
            SqlTabMod->fetchMore();
        abstractModel = SqlTabMod;
        return SqlTabMod;

    }
    else if(QDir(path).dirName() == "RecorderDb.db")
    {
        // query.exec("SELECT name, size, length, created, modified, callrecordimsi FROM recorder");
        SqlTabMod->setTable("recorder");
        SqlTabMod->select();

        //Прокрутка для tableView - SqlTabOtpusk
        while (SqlTabMod->canFetchMore())
            SqlTabMod->fetchMore();
        abstractModel = SqlTabMod;
        return SqlTabMod;
    }
    else if(QDir(path).dirName() == "CallRecProg.db")
    {
        //query.exec("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record");
        SqlTabMod->setTable("record");
        SqlTabMod->select();
        SqlTabMod->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        SqlTabMod->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя файла"));
        SqlTabMod->setHeaderData(2, Qt::Horizontal, QObject::tr("Имя контакта"));
        SqlTabMod->setHeaderData(3, Qt::Horizontal, QObject::tr("Номер тел."));
        SqlTabMod->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата"));
        SqlTabMod->setHeaderData(5, Qt::Horizontal, QObject::tr("Номер модифицированный."));
        SqlTabMod->setHeaderData(6, Qt::Horizontal, QObject::tr("Размер байт"));
        SqlTabMod->setHeaderData(7, Qt::Horizontal, QObject::tr("Продолжительность с."));
        SqlTabMod->setHeaderData(8, Qt::Horizontal, QObject::tr("Тип вызова"));
        SqlTabMod->setHeaderData(9, Qt::Horizontal, QObject::tr("Комментарий"));
        SqlTabMod->setHeaderData(10, Qt::Horizontal, QObject::tr("Приоритет"));
        SqlTabMod->setHeaderData(11, Qt::Horizontal, QObject::tr("Created"));
        SqlTabMod->setHeaderData(12, Qt::Horizontal, QObject::tr("Modificated"));

        //Прокрутка для tableView
        while (SqlTabMod->canFetchMore())
            SqlTabMod->fetchMore();
        abstractModel = SqlTabMod;
        return SqlTabMod;
    }
    else
        if(QDir(path).dirName() == "contacts2.db")
        {
                SqlTabMod->setTable("calls");
                SqlTabMod->select();
                SqlTabMod->removeColumn(1);//Удаление всей колонки
                SqlTabMod->removeColumn(1);//Удаление всей колонки
                SqlTabMod->removeColumn(3);//Удаление всей колонки
                SqlTabMod->removeColumn(4);//Удаление всей колонки
                SqlTabMod->removeColumn(4);//Удаление всей колонки
                SqlTabMod->removeColumn(4);//Удаление всей колонки
                SqlTabMod->removeColumn(4);//Удаление всей колонки
                SqlTabMod->removeColumn(4);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(5);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(6);//Удаление всей колонки
                SqlTabMod->removeColumn(7);//Удаление всей колонки
                SqlTabMod->removeColumn(7);//Удаление всей колонки
                SqlTabMod->removeColumn(7);//Удаление всей колонки
                SqlTabMod->removeColumn(7);//Удаление всей колонки
                SqlTabMod->removeColumn(7);//Удаление всей колонки
                //qDebug() << SqlTabMod->record().fieldName(2);

                SqlTabMod->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
                SqlTabMod->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"));
                SqlTabMod->setHeaderData(2, Qt::Horizontal, QObject::tr("Продолжительность с."));
                SqlTabMod->setHeaderData(3, Qt::Horizontal, QObject::tr("Тип вызова"));
                SqlTabMod->setHeaderData(4, Qt::Horizontal, QObject::tr("Имя контакта"));
                SqlTabMod->setHeaderData(5, Qt::Horizontal, QObject::tr("Номер тел"));
                SqlTabMod->setHeaderData(6, Qt::Horizontal, QObject::tr("record_uuid"));

                while (SqlTabMod->canFetchMore())
                    SqlTabMod->fetchMore();


                abstractModel = SqlTabMod; //ИЗМЕНЕНИЯ ПРОИСХОДЯТ СРАЗУ В БАЗЕ

                //Обрезаем у поля date 3 последних цифры
//                QModelIndex modelIndex;
//                QAbstractItemModel *modelAbstract;

//                for(int i = 0; i < abstractModel->rowCount(); i++)
//                {
//                    modelIndex = abstractModel->index(i,1);
//                    qDebug() << abstractModel->data(modelIndex).toString().count();
//                    if(abstractModel->data(modelIndex).toString().count()==13){
//                        abstractModel->setData(modelIndex,abstractModel->data(modelIndex).toString().remove(10,3));
//                    }
//                }

                //Обрезаем у поля date 3 последних цифры
//                QModelIndex modelIndex;
//                QAbstractItemModel *modelAbstract;

//                for(int i = 0; i < SqlTabMod->rowCount(); i++)
//                {
//                    for(int j = 0; j < 6; j++)
//                    {
//                        modelIndex = SqlTabMod->index(i,1);
//                        qDebug() << abstractModel->data(modelIndex).toString().count();
//                        if(abstractModel->data(modelIndex).toString().count()==13){
//                            abstractModel->setData(modelIndex,abstractModel->data(modelIndex).toString().remove(10,3));
//                        }
//                        modelAbstract->setData(modelAbstract->index(i,j),abstractModel->data(modelIndex).toString().remove(10,3))
//                    }
//                }

//                QString id = query.value(0).toString();
//                item = new QStandardItem(QString(id));
//                model->setItem(i,1,item);

//                QString name = query.value(1).toString();
//                item = new QStandardItem(QString(name));
//                model->setItem(i,3,item);

//                QString normalized_number = query.value(2).toString();
//                item = new QStandardItem(QString(normalized_number));
//                model->setItem(i,4,item);

//                QString date = query.value(3).toString().remove(9,3);
//                item = new QStandardItem(QString(date));
//                model->setItem(i,5,item);

//                QString duration = query.value(4).toString();
//                item = new QStandardItem(QString(duration));
//                model->setItem(i,8,item);

//                QString type = query.value(5).toString();
//                item = new QStandardItem(QString(type));
//                model->setItem(i,9,item);

//                QString record_uuid = query.value(6).toString();
//                item = new QStandardItem(QString(record_uuid));
//                model->setItem(i,15,item);

//                i++;
//            }
            //abstractModel = SqlTabMod;
            return abstractModel;
        }
        else
        {
            qDebug() << "Ошибка" <<path;
            return NULL;
        }
}


QStringList databaseMy::horizontalHeaderMy() //Заголовок таблицы
{
    QStringList horizontalHeader;
    // horizontalHeader.append("111");
    horizontalHeader.append("ID");
    horizontalHeader.append("Имя файла");
    horizontalHeader.append("Имя контакта");
    horizontalHeader.append("Номер тел.");
    horizontalHeader.append("Дата");
    horizontalHeader.append("Номер модифицированный.");
    horizontalHeader.append("Размер байт");
    horizontalHeader.append("Продолжительность с.");
    horizontalHeader.append("Тип вызова");
    horizontalHeader.append("Комментарий");
    horizontalHeader.append("Приоритет");
    horizontalHeader.append("created");
    horizontalHeader.append("modific");
    horizontalHeader.append("SIM");
    horizontalHeader.append("record_uuid");
    return horizontalHeader;
}

 //***************миграция базы данных contacts2 и RecorderDb в одну модель************//
QAbstractItemModel*databaseMy::migrationMeizuBD(QAbstractItemModel*contactsModel, QAbstractItemModel*RecorderDbModel)
{
    QSqlQuery RecorderQuery(db);
    qint64 qwe=0;

    //RecorderDbModel->setHorizontalHeaderLabels(horizontalHeaderMy()); //Заголовок таблицы
    for(int i = 0; i < contactsModel->rowCount(); i++)
    {
        RecorderQuery.prepare("SELECT name, size, created, modified, callrecordimsi FROM recorder WHERE callrecordid = :record_uuid");
        qwe=contactsModel->data(contactsModel->index(i,6)).toLongLong();//record_uuid записывается в эту переменную из модели
        if(qwe==0)
            continue;
        //qDebug() <<qwe;
        RecorderQuery.bindValue(":record_uuid", qwe); // поиск переменной в базе данных RecordDB
        RecorderQuery.exec();
        RecorderQuery.first();
        if(!RecorderQuery.value(0).isNull()) // Если значения есть то...
        {
            contactsModel->setData(contactsModel->index(i,2),RecorderQuery.value(0).toString());//name
            contactsModel->setData(contactsModel->index(i,7),RecorderQuery.value(1).toString());//size
            contactsModel->setData(contactsModel->index(i,12),RecorderQuery.value(2).toString());//modified
            contactsModel->setData(contactsModel->index(i,13),RecorderQuery.value(3).toString());//created
            contactsModel->setData(contactsModel->index(i,14),RecorderQuery.value(4).toString());//callrecordimsi
        }
    }
    return RecorderDbModel;

//    SqlTabMod->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
//    SqlTabMod->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"));
//    SqlTabMod->setHeaderData(2, Qt::Horizontal, QObject::tr("Продолжительность с."));
//    SqlTabMod->setHeaderData(3, Qt::Horizontal, QObject::tr("Тип вызова"));
//    SqlTabMod->setHeaderData(4, Qt::Horizontal, QObject::tr("Имя контакта"));
//    SqlTabMod->setHeaderData(5, Qt::Horizontal, QObject::tr("Номер тел"));
//    SqlTabMod->setHeaderData(6, Qt::Horizontal, QObject::tr("record_uuid"));
}

//Перенос из модели в базу данных
//isClosedBdBd сигнал остановки
void databaseMy::bdbd(QAbstractItemModel*tabelModel, QString nameBD)
{
    isClosedBdBd = false;
    QString numb="";
    int m_nStep=0;

    QString PATH =  QDir::current().absolutePath() + "/CallRecProg.db"; //путь к главной БД

    //**********Поиск последнего id элемента базы CallRecProg.db****************//
    if(!(this->open(PATH))) //Открытие главной базы данных
        return;
    QSqlQuery CallRecquery(db);
    int CallRecID=0;
    CallRecquery.exec("SELECT id FROM record");
    while (CallRecquery.next())
        CallRecID=CallRecquery.value(0).toInt();
    //*************************************************************************//
    qDebug()<< "Последний элемент"<< CallRecID;//Вывод последненго элемента

    if(nameBD == "CallVoiceRecorder.db")
    {
        for(int i = 0; i < tabelModel->rowCount() && !isClosedBdBd; i++)
        {
            QString nameFile="";
            ++m_nStep;

            if(m_nStep%50==0)
                emit signProgresBar(m_nStep);

            CallRecquery.prepare("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE nameFile LIKE :nameFile");
            nameFile=tabelModel->data(tabelModel->index(i,1)).toString();//nameFile записывается в эту переменную из модели
            //            if(nameFile==NULL)
            //                continue;
            qDebug() << nameFile;
            CallRecquery.bindValue(":nameFile", nameFile);// поиск переменной в базе данных record
            CallRecquery.exec();
            CallRecquery.first();

            if(CallRecquery.value(0).isNull()) // Если значения нет то записываем в базу
            {
                CallRecID++;
                CallRecquery.prepare("insert into record(id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid)"
                                     "values(:id, :nameFile, :namePeople, :number, :dateTimeRec, :numberModif, :fileSize, :fileLenght, :callType, :comment, :priority, :modified, :created, :sim, :record_uuid)");
                CallRecquery.bindValue(":id", CallRecID);
                CallRecquery.bindValue(":nameFile", tabelModel->data(tabelModel->index(i,1)).toString());
                CallRecquery.bindValue(":namePeople", tabelModel->data(tabelModel->index(i,4)).toString());
                CallRecquery.bindValue(":number", tabelModel->data(tabelModel->index(i,3)).toString());

                //******************Приведение даты к unix формату
                QDateTime dt;
                dt = dt.fromString(tabelModel->data(tabelModel->index(i,6)).toString(),"yyyy-MM-dd HH:mm:ss");
                uint unix=dt.toTime_t(); //в UNIX формат
                CallRecquery.bindValue(":dateTimeRec", QString::number(unix));
                //******************

                //проверка записи телефона
                numb=tabelModel->data(tabelModel->index(i,3)).toString();
                if((numb.count()>10))
                    if( !((numb[0]=='+') && (numb[1]=='8')))
                        if((numb[0]=='+') && (numb[1]=='7'))
                            numb.remove(0,2);
                        else
                            if((numb[0]=='8') && (numb.count()>10))
                                numb.remove(0,1);
                CallRecquery.bindValue(":numberModif", numb);
                CallRecquery.bindValue(":fileSize", tabelModel->data(tabelModel->index(i,2)).toInt());
                CallRecquery.bindValue(":fileLenght", tabelModel->data(tabelModel->index(i,5)).toInt());
                CallRecquery.bindValue(":callType", tabelModel->data(tabelModel->index(i,7)).toInt());
                CallRecquery.exec();
            }
        }
    }
    else
    {
        for(int i = 0; i < tabelModel->rowCount() && !isClosedBdBd; i++)
        {
            QString nameFile="";
            QString dateTimeRec="";
            ++m_nStep;
            if(m_nStep%50==0)
                emit signProgresBar(m_nStep);
            //qDebug()<< CallRecID;

            //Проверяет на повторяющиеся значения по nameFile и dateTimeRec, если не повторяется то записывает в базу данных
            CallRecquery.prepare("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE nameFile LIKE :nameFile or dateTimeRec LIKE :dateTimeRec");
            nameFile=tabelModel->data(tabelModel->index(i,1)).toString();//nameFile записывается в эту переменную из модели
            dateTimeRec=tabelModel->data(tabelModel->index(i,7)).toString();
            //            if(nameFile==NULL)
            //                continue;
            CallRecquery.bindValue(":nameFile", nameFile);// поиск переменной в базе данных RecordDB
            CallRecquery.bindValue(":dateTimeRec", dateTimeRec);// поиск переменной в базе данных RecordDB
            CallRecquery.exec();
            CallRecquery.first();

            if(CallRecquery.value(0).isNull()) // Если значения нет то записываем в базу
            {
                CallRecID++;
                CallRecquery.prepare("insert into record(id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid)"
                                     "values(:id, :nameFile, :namePeople, :number, :dateTimeRec, :numberModif, :fileSize, :fileLenght, :callType, :comment, :priority, :modified, :created, :sim, :record_uuid)");
                CallRecquery.bindValue(":id", CallRecID);
                CallRecquery.bindValue(":nameFile", tabelModel->data(tabelModel->index(i,1)).toString());
                CallRecquery.bindValue(":namePeople", tabelModel->data(tabelModel->index(i,3)).toString());
                CallRecquery.bindValue(":number", tabelModel->data(tabelModel->index(i,4)).toString());
                CallRecquery.bindValue(":dateTimeRec", tabelModel->data(tabelModel->index(i,5)).toString());
                //проверка записи телефона
                numb=tabelModel->data(tabelModel->index(i,4)).toString();
                if((numb.count()>10))
                    if( !((numb[0]=='+') && (numb[1]=='8')))
                        if((numb[0]=='+') && (numb[1]=='7'))
                            numb.remove(0,2);
                        else
                            if((numb[0]=='8') && (numb.count()>10))
                                numb.remove(0,1);
                CallRecquery.bindValue(":numberModif", numb);
                CallRecquery.bindValue(":fileSize", tabelModel->data(tabelModel->index(i,7)).toInt());
                CallRecquery.bindValue(":fileLenght", tabelModel->data(tabelModel->index(i,8)).toInt());
                CallRecquery.bindValue(":callType", tabelModel->data(tabelModel->index(i,9)).toInt());
                CallRecquery.bindValue(":comment", tabelModel->data(tabelModel->index(i,10)).toString());
                CallRecquery.bindValue(":priority", tabelModel->data(tabelModel->index(i,11)).toInt());
                CallRecquery.bindValue(":sim", tabelModel->data(tabelModel->index(i,14)).toString());
                CallRecquery.bindValue(":modified",tabelModel->data(tabelModel->index(i,13)).toInt());
                CallRecquery.bindValue(":created", tabelModel->data(tabelModel->index(i,12)).toInt());
                CallRecquery.bindValue(":record_uuid", tabelModel->data(tabelModel->index(i,15)).toLongLong());
                CallRecquery.exec();
            }
        }
    }
    emit signProgresBar(0);
    emit sendMessage("Перенос в БД выполнен успешно!");
}
