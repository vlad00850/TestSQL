#include "smsdialog.h"
#include "ui_smsdialog.h"


//data/data/com.android.providers.telephony/databases/mmssms.db

smsDialog::smsDialog(QSqlDatabase db, QString PATH, QWidget *parent) :
    QDialog(parent),
    _db(db),
    _PATH(PATH),//путь БД c CallRecProg.bd
    ui(new Ui::smsDialog)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->progressBar->setAlignment(Qt::AlignCenter);
    ui->lineEdit->setText("SELECT * FROM sms ORDER BY address ASC;");
    connect(ui->pushButton_4, SIGNAL(clicked()), SLOT(writeSql()));// Выполнение sql запроса

    connect(this, SIGNAL(signInizilizeProgBar(int)), SLOT(inizilizeProgBar(int)));
    connect(this, SIGNAL(signProgresBar(int)), ui->progressBar,SLOT(setValue(int)));

    connect(ui->pushButton, SIGNAL(clicked()), SLOT(SelectDataBase()));//Выбор БД
    connect(ui->pushButton_2, SIGNAL(clicked()), SLOT(bdbd()));
    connect(ui->pushButton_3, SIGNAL(clicked()), SLOT(contacts()));// test

    connect(ui->tableView_2, SIGNAL(doubleClicked(QModelIndex)), SLOT(getId(QModelIndex)));
    ui->listWidget->setVisible(false);
    //Открытие БД
    //_db.setDatabaseName(PATH);
    QMessageBox ms;
    QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
    QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
    ms.setText("Сразу открыть из главной БД (CallRecProg.bd)?");
    ms.exec();
    qDebug() << QDir(_db.databaseName()).dirName();
    if(ms.clickedButton()==yes)
    {
        _db.open();
        Load();
        contacts();
    }
    if(ms.clickedButton()==no)
        SelectDataBase();

    //**************Сортировки, поиски****************************
    connect(ui->lineEdit_3,SIGNAL(textChanged(QString)), SLOT(FindRecord(QString))); //при изменении текста, значение передается в фун-ию сортировки
    connect(ui->findOk,SIGNAL(clicked()), SLOT(click_findOk())); //при изменении текста, значение передается в фун-ию сортировки
    ui->pushButton_2->setEnabled(false);
    //*******comboBox сортировки**************
    QStringList findList;
    findList.append("По номеру телефона");
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

}

//убрать везде date_sent
//передать в качестве параметра конструктора базу данных

smsDialog::~smsDialog()
{
    delete ui;
}

//*********************Входящий или исходящий сортировка******************//
void smsDialog::InOutCall()
{
    Clear();
    model->setHorizontalHeaderLabels(horizontalHeaderMy()); //Заголовок таблицы
    QSqlQuery query;

    if(ui->radioButton->isChecked())
        query.exec("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE type=1");
    if(ui->radioButton_2->isChecked())
        query.exec("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE type=2");
    if(ui->radioButton_3->isChecked())//Исходное состояние
        query.exec("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms");

    Zapis(query,0); // 0 или 1?
}

//***************Сортировка*************//
void smsDialog::FindRecord(QString textSort)
{
    if(ui->checkBox->isChecked())
    {
        Clear();
        model->setHorizontalHeaderLabels(horizontalHeaderMy()); //Заголовок таблицы
        QSqlQuery query;
        QString par1 = textSort + "%";
        quint64 parI;
        quint64 par2;
        if(ui->comboBox->currentIndex()==0)
        {
            ui->comboBox_2->setVisible(false);
            query.prepare("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE address LIKE :par1 ");
            query.bindValue(":par1", par1);
        }

        if(ui->comboBox->currentIndex()==1)
        {
            ui->comboBox_2->setVisible(true);
            QDateTime dt;
            dt = dt.fromString(textSort,"dd.MM.yyyy");// из строчки в формат
            parI=dt.toTime_t();
            if(ui->comboBox_2->currentIndex()==0) //для =
            {
                query.prepare("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE date = :par1 ");
                query.bindValue(":par1", parI);
            }
            if(ui->comboBox_2->currentIndex()==1) //для >
            {
                query.prepare("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE date > :par1 ");
                query.bindValue(":par1", parI);
            }

            if(ui->comboBox_2->currentIndex()==2) //для <
            {
                query.prepare("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE date < :par1 ");
                query.bindValue(":par1", parI);
            }

            if(ui->comboBox_2->currentIndex()==3) //Между
            {
                ui->findPar2->setVisible(true);
                par2= dt.fromString(ui->findPar2->text(),"dd.MM.yyyy").toTime_t();// из строчки в формат
                query.prepare("SELECT id, address, date ,date_sent ,type , body, imsi,thread_id , association_id FROM sms WHERE date between :par1 and :par2");
                query.bindValue(":par1", parI);
                query.bindValue(":par2", par2);
            }
        }
        query.exec();
        Zapis(query,1);
    }
}

