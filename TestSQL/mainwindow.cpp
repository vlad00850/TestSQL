#include "mainwindow.h"
#include "ui_mainwindow.h"



QString PATH = QDir::current().absolutePath() + "/CallRecProg.db"; //путь к главной БД


//data/data/com.android.soundrecorder/databases/recorded.db
//data/data/com.android.providers.contacts/databases/contacts2.db
//в самом начале выбирать базу callRecProg.db для глобального и не записывать в модель а сразу преобразовывать
//Все аудиозаписи которые не нашли контакт записать в таблицу

//Раскрашивание из query модели добавить

//QString FileDir="";//папка с выбранными файлами
QStandardItemModel *model = new QStandardItemModel;

QStandardItem *item;
bool isClosed=false; // закрытие приложения

MainWindow::MainWindow(QSqlDatabase db1 , QWidget *parent) :
    db(db1),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    SqlWrite = new QSqlQueryModel;
    SqlTabMod= new QSqlTableModel;

    Audio = new audio();
    ui->playlistView->setModel(Audio->m_playListModel);

    connect(ui->btn_previous, SIGNAL(clicked()),Audio->m_playlist,SLOT(previous()));
    connect(ui->btn_next, SIGNAL(clicked()),Audio->m_playlist,SLOT(next()));
    connect(ui->btn_play, SIGNAL(clicked()),Audio->m_player,SLOT(play()));
    connect(ui->btn_pause, SIGNAL(clicked()),Audio->m_player,SLOT(pause()));
    connect(ui->btn_stop, SIGNAL(clicked()),Audio->m_player,SLOT(stop()));

    //**********************Звук****************************
    ui->horizontalSlider->setRange(0,100);//Задаем, что с 0 до 100 будет передвигаться ползунок
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),Audio->m_player,SLOT(setVolume(int)));//Громкость воспроизведения и горизонтальный ползунок
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),ui->volumLabel,SLOT(setNum(int)));//Громкость воспроизведения и запись в label
    ui->horizontalSlider->setValue(50);//Начальное значение ползунка
    //********************Ползунок воспроизмеделния трека***********************************
    connect(Audio->m_player, SIGNAL(positionChanged(qint64)), this, SLOT(setMusicRange(qint64)));//Передача параметра позиции записи по времени
    connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)),Audio,SLOT(setPosition(int)));//Прокрутка времени
    //**************************************************************************************

    //Для записи в главную таблицу
    connect(ui->pushButton_3, SIGNAL(clicked()), SLOT(migrationMeizu()));
    connect(this, SIGNAL(signInizilizeProgBar(int)), SLOT(inizilizeProgBar(int)));
    connect(this, SIGNAL(signProgresBar(int)),ui->progressBar, SLOT(setValue(int)));
    connect(ui->btn_paint, SIGNAL(clicked()), SLOT(payPrint()));
    connect(ui->actionSMs,SIGNAL(triggered()), SLOT(smsCreate()));

    ui->progressBar->setVisible(false);
    ui->progressBar->setAlignment(Qt::AlignCenter);

    //**************Сортировки, поиски****************************
    connect(ui->lineEdit,SIGNAL(textChanged(QString)), SLOT(FindRecord(QString))); //при изменении текста, значение передается в фун-ию сортировки
    connect(ui->findOk,SIGNAL(clicked()), SLOT(click_findOk())); //при изменении текста, значение передается в фун-ию сортировки
    ui->pushButton_2->setEnabled(false);
    //Сортировка по нажатию на заголовок, релизовать в будущем
    // connect(ui->tableView->horizontalHeader(),&QHeaderView::sectionClicked, this, [=](int logicalIndex) );
    //  connect(ui->tableView->horizontalHeader(),SIGNAL(QHeaderView::sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(QTableView::sortByColumn(int, Qt::SortOrder)));
    //*******comboBox сортировки**************
    QStringList findList;
    findList.append("По номеру телефона");
    findList.append("По имени контакта");
    findList.append("По дате");
    ui->comboBox->addItems(findList);
    //**********************************
    ui->findPar2->setVisible(false);

    //*******comboBox_2 сортировки(скрытый) только для работы с датами**************
    findList.clear();
    findList.append("=");
    findList.append(">");
    findList.append("<");
    findList.append("Между");
    ui->comboBox_2->addItems(findList);
    ui->comboBox_2->setVisible(false);
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), SIGNAL(combobox2click()));//Выбирая  > < или = сразу
    connect(this, SIGNAL(combobox2click()), SLOT(click_findOk()));//                                            же выполняется запрос
    //***************Входящ-иходящие********************************
    connect(ui->radioButton, SIGNAL(clicked()), SLOT(InOutCall()));
    connect(ui->radioButton_2, SIGNAL(clicked()), SLOT(InOutCall()));
    connect(ui->radioButton_3, SIGNAL(clicked()), SLOT(InOutCall()));
    connect(ui->pushButton, SIGNAL(clicked()), SLOT(SelectDataBase()));
    //**********************************

    connect(ui->playlistView, SIGNAL(doubleClicked(QModelIndex)),Audio,SLOT(setCurrentIndex(QModelIndex))); // Двойной клик на представление playlistView и запуск трека
    connect(Audio->m_playlist, SIGNAL(currentIndexChanged(int)),this, SLOT(currentTrackLabel(int)));//Для вывода в Label - currentTrack трека который воспроизводится

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(getId(QModelIndex)));
    connect(ui->pushButton_2, SIGNAL(clicked()), SLOT(bdbdThread()));//Перенос из model в БД

    connect(this,SIGNAL(signProgresBarHidden(bool)),ui->progressBar, SLOT(setVisible(bool)));//Скрывает progressBar после переноса в БД

    ui->lineEdit_2->setText("SELECT * FROM record ORDER BY namePeople ASC;"); //lineEdit для запросов
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(writeSql()));// Для запросов к БД

    ui->statusBar->addPermanentWidget(ui->progressBar,1); // progressBar в нижнюю строку(  statusBar)



