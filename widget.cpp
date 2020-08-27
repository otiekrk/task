#include "widget.h"

loadProgress::loadProgress(){
    connect(this,&loadProgress::continueThread,this,&loadProgress::process);
    breaked = false;
    qDebug() << "Created";
}

loadProgress::~loadProgress(){
    qDebug() << "Destructed";
}

void loadProgress::breakMe(){
    breaked = true;
    ref = false;
}
void loadProgress::breakMe2(){
    breaked = true;
    ref = true;
}

void loadProgress::process(){

    qDebug() << "proces" << value;
    if (value <= 1000){
        emit newValue((value));
        value++;
        this->thread()->msleep(100);
        if (!breaked){
            emit continueThread();
        }else{
            emit finished();
        }
    }else {
        emit finished();
    }
    if (ref){
        emit breakDone();
    }
}


void progress::setProgressValue(int v){
    progressBar->setValue(v);
}

progress::progress(QWidget *parent) : QWidget(parent){
    QVBoxLayout *vbox = new QVBoxLayout(this);

    vbox->addWidget(progressBar);
    progressBar->setRange(0,1000);
    progressBar->setValue(0);
    vbox->addWidget(stopButton);

    setLayout(vbox);

    mainThread = new QThread(this);
    loadProgress * lp = new loadProgress();
    lp->moveToThread(mainThread);

    connect(mainThread, SIGNAL (started()), lp, SLOT (process()));
    connect(lp, SIGNAL (finished()), mainThread, SLOT (quit()));
    connect(lp, SIGNAL (finished()), lp, SLOT (deleteLater()));
    connect(mainThread, &QThread::finished, this,[this]{
        mainThread->deleteLater();
    });
    connect(stopButton,&QPushButton::clicked,this,[lp]{
        lp->breakMe();
    });
    connect(lp,&loadProgress::newValue,this,&progress::setProgressValue);
    connect(this,&progress::breakThread,this,[lp]{
        lp->breakMe2();
    });
    connect(lp,&loadProgress::breakDone,this,&progress::destructMe);
    mainThread->start();
}

void progress::destructMe(){
   deleteLater();
}

void progress::closeEvent(QCloseEvent *event){
    if (mainThread->isRunning()){
        emit breakThread();
    }else{
        deleteLater();
    }
    QWidget::closeEvent(event);
}

AddNewWindow::AddNewWindow(Widget *parent) : QWidget(parent){
    QGridLayout * grid = new QGridLayout(this);
    grid->addWidget(rejLabel,0,0);
    grid->addWidget(rejLine,0,1,1,3);
    grid->addWidget(vinLabel,1,0);
    grid->addWidget(vinLine,1,1,1,3);
    grid->addWidget(modelLabel,2,0);
    grid->addWidget(modelLine,2,1,1,3);
    grid->addWidget(markaLabel,3,0);
    grid->addWidget(markaLine,3,1,1,3);
    grid->addWidget(saveButton,4,0,1,2);
    grid->addWidget(clearButton,4,2,1,2);

    connect(saveButton,&QPushButton::clicked,this,[=]{
        if (rejLine->text() != ""){
            emit addNew(rejLine->text(),vinLine->text(),modelLine->text(),markaLine->text());
        }
        close();
    });
    connect(clearButton,&QPushButton::clicked,this,[=]{
        rejLine->clear();
        vinLine->clear();
        modelLine->clear();
        markaLine->clear();
    });

    setLayout(grid);
    setWindowTitle(tr("Dodaj samochód"));
}

AddNewWindow::~AddNewWindow(){

}

void AddNewWindow::closeEvent(QCloseEvent *event){
    this->deleteLater();
    QWidget::closeEvent(event);
}

