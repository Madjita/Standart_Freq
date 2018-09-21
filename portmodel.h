#ifndef PORTMODEL_H
#define PORTMODEL_H

#include <QObject>
#include <QThread>
#include <QDebug>

//Класс порта RS232
#include <port.h>
#include <QSerialPortInfo>

class PortModel : public QObject
{
    Q_OBJECT
public:
    explicit PortModel(QObject *parent = nullptr);

    Q_PROPERTY(QStringList listCOM READ listCOM NOTIFY listCOMChanged)

    const QStringList listCOM() const;


    Q_PROPERTY(QString nameOpenPort READ nameOpenPort NOTIFY nameOpenPortChanged)
    const QString nameOpenPort() const;

    Q_PROPERTY(bool mrkLoad READ mrkLoad NOTIFY mrkLoadChanged)
    bool mrkLoad() const;


    int CountFindGPS;
    int CountFindGLONASS;
    int CountFindALL;


    QStringList  GetlistSP;
    QStringList  GetlistSP_Amplitude;
    QStringList  GetlistSP_Name;


    //Порт RS232 Инициализация
    Port *PortNew;

signals:
    void listCOMChanged(QStringList);  //Сигнал Изменения списка ком портов
    void nameOpenPortChanged(QString);  // Сигнал отправки имени открытого измененного порта
    void open(); // Сигнал на открытие
    void mrkLoadChanged(bool mrkLoad);
    void mrkLoadProverkaChanged(bool mrkLoad);

    void Work();
    void endWork();

    void Write_Setting_Port(QString ,int , int , int , int , int);
    void DisconnectPort();
    void UpdateCountFindQML(int,int,int);

    void signal_setListSpytnik(QStringList,QStringList,QStringList);

    void signal_Ok(bool good);
    void signal_GetNameMRK();

    void sendNameMRK();

public slots:
    void openPort(QString); // Слот на открытие порта
    void openOK(QString ok);  // Слот на правильность открытия порта
    void closePort();  // Слот на закрытие порта

    void slot_Work();
    void slot_endWork();

    void UpdateCountFind(int,int,int);

    void setListSpytnik(QStringList,QStringList,QStringList);

    void slot_getNameMRK();
    void slot_MRkLoad(bool);
    void slot_MRkLoadProverka(bool);
    void slot_MRkGetName(QByteArray nameMRK);

    void slot_updateListCOM();


private:
    QStringList m_listCOM; // Лист доступных портов
    QString name;  // Название порта
    bool _mrkLoad;
    bool _mrkLoadProverka;
};

#endif // PORTMODEL_H