//    //*************************************************8
//    //для записи в бд
//    QSqlRecord record = SqlTabMod->record(1);
//    QString text = record.value(1).toString();
//    QString text2 = record.value(2).toString();
//    qDebug() << text;
//    qDebug() << text2;

//    //*************************************************8

}

//Для вывода в Label - currentTrack трека который воспроизводится
void MainWindow::currentTrackLabel(int index)
{
    ui->currentTrack->setText(Audio->m_playListModel->data(Audio->m_playListModel->index(index,0)).toString());
}

//***********При двойном клике запускается файл аудиозаписи*******************
void MainWindow::getId(QModelIndex row)
{
    QModelIndex result;
    QString text = row.sibling(row.row(),2).data(Qt::DisplayRole).toString(); //Текстовая строка выбранной строки (имя файла)
    //Поиск по модели m_playListModel, где храняться файлы
    for(int i = 0; i< Audio->m_playListModel->rowCount(); i++)
    {
        if(text==Audio->m_playListModel->item(i,0)->text())
        {
            result=Audio->m_playListModel->indexFromItem(Audio->m_playListModel->item(i,0));
            Audio->setCurrentIndex(result);//Передача в функцию воспроизведения трека
            ui->label_3->setText(Audio->m_playListModel->item(i,1)->text());
        }
    }
}

//НЕ РАБОТАЕТб сортировка по столбцам
void MainWindow::onItemActivated(QModelIndex index)
{

}


//Выбор базы данных
void MainWindow::SelectDataBase()
{
    QString files1 = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных", QString(), "Data basef ile (*.db)");
    if(files1=="")
        emit openBD(PATH,&db);
    else
        emit openBD(files1,&db);

   model->clear();
    QSqlQuery query;
    model->setHorizontalHeaderLabels(horizontalHeaderMy()); //Заголовок таблицы

    if(QDir(db.databaseName()).dirName() == "CallVoiceRecorder.db")
    {
        query.exec("SELECT _id, NameFile, PhoneSubscr, NameSubscr, DateTimeRec, CallType, DurationRec, FileSize FROM CALL_RECORDS");
        ui->pushButton_2->setEnabled(true);
        Zapis(0, query);

        QMessageBox ms;
        QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
        QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
        ms.setText("Cразу записать в БД приложения (CallRecProg.db)?");
        ms.exec();
        if(ms.clickedButton()==yes)
        {
            ui->pushButton_2->setEnabled(false);
            QtConcurrent::run(this,&MainWindow::bdbd);
        }
        if(ms.clickedButton()==no)
            ui->pushButton_2->setEnabled(true);

    }
    else if(QDir(db.databaseName()).dirName() == "RecorderDb.db")
    {
        query.exec("SELECT name, size, length, created, modified, callrecordimsi FROM recorder");
        Zapis(1,query);
    }
    else if(QDir(db.databaseName()).dirName() == "CallRecProg.db")
    {
       // query.exec("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record");
        Zapis(2,query);
    }
    else
        if(QDir(db.databaseName()).dirName() == "contacts2.db")
        {
            query.exec("SELECT _id, name, normalized_number, date, duration, type, record_uuid FROM calls");
            Zapis(3,query);
        }
        else
        {
            QMessageBox::critical(NULL, "Ошибка", "Название базы не соответствует");
            qDebug() << QDir(db.databaseName()).dirName();
            return;
        }
}

