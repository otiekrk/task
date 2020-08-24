#ifndef WIDGET_H
#define WIDGET_H

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
#include <QTimer>

#include <QDebug>

class progress : public QWidget
{
public:
    progress(QWidget *parent = nullptr);
private:
    bool breakMe = false;
    void refresh();
    QProgressBar *progressBar = new QProgressBar(this);
    QPushButton * stopButton = new QPushButton(tr("Stop"));
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

    QPushButton *extraButton = new QPushButton(tr("ProgressBar"));
    QVector<progress *> pList;
    QVector<AddNewWindow *> aList;
};

class AddNewWindow : public QWidget
{
    Q_OBJECT
public:
    AddNewWindow(Widget * parent = nullptr);
signals:
    void addNew(const QString &nr_rej, const QString &vin, const QString &model, const QString &marka);
private:
    QLabel * rejLabel = new QLabel(tr("Rejestracja :"));
    QLabel * vinLabel = new QLabel(tr("VIN :"));
    QLabel * modelLabel = new QLabel(tr("Model :"));
    QLabel * markaLabel = new QLabel(tr("Marka :"));
    QLineEdit * rejLine = new QLineEdit;
    QLineEdit * vinLine = new QLineEdit;
    QLineEdit * modelLine = new QLineEdit;
    QLineEdit * markaLine = new QLineEdit;
    QPushButton * saveButton = new QPushButton(tr("Zapisz"));
    QPushButton * clearButton = new QPushButton(tr("Wyczyść"));


};
#endif // WIDGET_H
