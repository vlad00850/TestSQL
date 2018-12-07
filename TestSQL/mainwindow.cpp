
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QWhatsThis"  //Для всплывающих подсказок
#include "QRegExp"
//#include "QMediaMetaData"

#include "taglib/fileref.h"
#include "taglib/taglib.h"
#include "taglib/tag.h"
#include <QFile>


QString PATH = QDir::current().absolutePath() + "/CallRecProg.db"; //путь к главной БД

//Добавить множественное добавление базы contacts и record

//data/data/com.android.soundrecorder/databases/recorded.db
//data/data/com.android.providers.contacts/databases/contacts2.db
//Все аудиозаписи которые не нашли контакт записать в таблицу
//taglib.dll положить в папку с exe!

//Раскрашивание из query модели добавить
//Группировка по контактам и людям

//SqlTabMod->setSort(3,Qt::DescendingOrder);
//SqlTabMod->sort(3,Qt::DescendingOrder);

//Правой кнопкой на значение и открывается меню как в windows

//Представление времени в формате mm:ss
//            QString FileLenght = query.value(7).toString();
//            QTime displayTime(0, (FileLenght.toInt()/60) % 60, (FileLenght.toInt()) % 60);
//            item = new QStandardItem(QString(displayTime.toString("mm:ss")));
//            model->setItem(i,8,item);

//QString FileDir="";//папка с выбранными файлами


//QStandardItemModel *model = new QStandardItemModel;

//QStandardItem *item;
bool isClosed=false; // закрытие приложения

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SqlWrite = new QSqlQueryModel;
    SqlTabMod= new QSqlTableModel;

    audioPlay = new audioplayeer();
    ui->playlistView->setModel(audioPlay->m_playListModel);
    myDataBase = new databaseMy();

    connect(ui->btn_previous, SIGNAL(clicked()),audioPlay->m_playlist,SLOT(previous()));
    connect(ui->btn_next, SIGNAL(clicked()),audioPlay->m_playlist,SLOT(next()));
    connect(ui->btn_play, SIGNAL(clicked()),audioPlay->m_player,SLOT(play()));
    connect(ui->btn_pause, SIGNAL(clicked()),audioPlay->m_player,SLOT(pause()));
    connect(ui->btn_stop, SIGNAL(clicked()),audioPlay->m_player,SLOT(stop()));

    ui->currentTrack->setText(" ");//Трек который воспроизводится
    //**********************Звук****************************
    ui->horizontalSlider->setRange(0,100);//Задаем, что с 0 до 100 будет передвигаться ползунок
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),audioPlay->m_player,SLOT(setVolume(int)));//Громкость воспроизведения и горизонтальный ползунок
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),ui->volumLabel,SLOT(setNum(int)));//Громкость воспроизведения и запись в label
    ui->horizontalSlider->setValue(50);//Начальное значение ползунка
    //********************Ползунок воспроизмеделния трека***********************************
    connect(audioPlay->m_player, SIGNAL(positionChanged(qint64)), this, SLOT(setMusicRange(qint64)));//Передача параметра позиции записи по времени
    connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)),audioPlay,SLOT(setPosition(int)));//Прокрутка времени
    //**************************************************************************************

    //Для записи в главную таблицу
    connect(ui->pushButton_3, SIGNAL(clicked()), SLOT(migrationMeizu()));
    connect(this, SIGNAL(signInizilizeProgBar(int)), SLOT(inizilizeProgBar(int)));
    connect(ui->btn_paint, SIGNAL(clicked()), SLOT(payPrintThread()));
    connect(this,SIGNAL(signpayPrintEnd(QStandardItemModel*,int,int)),SLOT(payPrintEnd(QStandardItemModel*,int,int)));
    connect(ui->actionSMs,SIGNAL(triggered()), SLOT(smsCreate()));
    connect(ui->action_startNetwork,SIGNAL(triggered()), SLOT(startNetwork()));
    connect(ui->action_migrationSomes, SIGNAL(triggered()), SLOT(migrationMeizuSomes()));

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

    connect(ui->playlistView, SIGNAL(doubleClicked(QModelIndex)),audioPlay,SLOT(setCurrentIndex(QModelIndex))); // Двойной клик на представление playlistView и запуск трека
    connect(audioPlay->m_playlist, SIGNAL(currentIndexChanged(int)),this, SLOT(currentTrackLabel(int)));//Для вывода в Label - currentTrack трека который воспроизводится

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(getId(QModelIndex)));//Воспроизведение трека по двойному щелчку TableView
    connect(ui->pushButton_2, SIGNAL(clicked()), SLOT(bdbd()));//Перенос из model в БД

    connect(this,SIGNAL(signProgresBarHidden(bool)),ui->progressBar, SLOT(setVisible(bool)));//Скрывает progressBar после переноса в БД

    ui->lineEdit_2->setText("SELECT * FROM record ORDER BY namePeople ASC;"); //lineEdit для запросов
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(writeSql()));// Для запросов к БД

    ui->statusBar->addPermanentWidget(ui->progressBar,1); // progressBar в нижнюю строку(  statusBar)
    connect(this, SIGNAL(signError(QString)), SLOT(slotError(QString))); //Для вывода ошибки
    connect(myDataBase, SIGNAL(sendMessage(QString)), SLOT(slotMessage(QString))); //Для вывода ошибки




}