//***************Вывод на экран
void MainWindow::Zapis(int j, QSqlQuery query)
{
    int i=0;

    //CallVoiceRecorder.db
    if(j==0)
    {
        SqlTabMod->setTable("CALL_RECORDS");
        SqlTabMod->select();
        SqlTabMod->removeColumn(2);
        SqlTabMod->removeColumn(6);
        SqlTabMod->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        SqlTabMod->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя файла"));
        SqlTabMod->setHeaderData(2, Qt::Horizontal, QObject::tr("Размер байт"));
        SqlTabMod->setHeaderData(3, Qt::Horizontal, QObject::tr("Номер тел."));
        SqlTabMod->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата"));
        SqlTabMod->setHeaderData(5, Qt::Horizontal, QObject::tr("Номер модифицированный."));
        SqlTabMod->setHeaderData(6, Qt::Horizontal, QObject::tr("Продолжительность с."));
        SqlTabMod->setHeaderData(7, Qt::Horizontal, QObject::tr("Имя контакта"));
        SqlTabMod->setHeaderData(8, Qt::Horizontal, QObject::tr("Тип вызова"));
        SqlTabMod->setHeaderData(9, Qt::Horizontal, QObject::tr("Комментарий"));
        SqlTabMod->setHeaderData(10, Qt::Horizontal, QObject::tr("123"));
        SqlTabMod->setHeaderData(11, Qt::Horizontal, QObject::tr("123"));
        SqlTabMod->setHeaderData(12, Qt::Horizontal, QObject::tr("123"));

        //Прокрутка для tableView - SqlTabOtpusk
        while (SqlTabMod->canFetchMore())
            SqlTabMod->fetchMore();

        ui->tableView->setModel(SqlTabMod);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
        ui->tableView ->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения



        ui->statusBar->showMessage(QString("Кол-во записей :%1")
                                   .arg( SqlTabMod->rowCount()), 0 );

        //            //Приведение даты к unix формату
        //            QDateTime dt;
        //            dt = dt.fromString(query.value(4).toString(),"yyyy-MM-dd HH:mm:ss");
        //                       uint unix=dt.toTime_t(); //в UNIX формат
        //            item = new QStandardItem(QString::number(unix));
        //            model->setItem(i,5,item);


    }
    else if(j==1) //RecorderDb.db
    {

        SqlTabMod->setTable("recorder");
        SqlTabMod->select();

        ui->tableView->setModel(SqlTabMod);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
        ui->tableView ->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения

        //Прокрутка для tableView - SqlTabOtpusk
        while (SqlTabMod->canFetchMore())
            SqlTabMod->fetchMore();

        ui->statusBar->showMessage(QString("Кол-во записей :%1")
                                   .arg( SqlTabMod->rowCount()), 0 );

    }
    else if(j==2) //CallRecProg.db
    {
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

        ui->tableView->setModel(SqlTabMod);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
        ui->tableView ->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения

        //Прокрутка для tableView
        while (SqlTabMod->canFetchMore())
            SqlTabMod->fetchMore();

        ui->statusBar->showMessage(QString("Кол-во записей :%1")
                                   .arg( SqlTabMod->rowCount()), 0 );

        //Представление времени в формате mm:ss
        //            QString FileLenght = query.value(7).toString();
        //            QTime displayTime(0, (FileLenght.toInt()/60) % 60, (FileLenght.toInt()) % 60);
        //            item = new QStandardItem(QString(displayTime.toString("mm:ss")));
        //            model->setItem(i,8,item);
    }
    else if(j==3) //contacts2.db
    {
        while (query.next())
        {
            QString id = query.value(0).toString();
            item = new QStandardItem(QString(id));
            model->setItem(i,1,item);

            QString name = query.value(1).toString();
            item = new QStandardItem(QString(name));
            model->setItem(i,3,item);

            QString normalized_number = query.value(2).toString();
            item = new QStandardItem(QString(normalized_number));
            model->setItem(i,4,item);

            QString date = query.value(3).toString().remove(9,3);
            item = new QStandardItem(QString(date));
            model->setItem(i,5,item);

            QString duration = query.value(4).toString();
            item = new QStandardItem(QString(duration));
            model->setItem(i,8,item);

            QString type = query.value(5).toString();
            item = new QStandardItem(QString(type));
            model->setItem(i,9,item);

            QString record_uuid = query.value(6).toString();
            item = new QStandardItem(QString(record_uuid));
            model->setItem(i,15,item);

            i++;
        }
        ui->tableView->setModel(model);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
        ui->tableView->setColumnHidden(0,true);
        ui->statusBar->showMessage(QString("%1 Добавить, Кол-во записей :%2")
                                   .arg(12 ,0,'f',0)
                                   .arg( model->rowCount()), 0 );
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения
    }
}

