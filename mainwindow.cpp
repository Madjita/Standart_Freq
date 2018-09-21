#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    port = new PortModel();
    ui->comboBox_listCOM->setPort(port);
    ui->comboBox_listCOM->addItems(port->listCOM());

    connect(port,&PortModel::signal_Ok,this,&MainWindow::slot_signal_OkCOM);
    connect(port->PortNew,&Port::getCurrentFreq,this,&MainWindow::slot_getCurrentFreq);

    qRegisterMetaType<int32_t>("int32_t");


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_connectCOM_clicked()
{
    if(ui->pushButton_connectCOM->text() == "Подключить")
    {
        port->openPort(ui->comboBox_listCOM->currentText());
        ui->comboBox_listCOM->setEnabled(false);
        ui->lineEdit->clear();
    }
    else
        port->closePort();
}

void MainWindow::slot_signal_OkCOM(bool ok)
{
    if(ok)
    {
        ui->pushButton_connectCOM->setText("Отключить");
        ui->pushButton_connectCOM->setStyleSheet("background-color: green");
    }
    else
    {
        ui->pushButton_connectCOM->setText("Подключить");
        ui->pushButton_connectCOM->setStyleSheet("");
        ui->comboBox_listCOM->setEnabled(true);
    }
}

void MainWindow::slot_getCurrentFreq(int32_t currentFreq)
{
    ui->label->setText(QString::number(currentFreq));
}




void MainWindow::on_pushButton_clicked()
{
    QByteArray b;

    b.append(static_cast<char>(0x2D));
    b.append(static_cast<char>(0x04));
    b.append(static_cast<char>(0x00));
    b.append(static_cast<char>(0x29));

    port->PortNew->writeData(b);



    //    QByteArray dataBuild;
    //    dataBuild.append(0x2D);
    //    dataBuild.append(0x09);
    //    dataBuild.append('\0');
    //    dataBuild.append(0x24);
    //    dataBuild.append(0xFF);
    //    dataBuild.append(0xF0);
    //    dataBuild.append(0xFF);
    //    dataBuild.append(0xFF);
    //    dataBuild.append(0x21);


    //    int32_t currentFreq;


    //    if(dataBuild[0] == static_cast<char>(0x2D))
    //    {
    //           dataBuild.remove(0,4);
    //           dataBuild.remove(dataBuild.count()-1,1);
    //           dataBuild = Reverse(dataBuild);

    //           currentFreq =0;
    //           memcpy(&currentFreq, dataBuild.data(),dataBuild.size());


    //          // currentFreq = *(int32_t*)dataBuild.data();

    //           qDebug() <<"currentFreq = " <<  currentFreq;

    //           dataBuild.clear();

    //           return;
    //    }

}

QByteArray Reverse(QByteArray _b)
{
    int n = _b.count();
    QByteArray rez;

    for(int i=0 ; i < n;i++)
    {
        rez.append(_b[_b.count()-1]);
        _b.remove(_b.count()-1,1);
    }

    return rez;
}

void MainWindow::on_pushButton_2_clicked()
{
    QByteArray b;

    b.append(static_cast<char>(0x2E));
    b.append(static_cast<char>(0x09));
    b.append(static_cast<char>(0x00));
    b.append(static_cast<char>(0x27));

    int32_t  _x = ui->lineEdit->text().toInt();

    auto m = Reverse(QByteArray::fromHex(QString("%1").arg(_x,0,16).toUtf8()));
    if(m.count() > 4)
        m.remove(4,m.count()-4);
    else
    {
        while(m.count() < 4)
        {
            m.append('\0');
        }

        m = Reverse(m);

    }

    if(_x < 0)
        m = Reverse(m);

    b.append(m);

    char c = '\0';

    for(int i=0; i <m.count();i++)
    {
        c^=m[i];
    }

    b.append(c);

    port->PortNew->writeData(b);

}

void MainWindow::on_pushButton_3_clicked()
{
    QByteArray b;

    b.append(static_cast<char>(0x2C));
    b.append(static_cast<char>(0x09));
    b.append(static_cast<char>(0x00));
    b.append(static_cast<char>(0x25));

    int32_t  _x = ui->lineEdit->text().toInt();

    auto m = Reverse(QByteArray::fromHex(QString("%1").arg(_x,0,16).toUtf8()));
    if(m.count() > 4)
        m.remove(4,m.count()-4);

    m = Reverse(m);

    b.append(m);

    char c = '\0';

    for(int i=0; i <m.count();i++)
    {
        c^=m[i];
    }

    b.append(c);

    port->PortNew->writeData(b);
}