//Для вывода в Label - currentTrack трека который воспроизводится
void MainWindow::currentTrackLabel(int index)
{
    ui->currentTrack->setText(audioPlay->m_playListModel->data(audioPlay->m_playListModel->index(index,0)).toString());
}

//***********При двойном клике запускается файл аудиозаписи*******************
void MainWindow::getId(QModelIndex row)
{
    QModelIndex result;
    QString text = row.sibling(row.row(),1).data(Qt::DisplayRole).toString(); //Текстовая строка выбранной строки (имя файла)
    qDebug() << text;
    //Поиск по модели m_playListModel, где храняться файлы
    for(int i = 0; i< audioPlay->m_playListModel->rowCount(); i++)
    {
        if(text==audioPlay->m_playListModel->item(i,0)->text())
        {
            result=audioPlay->m_playListModel->indexFromItem(audioPlay->m_playListModel->item(i,0));
            audioPlay->setCurrentIndex(result);//Передача в функцию воспроизведения трека
            ui->label_3->setText(audioPlay->m_playListModel->item(i,1)->text());
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
    QString pathBd = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных", QString(), "Data basef ile (*.db)");
    if(pathBd==""){ //Если ничего не выбрали открываем по умолчанию
        pathBd = PATH;
        if(!myDataBase->open(PATH))
            return;
    }
    else
        if(!myDataBase->open(pathBd))
            return;

    ui->tableView->setModel(myDataBase->openTable(pathBd));
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->tableView ->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения
    ui->statusBar->showMessage(QString("Кол-во записей :%1     База данных: %2").arg(myDataBase->getAbstractModel()->rowCount()).arg(QString(myDataBase->getDb().databaseName())));


    if(QDir(pathBd).dirName() == "CallVoiceRecorder.db")
    {
        QMessageBox ms;
        QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
        QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
        ms.setText("Cразу записать в БД приложения (CallRecProg.db)?");
        ms.exec();
        if(ms.clickedButton()==yes)
        {
            ui->pushButton_2->setEnabled(false);
            this->bdbd();
        }
        if(ms.clickedButton()==no)
            ui->pushButton_2->setEnabled(true);
    }

}

//***************миграция базы данных contacts2 и RecorderDb в одну модель************//
void MainWindow::migrationMeizu()
{
    //Открываем contacts2.db и записываем в модель
    QAbstractItemModel *resultModel;
    QString contacts2 = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных contacts2.db", QString(), "Data basef ile (*.db)");
    if(contacts2=="")
        return;
    myDataBase->open(contacts2);
    QAbstractItemModel *contactsModel = myDataBase->openTable(contacts2); //Записываем таблицу в модель

    //Открываем RecorderDb.db
    QString RecorderDb = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных RecorderDb.db", QString(), "Data basef ile (*.db)");
    if(RecorderDb=="")
        return;
    myDataBase->open(RecorderDb);
    QAbstractItemModel *RecorderDbModel = myDataBase->openTable(RecorderDb); //Записываем таблицу в модель

    resultModel = myDataBase->migrationMeizuBD(contactsModel,RecorderDbModel); // Создаем модель общую из contacts2 и RecorderDb

    ui->tableView->setModel(resultModel);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    //ui->tableView->setColumnWidth(2,400); // одну колонку увеличить в размерах 2-колонка
   // ui->tableView->setColumnHidden(0,true);
    ui->statusBar->showMessage(QString("Кол-во записей :%1").arg(resultModel->rowCount()), 0);

    QMessageBox ms;
    QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
    QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
    ms.setText("Cразу записать в БД приложения (CallRecProg.db)?");
    ms.exec();
    if(ms.clickedButton()==yes)
    {
        ui->pushButton_2->setEnabled(false);
        this->bdbd();
    }
    if(ms.clickedButton()==no)
        ui->pushButton_2->setEnabled(true);

    ui->progressBar->reset();
}

//***************миграция базы данных contacts2 и RecorderDb в одну модель с множественным выбором баз************//
//Добавить в поток ресурсозатратные действия
void MainWindow::migrationMeizuSomes()
{
    QAbstractItemModel *resultModel;
    QStringList vecPath;
    QString files1 = QFileDialog::getExistingDirectory(this, "Open files", QString()); //выбираем папку с файлами
    vecPath = FileFinderBd(QDir(files1));
    qDebug() << vecPath;
    QStandardItemModel *contacts = new QStandardItemModel();
    QStandardItemModel *record = new QStandardItemModel();

    myDataBase->openSomeTables(contacts,record, vecPath); //в поток отправить
    resultModel = myDataBase->migrationMeizuBD(contacts,record); // Создаем модель общую из contacts2 и RecorderDb

    ui->tableView->setModel(resultModel);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    //ui->tableView->setColumnWidth(2,400); // одну колонку увеличить в размерах 2-колонка
    ui->statusBar->showMessage(QString("Кол-во записей :%1").arg(resultModel->rowCount()), 0);

    QMessageBox ms;
    QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
    QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
    ms.setText("Cразу записать в БД приложения (CallRecProg.db)?");
    ms.exec();
    if(ms.clickedButton()==yes)
    {
        ui->pushButton_2->setEnabled(false);
        this->bdbd();
    }
    if(ms.clickedButton()==no)
        ui->pushButton_2->setEnabled(true);
}

//Перенос из модели в базу данных
void MainWindow::bdbd()
{
    connect(myDataBase, SIGNAL(signProgresBar(int)),this->ui->progressBar, SLOT(setValue(int)));
    QAbstractItemModel *tableModel = ui->tableView->model();

    emit signInizilizeProgBar(tableModel->rowCount());//Сигнал о количестве записей в модели

    //qDebug() << QDir(myDataBase->getDb().databaseName()).dirName();
    QtConcurrent::run(myDataBase,&databaseMy::bdbd, tableModel, QDir(myDataBase->getDb().databaseName()).dirName());

    emit signProgresBar(0);
     //signProgresBarHidden(false);//скрывает progressBar
    // ui->progressBar->setVisible(false);
}

//*********************Входящий или исходящий - сортировка******************//
void MainWindow::InOutCall()
{
    QString query;

    if(ui->radioButton->isChecked())
       query = "SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE callType=1 or callType=3";
    if(ui->radioButton_2->isChecked())
        query = "SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE callType=2 or callType=3";
    if(ui->radioButton_3->isChecked())//Исходное состояние
        query = "SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record";

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
            qDebug() << query;
        }

        if(ui->comboBox->currentIndex()==1)
        {
            ui->comboBox_2->setVisible(false);
            query = QString("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE namePeople LIKE %1 ").arg(par1);
        }

        if(ui->comboBox->currentIndex()==2)
        {
            if(ui->comboBox_2->isVisible()==false) //Сообщение выводится один раз, когда выберем вкладку combobox = "Дата"
                emit signError("Дата должна быть в формате dd.mm.yyyy");

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
QStringList MainWindow::FileFinderAmrMp3(const QDir& dir)
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
        filesALL.append(FileFinderAmrMp3(QDir(dir.absoluteFilePath(subdir)))); //рекурсивный вызов функции
    }
    return filesALL;
}

QStringList MainWindow::FileFinderBd(const QDir&dir)
{
    QStringList filter("contacts2.db");
    filter.append("RecorderDb.db");
    QStringList filesALL;
    QApplication::processEvents();
    QStringList listFiles =  dir.entryList(QStringList(filter), QDir::Files);// список файлов в каталоге формата filter, параметр2= только файлы
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
        filesALL.append(FileFinderBd(QDir(dir.absoluteFilePath(subdir)))); //рекурсивный вызов функции
    }
    return filesALL;
}