//***************миграция базы данных contacts2 и RecorderDb в одну модель************//
void MainWindow::migrationMeizu()
{
    //Открываем contacts2.db и записываем в модель
    QString contacts2 = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных contacts2.db", QString(), "Data basef ile (*.db)");
    if(contacts2=="")
        return;
   emit openBD(contacts2,&db);
    QSqlQuery query;
    query.exec("SELECT _id, name, normalized_number, date, duration, type, record_uuid FROM calls");
    Zapis(3,query);

    //Открываем RecorderDb.db
    QString RecorderDb = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных RecorderDb.db", QString(), "Data basef ile (*.db)");
    if(RecorderDb=="")
        return;
    emit openBD(RecorderDb,&db);

    QSqlQuery RecorderQuery(db);
    qint64 qwe=0;

    model->setHorizontalHeaderLabels(horizontalHeaderMy()); //Заголовок таблицы
    for(int i = 0; i<model->rowCount(); i++)
    {
        RecorderQuery.prepare("SELECT name, size, created, modified, callrecordimsi FROM recorder WHERE callrecordid = :record_uuid");
        qwe=model->item(i,15)->text().toLongLong();//record_uuid записывается в эту переменную из модели
        if(qwe==0)
            continue;
   //     qDebug() <<qwe;
        RecorderQuery.bindValue(":record_uuid", qwe); // поиск переменной в базе данных RecordDB
        RecorderQuery.exec();
        RecorderQuery.first();
        if(!RecorderQuery.value(0).isNull()) // Если значения есть то...
        {
            item = new QStandardItem(QString(RecorderQuery.value(0).toString()));//name
            model->setItem(i,2,item);

            item = new QStandardItem(QString(RecorderQuery.value(1).toString()));//size
            model->setItem(i,7,item);

            item = new QStandardItem(QString(RecorderQuery.value(2).toString()));//modified
            model->setItem(i,12,item);

            item = new QStandardItem(QString(RecorderQuery.value(3).toString()));//created
            model->setItem(i,13,item);

            item = new QStandardItem(QString(RecorderQuery.value(4).toString()));//callrecordimsi
            model->setItem(i,14,item);
        }
    }

    QMessageBox ms;
    QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
    QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
    ms.setText("Cразу записать в БД приложения (CallRecProg.db)?");
    ms.exec();
    if(ms.clickedButton()==yes)
    {
        ui->pushButton_2->setEnabled(false);
        QtConcurrent::run(this,&MainWindow::bdbd);
    }
    if(ms.clickedButton()==no)
        ui->pushButton_2->setEnabled(true);
    ui->tableView->setModel(model);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    //ui->tableView->setColumnWidth(2,400); // одну колонку увеличить в размерах 2-колонка
    ui->tableView->setColumnHidden(0,true);
    ui->statusBar->showMessage(QString("%1 Добавить, Кол-во записей :%2")
                               .arg(12 ,0,'f',0)
                               .arg( model->rowCount()), 0);
    ui->progressBar->reset();
}


