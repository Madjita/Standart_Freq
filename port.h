#ifndef PORT_H
#define PORT_H

#include <basetsd.h>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/qserialport.h>
#include <QTimer>
#include <QDebug>
#include <QStringList>

typedef unsigned char byte;

struct Settings
{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};


#pragma pack(push, 1)
struct amplituda
{
    __int16 A1L1[24]; // Амплиуда //__int16
};
#pragma pack(pop)



#pragma pack(push, 1)
struct frame1
{
     byte NkaLit;
     byte NkaNum;
     byte Ka1Mode;
     byte Ka2Mode;
     byte TechParam1[2];
     byte TechParam2[2];
     __int16 L1dopplerOffset;
     __int16 LongCodeL1dopplerOffset;
     __int8 rmsPhaseDev;
     __int8 recvStringNum;
     __int8 recvFrameNum;
     byte TechParam3;
     byte TechParam4[2];
     __int16 ant1SignalAmplitude;
     __int16 ant2SignalAmplitude;
     __int16 ant3SignalAmplitude;
};
#pragma pack(pop)




//Структура с данными
//#pragma pack(push, 1)
struct frameExample
{

    byte NkaLit[24];  // Номер Литиры
    byte NkaNum[24];  // Номер НКА
    byte Ka1Mode[24]; // Режим КА1
    //    byte Ka2Mode[24]; // Режим КА2
    amplituda* A1L1;// Амплиуда

};
//#pragma pack(pop)

Q_DECLARE_METATYPE(frameExample);

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = nullptr);

    //Инициализация таймеров
    QTimer* timer_MRK_Data;

    int CountFindGPS;
    int CountFindGLONASS;
    int CountFindALL;

    bool flag_start_MRK;
    bool flag_end_MRK;

    QByteArray dataBuild;
    QByteArray nameMRK;

    bool flag_GetMrk_liters = false;


    QStringList* listSP;
    QStringList* listSP_Amplitude;
    QStringList* listSP_Name;

    QByteArray data;
    QByteArray lol;

    int position_liters;
    int position_number;
    int position_Mode;
    int position_A;

    unsigned char RX[16384]; // разобранный пакет

    char D0;
    char D1;
    char T;
    char H;
    char P;
    bool flagErrorKS;


public:
    ~Port();

    void RazborFrame(frameExample);

public:
    QSerialPort* thisPort;
    Settings SettingsPort;
    frameExample * container;

    frame1 * frame[24];



    //
    int32_t currentFreq;

    //


    bool flag;
    bool flag_work;
    bool flag_getName;
    bool flag_waitloadingMRK;
    bool flag_waitloadingMRK_proverka;


    //обработка пакета//////////////////////////////////////////////////
    unsigned short buf;	//	последние два принятых байта
    unsigned long n_rx;	//	размер принятого пакета - только данные
    unsigned char crc;	//	контрольная сумма
    unsigned short crc16;// контрольная сумма crc16
    int unpackWINFrame(unsigned char ch);
    bool begin_flag;
    QByteArray receiveBuf;

signals:
    void finihed_Port(); // Сигнал закртытия класса
    void error_(QString err); // Сигнал ошибок порта
    void outPort(QString data); // Сигнал вывода полученных данных
    void StopTimer();
    void outMRKdata(frameExample);

    void outPortOpen(QString);

    void startPort();
    void startWork();
    void endWork();

    void startTimerMrk(int);
    void stopTimerMrk();
    void UpdateCountFind(int,int,int);

    void signal_GoTORelizproverka(QStringList,QStringList,QStringList);

    void writeData(QByteArray);

    void start_UdpZapros();

    void error_10Mgz();

    void signal_MRkLoad(bool);

    void signal_MRkLoadProverka(bool);

    void signal_MRkgetName(QByteArray);

    //C310
    void openK2();
    void openK1();

    void getMessage(QByteArray,QByteArray);

    void getCurrentFreq(int32_t);

public slots:
    void DisconnectPort(); // Слот отключения порта
    void ConnectPort(); // Слот подключения порта
    void Write_Setting_Port(QString name, int baudrate, int DataBits, int parity, int StopBits, int FlowControl); // Слот занесение настроек порта в класс
    void process_Port(); // Тело
    void WriteToPort(QByteArray data); // Слот отправки данных в порт
    void GetMrk();
    void GetMrk_OT();
    void GetMrk_liters(int);
    void GetMrk_liters_2(int);

    void GetMrk_Name();


    void END();
    void Work();

    void process_start();

    QByteArray Reverse(QByteArray _b);

private slots:
    void ReadInProt(); //Слот чтения из порта по ReadyRead
    void ReadInProt_nameNP101(QByteArray _nameMRK); //Слот чтения из порта по ReadyRead имя приемника

    int RecvByte(unsigned char ch);


    void getComData();





public:
};

#endif // PORT_H