//**********Выбор папки***********//
void MainWindow::on_btn_add_clicked()
{
    QRegExp reg("C[0-9]{5}\-[0-9]{6}\.mp3"); // Для meizu
    QRegExp reg2("\\[\\+?\\S+");//для CallRecorder

    //добавить чтение файлов из зашифрованого архива
    //QString files1 = QFileDialog::getOpenFileName(this, "Open files", QString(), "Audio Files (*.amr)"); //выбор файлов из папки
    QString files1 = QFileDialog::getExistingDirectory(this, "Open files", QString()); //выбираем папку с файлами
    foreach (QString filePath, FileFinderAmrMp3(QDir(files1)))
    {
        QList<QStandardItem *> items;
        if((QString(QDir(filePath).dirName()).indexOf(reg)==-1)&&
                (QString(QDir(filePath).dirName()).indexOf(reg2)==-1))//Проверка на соответствие шаблону Пример: C80117-215142.mp3 или [+78412684779]_[+78412684779]_[28-11-2016]_[15-38-25].amr
            continue;

        items.append(new QStandardItem(QDir(filePath).dirName()));//имя файла
        items.append(new QStandardItem(filePath));//путь к файлу

        if((QString(QDir(filePath).dirName()).indexOf(reg)!=-1))
        {
            TagLib::FileRef f(QFile::encodeName(filePath).constData());
            if (!f.isNull() && f.tag() != NULL)
            {
                QFile s(filePath);
                QStringList List = QString("%1").arg(f.tag()->title().toCString()).split('_',QString::SkipEmptyParts);
                //******************Приведение даты к unix формату
                QDateTime dt;
                dt = dt.fromString(QString("201"+QString(QDir(filePath).dirName()).mid(1,5)+ QString(QDir(filePath).dirName()).mid(7,6)),"yyyyMMddHHmmss");
                uint unix=dt.toTime_t(); //в UNIX формат
                items.append(new QStandardItem(QString::number(unix)));//Дата
                //******************

                items.append(new QStandardItem(List[0]));//Номер телефон
                items.append(new QStandardItem(List[0]));//Имя контакта
                items.append(new QStandardItem(QString("%1").arg(f.audioProperties()->lengthInSeconds())));//Продолжительность
                items.append(new QStandardItem(QString::number(s.size()/1024)));//"Размер в КБ
            }
            audioPlay->m_playListModel->appendRow(items);
            audioPlay->m_playlist->addMedia(QUrl(filePath));
        }
       if((QString(QDir(filePath).dirName()).indexOf(reg2)!=-1))
       {
                audioPlay->m_playlist->addMedia(QUrl(filePath));
                QFile g2(filePath);
                QStringList List = QString(QDir(filePath).dirName()).split('_',QString::SkipEmptyParts);
                //******************Приведение даты к unix формату
                QDateTime dt;
                dt = dt.fromString(QString(List[2] + List[3].remove(10,4)),"[dd-MM-yyyy][HH-mm-ss]");
                uint unix=dt.toTime_t(); //в UNIX формат
                items.append(new QStandardItem(QString::number(unix)));//Дата
                //******************
                items.append(new QStandardItem(List[1].remove(List[1].count()-1,1).remove(0,1)));//Номер телефона
                items.append(new QStandardItem(List[0].remove(List[0].count()-1,1).remove(0,1)));//Имя контакта

                items.append(new QStandardItem("777"));//Продолжительность //Невозможно получить кол-во секунд(TagLib
                                                                           //смотрит на Сводку,а QMediaMetaData не отображает) Вариант через QMediaPlayer
                items.append(new QStandardItem(QString::number(g2.size()/1024)));//"Размер в КБ
                audioPlay->m_playListModel->appendRow(items);
        }
    }
    ui->btn_paint->setEnabled(true);

    audioPlay->m_playListModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя файла"));
    audioPlay->m_playListModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Путь"));
    audioPlay->m_playListModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Дата"));
    audioPlay->m_playListModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Номер телефона"));
    audioPlay->m_playListModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Имя контакта"));
    audioPlay->m_playListModel->setHeaderData(5, Qt::Horizontal, QObject::tr("Продолжительность"));
    audioPlay->m_playListModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Размер"));
    ui->playlistView->resizeRowsToContents();
    ui->playlistView->resizeColumnsToContents();
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

//Раскрашиваение элементов и добавление аудиозаписей которых нет в БД
void MainWindow::payPrint(QAbstractItemModel *modelTable) //Все аудиозаписи которые не нашли контакт записать в таблицу
{
    QStandardItemModel *model2 = new QStandardItemModel();
    model2->clear();
    QStandardItem *item;

    if(audioPlay->m_playListModel->rowCount()==0){
        emit signError("Список аудиозаписей пуст");
        return;
    }

    //****************Запись из таблицы tableView в модель model
    for(int i = 0; i < modelTable->rowCount(); i++)
    {
        for(int j = 0; j < 15; j++)
        {
            item = new QStandardItem(QString(modelTable->data(modelTable->index(i,j)).toString()));
            model2->setItem(i,j,item);
        }
    }
    //************************************

    model2->setHorizontalHeaderLabels(horizontalHeaderMy()); // Записываем заголовки

    int colZapGreen=0;
    QString callType;
    QString nameFile;
    QString dateTime;
    QString number;
    QString NamePeople;
    QString duration;
    QString filesize;

    //*******************Само разукрашивание
    int k = 0;
    int j = 0;
    int _zapInBD=0; //Кол-во записей добавленных в БД по результату проверки на наличие в БД
    for(int i = 0; i < audioPlay->m_playListModel->rowCount() ; i++)
    {
        for(j = 0; j < model2->rowCount(); j++)
        {
            if(model2->item(j,1)->text()==audioPlay->m_playListModel->item(i,0)->text()){
                k++;
                break;
            }
        }
        switch (k) {
        case 0://Если записи нет в БД, то ее добавляем
        {
             callType="3";
             nameFile=audioPlay->m_playListModel->item(i,0)->text();//nameFile записывается в эту переменную из модели
             dateTime =  audioPlay->m_playListModel->item(i,2)->text();
             number = audioPlay->m_playListModel->item(i,3)->text();
             NamePeople = audioPlay->m_playListModel->item(i,4)->text();
             duration = audioPlay->m_playListModel->item(i,5)->text();
             filesize = audioPlay->m_playListModel->item(i,6)->text();

             //**********Поиск последнего id элемента базы CallRecProg.db****************//
             db.setDatabaseName(PATH);   //Каталог базы данных в которую будет запись
             db.open();
             QSqlQuery CallRecquery(db);
             int CallRecID=0;
             CallRecquery.exec("SELECT id FROM record");
             while (CallRecquery.next())
                 CallRecID=CallRecquery.value(0).toInt();
             // qDebug()<< "Последний элемент"<< CallRecID;//Вывод последненго элемента
             //*************************************************************************//

             //Проверяет на повторяющиеся значения по nameFile и dateTimeRec, если не повторяется то записывает в базу данных
             CallRecquery.prepare("SELECT id, nameFile, namePeople, number, dateTimeRec, numberModif, fileSize, fileLenght, callType, comment, priority, modified, created, sim, record_uuid FROM record WHERE nameFile LIKE :nameFile");
             CallRecquery.bindValue(":nameFile", nameFile);// поиск переменной в базе данных RecordDB
             CallRecquery.exec();
             CallRecquery.first();

             if(CallRecquery.value(0).isNull()) // Если значения нет то записываем в базу
             {
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
                 QStringList text1 = audioPlay->m_playListModel->item(i,1)->text().split('/',QString::SkipEmptyParts);
                 if(text1.at(text1.count()-1)=="Outgoing")
                     callType = "2";
                 CallRecquery.bindValue(":fileSize", filesize);
                 CallRecquery.bindValue(":fileLenght", duration);
                 CallRecquery.bindValue(":callType", callType);

                 CallRecquery.exec();
                 _zapInBD++;
                 //После добавления в таблице выделяем желтым
                 model2->item(CallRecID+1,1)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,2)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,3)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,4)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,5)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,6)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,7)->setBackground(Qt::yellow);
                 model2->item(CallRecID+1,8)->setBackground(Qt::yellow);
             }
             break;
        }
        case 1://Если запись есть, то в таблице выделяем зеленым
            model2->item(j,1)->setBackground(Qt::green);
            model2->item(j,2)->setBackground(Qt::green);
            model2->item(j,3)->setBackground(Qt::green);
            model2->item(j,4)->setBackground(Qt::green);
            model2->item(j,5)->setBackground(Qt::green);
            model2->item(j,6)->setBackground(Qt::green);
            model2->item(j,7)->setBackground(Qt::green);
            model2->item(j,8)->setBackground(Qt::green);
            colZapGreen++;
            break;
        default:
            qDebug() << "Ошибка повторяющихся значений";
          //  QMessageBox::critical(NULL, "Ошибка", "Повторяющиеся аудиозаписи");
            break;
        }
        k=0;
    }
    //**************************************************
    emit signpayPrintEnd(model2,colZapGreen,_zapInBD);
}

