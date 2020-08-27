#pragma once

#include <QWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QTableView>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QCloseEvent>
#include <QPointer>
#include <QThread>
#include <QTimer>

#include <QDebug>

class loadProgress : public QObject
{
    Q_OBJECT
public :
    loadProgress();
    ~loadProgress();
public slots:
    void process();
    void breakMe();
    void breakMe2();
signals:
    void breakDone();
    void newValue(int v);
    void finished();
    void continueThread();
private:
    int value = 0;
    bool breaked;
    bool ref;
};

class progress : public QWidget
{
    Q_OBJECT
public:
    progress(QWidget *parent = nullptr);
public slots:
    void destructMe();
    void setProgressValue(int v);
signals:
    void breakThread();
protected:
    void closeEvent(QCloseEvent *event);
private:
    QProgressBar *progressBar = new QProgressBar(this);
    QPushButton * stopButton = new QPushButton(tr("Stop"),this);
    QThread * mainThread;
};

class AddNewWindow;

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void closeEvent(QCloseEvent *event);
public slots :
    void newCar(const QString &nr_rej, const QString &vin, const QString &model, const QString &marka);
private slots:
    void submit();
private:
    QPushButton *submitButton;
    QPushButton *revertButton;
    QPushButton *quitButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QDialogButtonBox *buttonBox;
    QSqlTableModel *model;

    QPushButton *extraButton = new QPushButton(tr("ProgressBar"),this);
    QPointer<progress> progress_pointer;
    QPointer<AddNewWindow> anw_pointer;
    QThread mainThread;
};

class AddNewWindow : public QWidget
{
    Q_OBJECT
public:
    AddNewWindow(Widget * parent = nullptr);
    ~AddNewWindow();
protected:
    void closeEvent(QCloseEvent *event);
signals:
    void addNew(const QString &nr_rej, const QString &vin, const QString &model, const QString &marka);
private:
    QLabel *  rejLabel = new QLabel(tr("Rejestracja :"),this);
    QLabel * vinLabel = new QLabel(tr("VIN :"),this);
    QLabel * modelLabel = new QLabel(tr("Model :"),this);
    QLabel * markaLabel = new QLabel(tr("Marka :"),this);
    QLineEdit * rejLine = new QLineEdit(this);
    QLineEdit * vinLine = new QLineEdit(this);
    QLineEdit * modelLine = new QLineEdit(this);
    QLineEdit * markaLine = new QLineEdit(this);
    QPushButton * saveButton = new QPushButton(tr("Zapisz"),this);
    QPushButton * clearButton = new QPushButton(tr("Wyczyść"),this);


};