//Перенос из модели в базу данных
void MainWindow::bdbd()
{
    int  m_nStep=0;
    emit signInizilizeProgBar(model->rowCount());//Сигнал о количестве записей в модели

    QString numb="";
    bool f = false;

    //проверка на базу данных - CallVoiceRecorder.db

    if(QDir(db.databaseName()).dirName() == "CallVoiceRecorder.db")
        f = true;
    else f = false;

    //**********Поиск последнего id элемента базы CallRecProg.db****************//
    db.setDatabaseName(PATH);
    db.open();
    //emit openBD(PATH,&db);//Каталог базы данных в которую будет запись

    QSqlQuery CallRecquery(db);
    int CallRecID=0;
    CallRecquery.exec("SELECT id FROM record");
    while (CallRecquery.next())
        CallRecID=CallRecquery.value(0).toInt();
    //*************************************************************************//
    qDebug()<< "Последний элемент"<< CallRecID;//Вывод последненго элемента

    if(f)
    {
        for(int i = 0; i<SqlTabMod->rowCount() && !isClosed; i++)
        {
            QString nameFile="";
            ++m_nStep;
            if(m_nStep%50==0)
                emit signProgresBar(m_nStep);

            CallRecquery.prepare("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE nameFile LIKE :nameFile");
            nameFile=SqlTabMod->data(SqlTabMod->index(i,2)).toString();//nameFile записывается в эту переменную из модели
            //            if(nameFile==NULL)
            //                continue;
            qDebug() << nameFile;
            CallRecquery.bindValue(":nameFile", nameFile);// поиск переменной в базе данных RecordDB
            CallRecquery.exec();
            CallRecquery.first();

            if(CallRecquery.value(0).isNull()) // Если значения нет то записываем в базу
            {
                CallRecID++;
                CallRecquery.prepare("insert into record(id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid)"
                                     "values(:id, :nameFile, :namePeople, :number, :dateTimeRec, :numberModif, :fileSize, :fileLenght, :callType, :comment, :priority, :modified, :created, :sim, :record_uuid)");
                CallRecquery.bindValue(":id", CallRecID);
                CallRecquery.bindValue(":nameFile", SqlTabMod->data(SqlTabMod->index(i,2)).toString());
                CallRecquery.bindValue(":namePeople", SqlTabMod->data(SqlTabMod->index(i,3)).toString());
                CallRecquery.bindValue(":number", SqlTabMod->data(SqlTabMod->index(i,4)).toString());
                CallRecquery.bindValue(":dateTimeRec", SqlTabMod->data(SqlTabMod->index(i,5)).toString());

                //проверка записи телефона
                numb=SqlTabMod->data(SqlTabMod->index(i,4)).toString();
                if((numb.count()>10))
                    if( !((numb[0]=='+') && (numb[1]=='8')))
                        if((numb[0]=='+') && (numb[1]=='7'))
                            numb.remove(0,2);
                        else
                            if((numb[0]=='8') && (numb.count()>10))
                                numb.remove(0,1);
                CallRecquery.bindValue(":numberModif", numb);

                CallRecquery.bindValue(":fileSize", SqlTabMod->data(SqlTabMod->index(i,7)).toInt());
                CallRecquery.bindValue(":fileLenght", SqlTabMod->data(SqlTabMod->index(i,8)).toInt());
                CallRecquery.bindValue(":callType", SqlTabMod->data(SqlTabMod->index(i,9)).toInt());
                CallRecquery.bindValue(":comment", SqlTabMod->data(SqlTabMod->index(i,10)).toString());
                CallRecquery.bindValue(":priority", SqlTabMod->data(SqlTabMod->index(i,11)).toInt());
                CallRecquery.bindValue(":sim", SqlTabMod->data(SqlTabMod->index(i,14)).toString());
                CallRecquery.bindValue(":modified",SqlTabMod->data(SqlTabMod->index(i,13)).toInt());
                CallRecquery.bindValue(":created", SqlTabMod->data(SqlTabMod->index(i,12)).toInt());
                CallRecquery.bindValue(":record_uuid", SqlTabMod->data(SqlTabMod->index(i,15)).toLongLong());
                CallRecquery.exec();
            }
        }
    }
    else
    {
        for(int i = 0; i<model->rowCount() && !isClosed; i++)
        {
            QString nameFile="";
            QString dateTimeRec="";
            ++m_nStep;
            if(m_nStep%50==0)
                emit signProgresBar(m_nStep);
            //qDebug()<< CallRecID;

            //Проверяет на повторяющиеся значения по nameFile и dateTimeRec, если не повторяется то записывает в базу данных
            CallRecquery.prepare("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE nameFile LIKE :nameFile or dateTimeRec LIKE :dateTimeRec");
            nameFile=model->data(model->index(i,2)).toString();//nameFile записывается в эту переменную из модели
            dateTimeRec=model->data(model->index(i,5)).toString();
            //            if(nameFile==NULL)
            //                continue;
            // qDebug() << nameFile;
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
                CallRecquery.bindValue(":nameFile", model->data(model->index(i,2)).toString());
                CallRecquery.bindValue(":namePeople", model->data(model->index(i,3)).toString());
                CallRecquery.bindValue(":number", model->data(model->index(i,4)).toString());
                CallRecquery.bindValue(":dateTimeRec", model->data(model->index(i,5)).toString());
                //проверка записи телефона
                numb=model->data(model->index(i,4)).toString();
                if((numb.count()>10))
                    if( !((numb[0]=='+') && (numb[1]=='8')))
                        if((numb[0]=='+') && (numb[1]=='7'))
                            numb.remove(0,2);
                        else
                            if((numb[0]=='8') && (numb.count()>10))
                                numb.remove(0,1);
                CallRecquery.bindValue(":numberModif", numb);
                CallRecquery.bindValue(":fileSize", model->data(model->index(i,7)).toInt());
                CallRecquery.bindValue(":fileLenght", model->data(model->index(i,8)).toInt());
                CallRecquery.bindValue(":callType", model->data(model->index(i,9)).toInt());
                CallRecquery.bindValue(":comment", model->data(model->index(i,10)).toString());
                CallRecquery.bindValue(":priority", model->data(model->index(i,11)).toInt());
                CallRecquery.bindValue(":sim", model->data(model->index(i,14)).toString());
                CallRecquery.bindValue(":modified",model->data(model->index(i,13)).toInt());
                CallRecquery.bindValue(":created", model->data(model->index(i,12)).toInt());
                CallRecquery.bindValue(":record_uuid", model->data(model->index(i,15)).toLongLong());
                CallRecquery.exec();
            }
        }
    }
    emit signProgresBar(0);
     //QMessageBox::information(NULL, "Сообщение", "Перенос в БД выполнен успешно!");

     signProgresBarHidden(false);//скрывает progressBar
    // ui->progressBar->setVisible(false);
}

