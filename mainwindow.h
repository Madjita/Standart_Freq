#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCore>

#include <QDebug>
#include <portmodel.h>

#include <QComboBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    PortModel* port;






private slots:
    void on_pushButton_connectCOM_clicked();
    void slot_signal_OkCOM(bool ok);
    void slot_getCurrentFreq(int32_t currentFreq);

    //bool event(QEvent *event);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