void MainWindow::payPrintThread()
{
    QAbstractItemModel *modelTable; // Переменная для записи модели из таблицы

    modelTable=ui->tableView->model();
    QtConcurrent::run(this,&MainWindow::payPrint,modelTable);
}

void MainWindow::payPrintEnd(QStandardItemModel*model2, int colZapGreen, int _zapInBD)
{
    ui->tableView->setModel(model2);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->tableView->setColumnHidden(0,true);
    ui->statusBar->showMessage(QString("Кол-во записей :%1, Найденных аудиозаписей в БД %2, Записанных аудио записей в БД %3")
                               .arg(model2->rowCount()).arg(colZapGreen).arg( _zapInBD ));
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

QStringList MainWindow::horizontalHeaderMy() //Заголовок таблицы
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

void MainWindow::writeSql(QString sql)
{
    if(sql=="teleg")
        sql=ui->lineEdit_2->text();
    SqlWrite->setQuery(sql);

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

//при изменении текста, значение передается в фун-ию сортировки
void MainWindow::click_findOk()
{
    FindRecord(ui->lineEdit->text());
}


//При воспроизведении передвигается ползунок
void MainWindow::setMusicRange(qint64 time)
{
    ui->horizontalSlider_2->setRange(0,audioPlay->m_player->duration());
    QTime displayTime(0, (time/60000) % 60, (time / 1000) % 60); //Из qint64 в формат QTime
    ui->musicLabStart->setText(displayTime.toString("mm:ss")); // Label начала
    ui->horizontalSlider_2->setValue((int)time); //Перемещение ползунка с воспроизведением трека
    QTime displayTimeEnd(0, (audioPlay->m_player->duration()/60000) % 60, (audioPlay->m_player->duration() / 1000) % 60);
    ui->musicLabEnd->setText(displayTimeEnd.toString("mm:ss")); // Запись в Label длинны записи трека
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete model;
  //  delete formsms;
}

void MainWindow::MainWindow2(QSqlDatabase db1)
{
    db = db1;
}

void MainWindow::slotError(QString text)
{
    QPoint p = QCursor::pos(); //Текущее положение мышки (х,у)
    QWhatsThis::showText(QPoint(p.x(),p.y()),text);
}

void MainWindow::slotMessage(QString message)
{
    QMessageBox::information(NULL, "Сообщение", message);
}

void MainWindow::startNetwork()
{
    teleg = new mynetworktelegram(this);//передать бд?(bd)
    connect(teleg, SIGNAL(writeSql(QString)),this, SLOT(writeSql(QString)));
    connect(this, SIGNAL(sendMessageTeleg(QString)),teleg, SLOT(sendMessage(QString)));
    connect(teleg, SIGNAL(readbd()),this, SLOT(readAndSendTeleg()));

}

void MainWindow::readAndSendTeleg()
{
//    QAbstractItemModel *modelTable; // Переменная для записи модели из таблицы
//    modelTable=ui->tableView->model();
    QString message;
    QDateTime dt;
    QString callType;
    emit sendMessageTeleg(QString("Количество записей:" + QString::number(SqlWrite->rowCount())));
    for(int i = 0; i < SqlWrite->rowCount(); i++)
    {
        if(SqlWrite->record(i).value(8).toInt()==1)
            callType = "Входящий";
        if(SqlWrite->record(i).value(8).toInt()==2)
            callType = "Исходящий";
        else callType = "3";
        dt.setTime_t(SqlWrite->record(i).value(4).toLongLong()-3600);
        message = QString("id " + SqlWrite->record(i).value(0).toString() + " Имя контакта: " + SqlWrite->record(i).value(2).toString() +
                          "Дата :" + dt.toString("dd.MM.yyyy HH:mm:ss")  + " Продолжительность с." + SqlWrite->record(i).value(7).toString() + "Тип вызова  "+ callType);
        emit sendMessageTeleg(message);
    }
}