//Формирует список смс
//запись в listWidget переписки, из модели model, по выбору тел из модели tableView_2
void smsDialog::getId(QModelIndex row)
{
    QListWidgetItem *itemColCont;
    ui->listWidget->setVisible(true);
    ui->tableView->setVisible(false);
    ui->listWidget->clear();
    QString text = row.sibling(row.row(),0).data(Qt::DisplayRole).toString(); //Текстовая строка выбранной строки (Номер тел)
    QString dateddmmyyyy="";
    ui->listWidget->addItem(text+ "\t \t \t \t \t \t " + "Пользователь");

    for(int i=0; i<model->rowCount(); i++)
        if(text==model->item(i,2)->text())//Поиск
        {

        //Дата сверху сообщений и не повторяется
        if(dateddmmyyyy!=model->item(i,3)->text().mid(0,11))
        {
            dateddmmyyyy=model->item(i,3)->text().mid(0,11);
            ui->listWidget->addItem("\t \t \t " + dateddmmyyyy);
        }

            //Дописать обрезание на две части текста смс которая больше 35 символов ????????????????????????????????????????????????????????????????????????????
            QString result=model->item(i,3)->text().mid(11) + "   " + model->item(i,6)->text();
            if(model->item(i,5)->text()=="Входящий")
            {
                while(result.count()>60){  //Пока строка больше 35 символов режем ее
                    QString cutResult = result.mid(0,60); //Обрезаем первые 35 символов
                    result=result.mid(60);//Обрезаем последние с 35го символа
                    itemColCont= new QListWidgetItem(cutResult);
                    itemColCont->setTextColor(Qt::red);
                    ui->listWidget->addItem(itemColCont);
                }

                itemColCont= new QListWidgetItem(result);
                itemColCont->setTextColor(Qt::red);
                ui->listWidget->addItem(itemColCont);
            }
            else
            {
                while(result.count()>60){  //Пока строка больше 35 символов режем ее
                    QString cutResult = result.mid(0,60); //Обрезаем первые 35 символов
                    result=result.mid(60);//Обрезаем последние с 35го символа
                    itemColCont= new QListWidgetItem("\t \t \t \t" + cutResult);
                    itemColCont->setTextColor(Qt::blue);
                    ui->listWidget->addItem(itemColCont);
                }

                itemColCont= new QListWidgetItem("\t \t \t \t" + result);
                itemColCont->setTextColor(Qt::blue);
                ui->listWidget->addItem(itemColCont);
            }
        }
}


void smsDialog::writeSql() //Ввод запроса
{
    ui->listWidget->setVisible(false);
    ui->tableView->setVisible(true);
    contact2->setQuery(ui->lineEdit->text());
    ui->tableView->setModel(contact2);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->tableView->setColumnHidden(0,true);
    ui->label->setText(QString("Кол-во записей :%1").arg(contact2->rowCount()));
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения
}

//Отображает всех кто писал смс сгруппированных по номеру телефона
void smsDialog::contacts()
{
    contact->setQuery("SELECT address, count(*) FROM sms GROUP BY address ORDER BY address ASC ;");
    ui->tableView_2->setModel(contact);
}

void smsDialog::inizilizeProgBar(int range)
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0,range);
}

void smsDialog::closeEvent(QCloseEvent *event)
{
   isClosed=true;
   Sleep(1000);
}

//Перевод даты из unix в формат dd.MM.yyyy HH:mm:ss
QString smsDialog::dateCreate(QString time1)
{
    QDateTime dt;
    QString result;
    if(time1.toLongLong()==0)
        result="0";
    else
    {
        dt.setTime_t(time1.toLongLong()-3600);
        result=dt.toString("dd.MM.yyyy HH:mm:ss");
    }
    return result;
}