//*********************Входящий или исходящий сортировка******************//
void MainWindow::InOutCall()
{
    QString query;

    if(ui->radioButton->isChecked())
       query = "SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE callType=1";
    if(ui->radioButton_2->isChecked())
        query = "SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE callType=2";
    if(ui->radioButton_3->isChecked())//Исходное состояние
        query = "SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record";

  //  SqlWrite->
    //Вывод
    SqlWrite->setQuery(query);

    SqlWrite->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    SqlWrite->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя файла"));
    SqlWrite->setHeaderData(2, Qt::Horizontal, QObject::tr("Имя контакта"));
    SqlWrite->setHeaderData(3, Qt::Horizontal, QObject::tr("Номер тел."));
    SqlWrite->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата"));
    SqlWrite->setHeaderData(5, Qt::Horizontal, QObject::tr("Номер модифицированный."));
    SqlWrite->setHeaderData(6, Qt::Horizontal, QObject::tr("Размер байт"));
    SqlWrite->setHeaderData(7, Qt::Horizontal, QObject::tr("Продолжительность с."));
    SqlWrite->setHeaderData(8, Qt::Horizontal, QObject::tr("Тип вызова"));
    SqlWrite->setHeaderData(9, Qt::Horizontal, QObject::tr("Комментарий"));
    SqlWrite->setHeaderData(10, Qt::Horizontal, QObject::tr("Приоритет"));
    SqlWrite->setHeaderData(11, Qt::Horizontal, QObject::tr("Created"));
    SqlWrite->setHeaderData(12, Qt::Horizontal, QObject::tr("Modificated"));

    ui->tableView->setModel(SqlWrite);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк

    //Прокрутка для tableView
    while (SqlWrite->canFetchMore())
        SqlWrite->fetchMore();

    ui->tableView->setColumnHidden(0,true);
    ui->statusBar->showMessage(QString("Кол-во записей :%1")
                               .arg( SqlWrite->rowCount()), 0 );
}



//***************Сортировка*************
// Используем модель     QSqlQueryModel *SqlWrite;
// Сортировка по номеру, по имени
// Сортировка по дате реализована с помощью UNIX времени
void MainWindow::FindRecord(QString textSort)
{
    if(ui->checkBox->isChecked()) // Галочка у фильтер
    {
        QString query;
        QString par1 = "'" + textSort + "%" + "'";
        quint64 parI=0;
        quint64 par2=0;
        if(ui->comboBox->currentIndex()==0)
        {
            ui->comboBox_2->setVisible(false);
            query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE number LIKE %1 ").arg(par1);
        }

        if(ui->comboBox->currentIndex()==1)
        {
            ui->comboBox_2->setVisible(false);
            query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE namePeople LIKE %1 ").arg(par1);
        }

        if(ui->comboBox->currentIndex()==2)
        {
            ui->comboBox_2->setVisible(true);
            QDateTime dt;
            dt = dt.fromString(textSort,"dd.MM.yyyy");// из строчки в формат
            qDebug() << dt.toString();
            parI = dt.toTime_t()+3600;  // Начало дня
            par2 = parI + 3600*24;  // Конец дня
            qDebug() << parI << "между" << par2;

            if(ui->comboBox_2->currentIndex()==0) //для =
            {
                query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE dateTimeRec between %1 and %2 ").arg(parI).arg(par2);
            }
            if(ui->comboBox_2->currentIndex()==1) //для >
            {
                query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE dateTimeRec > %1 ").arg(parI);
            }

            if(ui->comboBox_2->currentIndex()==2) //для <
            {
                query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE dateTimeRec < %1 ").arg(parI);
            }

            if(ui->comboBox_2->currentIndex()==3) //Между
            {
                ui->findPar2->setVisible(true);
                par2 = dt.fromString(ui->findPar2->text(),"dd.MM.yyyy").toTime_t();// из строчки в формат
                query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE dateTimeRec between %1 and %2").arg(parI).arg(par2);
            }
        }
        //Вывод
        SqlWrite->setQuery(query);

        SqlWrite->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        SqlWrite->setHeaderData(1, Qt::Horizontal, QObject::tr("Имя файла"));
        SqlWrite->setHeaderData(2, Qt::Horizontal, QObject::tr("Имя контакта"));
        SqlWrite->setHeaderData(3, Qt::Horizontal, QObject::tr("Номер тел."));
        SqlWrite->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата"));
        SqlWrite->setHeaderData(5, Qt::Horizontal, QObject::tr("Номер модифицированный."));
        SqlWrite->setHeaderData(6, Qt::Horizontal, QObject::tr("Размер байт"));
        SqlWrite->setHeaderData(7, Qt::Horizontal, QObject::tr("Продолжительность с."));
        SqlWrite->setHeaderData(8, Qt::Horizontal, QObject::tr("Тип вызова"));
        SqlWrite->setHeaderData(9, Qt::Horizontal, QObject::tr("Комментарий"));
        SqlWrite->setHeaderData(10, Qt::Horizontal, QObject::tr("Приоритет"));
        SqlWrite->setHeaderData(11, Qt::Horizontal, QObject::tr("Created"));
        SqlWrite->setHeaderData(12, Qt::Horizontal, QObject::tr("Modificated"));

        ui->tableView->setModel(SqlWrite);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк

        //Прокрутка для tableView
        while (SqlWrite->canFetchMore())
            SqlWrite->fetchMore();

        ui->tableView->setColumnHidden(0,true);
        ui->statusBar->showMessage(QString("Кол-во записей :%1")
                                   .arg( SqlWrite->rowCount()), 0 );
    }
}