void Widget::newCar(const QString &nr_rej, const QString &vin, const QString &str_model, const QString &marka){
    QSqlQuery query;
    QString q = QString("insert into samochody values('%1', '%2', '%3', '%4')").arg(nr_rej).arg(vin).arg(str_model).arg(marka);
    query.exec(q);
    model->select();
    model->revertAll();
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QMessageBox box(this);
    box.setWindowTitle("Baza danych");
    box.setText("Chcesz utworzyć nową baze danych, \nczy wybrać istniejącą ?");
    box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    auto buttonY = box.button(QMessageBox::Yes);
    buttonY->setText("Utwórz nową");
    auto buttonN = box.button(QMessageBox::No);
    buttonN->setText("Wybierz istniejącą");
    auto choice = box.exec();
    if (choice == QMessageBox::Yes){
        QString fn = QFileDialog::getSaveFileName(this,"Tworzenie bazy..","");
        db.setDatabaseName(fn);
        if (!db.open()) {
            QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
                QObject::tr("Unable to establish a database connection.\n"
                            "This example needs SQLite support. Please read "
                            "the Qt SQL driver documentation for information how "
                            "to build it.\n\n"
                            "Click Cancel to exit."), QMessageBox::Cancel);
        }
        QSqlQuery query;
        query.exec("create table samochody (rejestracja varchar(10) primary key, "
                   "vin varchar(20), model varchar(20), marka varchar(20))");
    }else{
        QString fn = QFileDialog::getOpenFileName(this,"Otwórz bazę");
        db.setDatabaseName(fn);
        if (!db.open()) {
            QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
                QObject::tr("Unable to establish a database connection.\n"
                            "This example needs SQLite support. Please read "
                            "the Qt SQL driver documentation for information how "
                            "to build it.\n\n"
                            "Click Cancel to exit."), QMessageBox::Cancel);
        }
    }




    model = new QSqlTableModel(this);
    model->setTable("samochody");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("Numer Rejestracyjny"));
    model->setHeaderData(1, Qt::Horizontal, tr("VIN"));
    model->setHeaderData(2, Qt::Horizontal, tr("Model"));
    model->setHeaderData(3, Qt::Horizontal, tr("Marka"));
    QTableView *view = new QTableView(this);
    view->setModel(model);
    view->resizeColumnsToContents();
    addButton = new QPushButton(tr("Dodaj"),this);
    submitButton = new QPushButton(tr("Zapisz"),this);
    deleteButton = new QPushButton(tr("Usuń"),this);
    revertButton = new QPushButton(tr("&Odśwież"),this);
    quitButton = new QPushButton(tr("Wyjście"),this);
    buttonBox = new QDialogButtonBox(Qt::Vertical,this);
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(submitButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(revertButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    buttonBox->addButton(extraButton,QDialogButtonBox::ActionRole);
    connect(addButton,&QPushButton::clicked,this,[this]{
        if (anw_pointer == nullptr){
            anw_pointer = new AddNewWindow();
            connect(anw_pointer,&AddNewWindow::addNew,this,&Widget::newCar);
            anw_pointer->show();
        }
    });
    connect(extraButton,&QPushButton::clicked,this,[=]{
        if (progress_pointer == nullptr){
            progress_pointer = new progress();
            progress_pointer->show();
        }
    });
    connect(deleteButton,&QPushButton::clicked,this,[this,view]{
        QString nr = view->model()->index(view->currentIndex().row(),0).data().toString();
        QSqlQuery query;
        QString q = QString("delete from samochody where rejestracja='%1'").arg(nr);
        query.exec(q);
        model->select();
        model->revertAll();
    });
    connect(submitButton, &QPushButton::clicked, this, &Widget::submit);
    connect(revertButton, &QPushButton::clicked,  model, &QSqlTableModel::revertAll);
    connect(quitButton, &QPushButton::clicked, this, &Widget::close);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(view);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Samochody"));

}

void Widget::closeEvent(QCloseEvent *event){
    this->deleteLater();
    QWidget::closeEvent(event);
}

Widget::~Widget()
{
    if (!progress_pointer.isNull()){
        progress_pointer->deleteLater();
    }

    if (!anw_pointer.isNull()){
        anw_pointer->deleteLater();
    }
}

void Widget::submit()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
    } else {
        model->database().rollback();
        QMessageBox::warning(this, tr("Samochody"),
                             tr("Error: %1")
                             .arg(model->lastError().text()));
    }
}