//Перенос из модели в базу данных
void smsDialog::bdbd()
{
    int  m_nStep=0;
    emit signInizilizeProgBar(model->rowCount());//Сигнал о количесве записей в модели

    //**********Поиск последнего id элемента базы CallRecProg.db****************//
    _db.setDatabaseName(_PATH);
    // _db.setDatabaseName("D:\\vlads\\Qt\\TestSQL\\TestSQL\\CallRecProg.db");
    _db.open();
    QSqlQuery CallRecquery(_db);
    int callRecSmsID=0;
    CallRecquery.exec("SELECT id FROM sms");
    while(CallRecquery.next())
        callRecSmsID=CallRecquery.value(0).toInt();
    //*************************************************************************//
    qDebug()<< callRecSmsID;

    for(int i = 0; i<model->rowCount() && !isClosed; i++)
    {
        QString association_id="";
        ++m_nStep;
        if(m_nStep%50==0)
            emit signProgresBar(m_nStep);
        CallRecquery.prepare("SELECT * FROM sms WHERE association_id LIKE :association_id");
        association_id=model->data(model->index(i,9)).toString();//association_id записывается в эту переменную из модели
        //            if(nameFile==NULL)
        //                continue;
        CallRecquery.bindValue(":association_id", association_id);// поиск переменной в базе данных RecordDB
        CallRecquery.exec();
        CallRecquery.first();

        if(CallRecquery.value(0).isNull()) // Если значения нет то записываем в базу
        {
            callRecSmsID++;
            CallRecquery.prepare("insert into sms(id, address, date, date_sent, type, body, imsi, thread_id, association_id)"
                                 "values(:id, :address, :date, :date_sent, :type, :body, :imsi, :thread_id, :association_id)");
            CallRecquery.bindValue(":id", callRecSmsID);
            //*********************Форматирует номер в нормальное значение
            QString adress= model->data(model->index(i,2)).toString();
            QString adressTrue="";
            for(int i=0;i<adress.count();i++)
                if(!((adress.at(i)=='-') || (adress.at(i)==' ')))
                    adressTrue+= adress.at(i);
            //*********************
            CallRecquery.bindValue(":address", adressTrue);
            CallRecquery.bindValue(":date", model->data(model->index(i,3)).toString().remove(10,3)); //удаление последних трех элементов с 10 позиции
            CallRecquery.bindValue(":date_sent", model->data(model->index(i,4)).toString().remove(10,3));//удаление последних трех элементов с 10 позиции
            CallRecquery.bindValue(":type", model->data(model->index(i,5)).toInt());
            CallRecquery.bindValue(":body", model->data(model->index(i,6)).toString());
            CallRecquery.bindValue(":imsi", model->data(model->index(i,7)).toString());
            CallRecquery.bindValue(":thread_id", model->data(model->index(i,8)).toInt());
            CallRecquery.bindValue(":association_id", model->data(model->index(i,9)).toString());
            CallRecquery.exec();
        }
    }
   // QMessageBox::information(NULL, "Сообщение", "Перенос в БД выполнен успешно!");
    emit signProgresBar(0);
//    ui->progressBar->setVisible(false);
}


void smsDialog::SelectDataBase()
{
    //_db = QSqlDatabase::addDatabase("QSQLITE");
    QString files1 = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных", QString(), "Data basef ile (*.db)");
    if(files1=="")
        return;
    _db.setDatabaseName(files1);
    _db.open();
    Load();
}

//Загрузка данных в таблицу
void smsDialog::Load()
{
    Clear();
    QSqlQuery query;

    model->setHorizontalHeaderLabels(horizontalHeaderMy()); //Заголовок таблицы

    if(QDir(_db.databaseName()).dirName() == "CallRecProg.db")  //Добавить открытие базы CallRecProg.db
    {
        query.exec("select id, address, date, date_sent, type, body, imsi, thread_id, association_id from sms");
        Zapis(query,0);
    }
    else
        if(QDir(_db.databaseName()).dirName() == "mmssms.db")
        {
            query.exec("select address, date, date_sent, type, body, imsi, thread_id, association_id from sms");
            Zapis(query,1);
            QMessageBox ms;
            QAbstractButton *yes= ms.addButton("Да", QMessageBox::YesRole);
            QAbstractButton *no= ms.addButton("Нет", QMessageBox::NoRole);
            ms.setText("Cразу записать в БД приложения (CallRecProg.bd)?");
            ms.exec();
            if(ms.clickedButton()==yes)
            {
//                files2 = QFileDialog::getOpenFileName(this, "Выберете файл с базой данных CallRecProg.db", QString(), "Data basef ile (*.db)");
//                if(files2=="")
//                    return;
                ui->pushButton_2->setEnabled(false);
                QtConcurrent::run(this,&smsDialog::bdbd);
            }
            if(ms.clickedButton()==no)
                ui->pushButton_2->setEnabled(true);
        }
        else
        {
            QMessageBox::critical(NULL, "Ошибка", "Название базы не соответствует");
            return;
        }
}