//**************Рекурсивная функция поиска файлов и запись в переменнную типа LIST*************//
QStringList MainWindow::FileFinder(const QDir& dir)
{
    QStringList filter("*.amr");
    filter.append("*.mp3");
    QStringList filesALL;
    QApplication::processEvents();
    QStringList listFiles =  dir.entryList(QStringList(filter), QDir::Files);// список файлов в каталоге формата *.amr, параметр2= только файлы
    foreach(QString file, listFiles)
    {
        filesALL.append(dir.absoluteFilePath(file));//путь до файла
        //qDebug() <<QDir(file).dirName();// имя файла
    }
    //для поиска в папках
    QStringList listDir = dir.entryList(QDir::Dirs);// всех подпапок
    foreach (QString subdir, listDir)
    {
        if (subdir == "." || subdir == "..")
            continue;
        filesALL.append(FileFinder(QDir(dir.absoluteFilePath(subdir)))); //рекурсивный вызов функции
    }
    return filesALL;
}


//**********Выбор папки***********//
void MainWindow::on_btn_add_clicked()
{
    //добавить чтение файлов из зашифрованого архива
    //QString files1 = QFileDialog::getOpenFileName(this, "Open files", QString(), "Audio Files (*.amr)"); //выбор файлов из папки
    QString files1 = QFileDialog::getExistingDirectory(this, "Open files", QString()); //выбираем папку с файлами
    foreach (QString filePath, FileFinder(QDir(files1)))
    {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));//имя файла
        items.append(new QStandardItem(filePath));//путь к файлу
        Audio->m_playListModel->appendRow(items);
        Audio->m_playlist->addMedia(QUrl(filePath));
    }
    ui->btn_paint->setEnabled(true);
}


//Инициализация линии прогресса записи в БД
void MainWindow::inizilizeProgBar(int range)
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0,range);
}

//*********мини функция для перевод unix времени в формат dd.mm.yyyy HH:MM:SS
//QString MainWindow::dateCreate(QString time1)
//{
//    QDateTime dt;
//    QString result;
//    if(time1.toLongLong()==0)
//        result="0";
//    else
//    {
//        dt.setTime_t(time1.toLongLong()-3600);
//        result=dt.toString("dd.MM.yyyy HH:mm:ss");
//    }
//    return result;
//}

void MainWindow::closeEvent(QCloseEvent *event)
{
    isClosed=true;
    Sleep(1000);
}