//***************Вывод на экран
void smsDialog::Zapis(QSqlQuery query, int j)
{
    int i=0;
    if(j==0)
    {
        while (query.next())
        {
            QString id = query.value(0).toString();
            item = new QStandardItem(QString(id));
            model->setItem(i,1,item);

            QString address = query.value(1).toString();
            item = new QStandardItem(QString(address));
            model->setItem(i,2,item);

            QString date = query.value(2).toString();
            item = new QStandardItem(dateCreate(date));
            model->setItem(i,3,item);

            QString date_sent = query.value(3).toString();
            item = new QStandardItem(dateCreate(date_sent));
            model->setItem(i,4,item);

            QString type ="";
            //QString type = query.value(4).toString();
            if(query.value(4).toInt()==1)
                type="Входящий";
            if(query.value(4).toInt()==2)
                type="Исходящий";
            item = new QStandardItem(QString(type));
            model->setItem(i,5,item);

            QString body = query.value(5).toString();
            item = new QStandardItem(QString(body));
            model->setItem(i,6,item);

            QString imsi = query.value(6).toString();
            item = new QStandardItem(QString(imsi));
            model->setItem(i,7,item);

            QString thread_id = query.value(7).toString();
            item = new QStandardItem(QString(thread_id));
            model->setItem(i,8,item);

            QString association_id = query.value(8).toString();
            item = new QStandardItem(QString(association_id));
            model->setItem(i,9,item);

            i++;
        }
    }
    else
        if(j==1)
        {
            while (query.next())
            {
                QString address = query.value(0).toString();
                item = new QStandardItem(QString(address));
                model->setItem(i,2,item);

                QString date = query.value(1).toString();
                item = new QStandardItem(QString(date));
                model->setItem(i,3,item);

                QString date_sent = query.value(2).toString();
                item = new QStandardItem(QString(date_sent));
                model->setItem(i,4,item);

                QString type = query.value(3).toString();
                item = new QStandardItem(QString(type));
                model->setItem(i,5,item);

                QString body = query.value(4).toString();
                item = new QStandardItem(QString(body));
                model->setItem(i,6,item);

                QString imsi = query.value(5).toString();
                item = new QStandardItem(QString(imsi));
                model->setItem(i,7,item);

                QString thread_id = query.value(6).toString();
                item = new QStandardItem(QString(thread_id));
                model->setItem(i,8,item);

                QString association_id = query.value(7).toString();
                item = new QStandardItem(QString(association_id));
                model->setItem(i,9,item);

                i++;
            }
        }
        else
        {
            QMessageBox::critical(NULL, "Ошибка", "Название базы не соответствует");
            return;
        }

    ui->tableView->setModel(model);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // режим выделения строк
    ui->tableView->setColumnHidden(0,true);
    ui->label->setText(QString("Кол-во записей :%1").arg( model->rowCount()));
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // для невозможности изменения
}

QStringList smsDialog::horizontalHeaderMy() //Заголовок таблицы
{
    QStringList horizontalHeader;
    horizontalHeader.append("111");
    horizontalHeader.append("id");
    horizontalHeader.append("address");
    horizontalHeader.append("date");
    horizontalHeader.append("date_sent");
    horizontalHeader.append("type");
    horizontalHeader.append("body");
    horizontalHeader.append("imsi");
    horizontalHeader.append("thread_id");
    horizontalHeader.append("association_id");
    return horizontalHeader;
}

void smsDialog::Clear()
{
     model->clear();
}

void smsDialog::on_findOk_clicked()
{

}