//Раскрашиваение элементов
void MainWindow::payPrint() //Все аудиозаписи которые не нашли контакт записать в таблицу
{
    model->clear();
    QSqlRecord record;
    QStandardItem *item;
    if(ui->tableView->model()->rowCount()==SqlTabMod->rowCount())
    {
        for(int i = 0; i < SqlTabMod->rowCount(); i++)
        {
            for(int j = 0; j < 15; j++)
            {
                record = SqlTabMod->record(i);
                item = new QStandardItem(QString(record.value(j).toString()));
                model->setItem(i,j,item);
            }
        }
    }

    if(ui->tableView->model()->rowCount()==SqlWrite->rowCount())
    {
        for(int i = 0; i < SqlWrite->rowCount(); i++)
        {
            for(int j = 0; j < 15; j++)
            {
                record = SqlWrite->record(i);
                item = new QStandardItem(QString(record.value(j).toString()));
                model->setItem(i,j,item);
            }
        }
    }

    model->setHorizontalHeaderLabels(horizontalHeaderMy());

    int k = 0;
    // model->item(0,2)->setBackground(Qt::yellow);
    for(int i = 0; i < model->rowCount(); i++)
    {
        for(int j = 0; j < Audio->m_playListModel->rowCount(); j++)
        {
            if(model->item(i,2)->text()==Audio->m_playListModel->item(j,0)->text())
                k++;
        }
        if(k==0)//Если записи нет, то в таблице выделяем красным
        {
            model->item(i,1)->setBackground(Qt::red);
            model->item(i,2)->setBackground(Qt::red);
            model->item(i,3)->setBackground(Qt::red);
            model->item(i,4)->setBackground(Qt::red);
            model->item(i,5)->setBackground(Qt::red);
            model->item(i,6)->setBackground(Qt::red);
            model->item(i,7)->setBackground(Qt::red);
            model->item(i,8)->setBackground(Qt::red);
        }
        else
            if(k>1)
                QMessageBox::critical(NULL, "Ошибка", "Повторяющиеся значения в базе");
            else if (k==1)//Если запись есть, то в таблице выделяем зеленым
            {
                qDebug() <<  model->item(i,1)->text();
                model->item(i,1)->setBackground(Qt::green);
                model->item(i,2)->setBackground(Qt::green);
                model->item(i,3)->setBackground(Qt::green);
                model->item(i,4)->setBackground(Qt::green);
                model->item(i,5)->setBackground(Qt::green);
                model->item(i,6)->setBackground(Qt::green);
                model->item(i,7)->setBackground(Qt::green);
                model->item(i,8)->setBackground(Qt::green);
            }
        k=0;
    }

    // Доработать запись аудиозаписи в базу данных, которая там не значится
    for(int i = 0; i < Audio->m_playListModel->rowCount(); i++)
    {
        for(int j = 0; j < model->rowCount(); j++)
        {
            if(model->item(j,2)->text()==Audio->m_playListModel->item(i,0)->text())
                k++;
        }
        if(k==0)
        {
            QString callType="1";
            QStringList text = Audio->m_playListModel->item(i,0)->text().split('_',QString::SkipEmptyParts);
            QString nameFile=Audio->m_playListModel->item(i,0)->text();//nameFile записывается в эту переменную из модели
            QString NamePeople = text.at(0).mid(1,text.at(0).count()-2);
            QString number = text.at(1).mid(1,text.at(1).count()-2);
            QString dateTime = text.at(2).mid(1,text.at(2).count()-2) + " " + text.at(3).mid(1,text.at(3).count()-6);
            //Приведение даты к unix формату
            QDateTime dt;
            dt = dt.fromString(dateTime,"dd-MM-yyyy HH-mm-ss");
            uint unix=dt.toTime_t(); //в UNIX формат
            dateTime = QString::number(unix);

            //**********Поиск последнего id элемента базы CallRecProg.db****************//
            db.setDatabaseName(PATH);   //Каталог базы данных в которую будет запись
            db.open();
            QSqlQuery CallRecquery(db);
            int CallRecID=0;
            CallRecquery.exec("SELECT id FROM record");
            while (CallRecquery.next())
                CallRecID=CallRecquery.value(0).toInt();
            //*************************************************************************//
            qDebug()<< "Последний элемент"<< CallRecID;//Вывод последненго элемента


            CallRecquery.prepare("insert into record(id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType)"
                                 "values(:id, :nameFile, :namePeople, :number, :dateTimeRec, :numberModif, :fileSize, :fileLenght, :callType)");
            CallRecquery.bindValue(":id", CallRecID+1);
            CallRecquery.bindValue(":nameFile",nameFile);
            CallRecquery.bindValue(":namePeople", NamePeople);
            CallRecquery.bindValue(":number", number);
            CallRecquery.bindValue(":dateTimeRec", dateTime);
            //проверка записи телефона
            if((number.count()>10))
                if(!((number[0]=='+') && (number[1]=='8')))
                    if((number[0]=='+') && (number[1]=='7'))
                        number.remove(0,2);
                    else
                        if((number[0]=='8') && (number.count()>10))
                            number.remove(0,1);
            CallRecquery.bindValue(":numberModif", number);
            QFile file(Audio->m_playListModel->item(i,1)->text());
            file.open(QIODevice::ReadOnly);
            QString fileSize = QString::number(file.size());
            QStringList text1 = Audio->m_playListModel->item(i,1)->text().split('/',QString::SkipEmptyParts);
            if(text1.at(text1.count()-1)=="Outgoing")
                callType = "2";
            file.close();

            CallRecquery.bindValue(":fileSize", fileSize);
            CallRecquery.bindValue(":fileLenght", "0");
            CallRecquery.bindValue(":callType", callType);

            CallRecquery.exec();
        }
    }
    ui->tableView->setModel(model);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->tableView->setColumnHidden(0,true);
    ui->statusBar->showMessage(QString("%1 Добавить, Кол-во записей :%2")
                               .arg(12 ,0,'f',0)
                               .arg( model->rowCount()), 0 );
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения
}

void MainWindow::smsCreate()
{
    // Создание формы "SMS"
    formsms = new smsDialog(db,PATH,this);
    // Главное окно скрыто
    // QMainWindow::hide();
    formsms->show(); // Запуск созданной формы
    //delete formsms;
}

void MainWindow::bdbdThread()
{
    QtConcurrent::run(this,&MainWindow::bdbd);
}

QStringList MainWindow::horizontalHeaderMy() //Заголовок таблицы
{
    QStringList horizontalHeader;
    horizontalHeader.append("111");
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

void MainWindow::writeSql()
{
    SqlWrite->setQuery(ui->lineEdit_2->text());
    ui->tableView->setModel(SqlWrite);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->tableView->setColumnHidden(0,true);
    ui->statusBar->showMessage(QString("Кол-во записей :%1")
                               .arg( SqlWrite->rowCount()), 0 );
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения
}

//при изменении текста, значение передается в фун-ию сортировки
void MainWindow::click_findOk()
{
    FindRecord(ui->lineEdit->text());
}


//При воспроизведении передвигается ползунок
void MainWindow::setMusicRange(qint64 time)
{
    ui->horizontalSlider_2->setRange(0,Audio->m_player->duration());
    QTime displayTime(0, (time/60000) % 60, (time / 1000) % 60); //Из qint64 в формат QTime
    ui->musicLabStart->setText(displayTime.toString("mm:ss")); // Label начала
    ui->horizontalSlider_2->setValue((int)time); //Перемещение ползунка с воспроизведением трека
    QTime displayTimeEnd(0, (Audio->m_player->duration()/60000) % 60, (Audio->m_player->duration() / 1000) % 60);
    ui->musicLabEnd->setText(displayTimeEnd.toString("mm:ss")); // Запись в Label длинны записи трека
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete formsms;
}

void MainWindow::MainWindow2(QSqlDatabase db1)
{
    db = db1;
}
