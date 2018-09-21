#include "port.h"


#include <QThread>

Port::Port(QObject *parent) :
    QObject(parent),
    CountFindALL(0),
    flag_start_MRK(false),
    position_liters(0),
    position_number(1),
    position_Mode(2),
    position_A(18),
    // thisPort(new QSerialPort()),
    flag_work(false),
    flag_getName(false),
    flag_waitloadingMRK(false),
    flag_waitloadingMRK_proverka(false)
{
    //помещаем класс в поток
    this->moveToThread(new QThread());
    connect(this->thread(),&QThread::started,this,&Port::process_start);
    this->thread()->start();

}

Port::~Port()
{
    qDebug("By in Thread!");
    emit finihed_Port(); // Сигнал о завершении работы

}

void Port::process_start()
{

    D0 ='\0';
    D1 ='\0';
    T = '\0';
    H = '\0';
    P = '\0';
    flagErrorKS = false;



    thisPort = new QSerialPort();



    qDebug("Open the port.cpp on the new Thread");

    connect(thisPort, &QSerialPort::readyRead,this,&Port::ReadInProt); //подключаем чтение с порта по сигналу readyRead()

    //connect(thisPort, SIGNAL(readyRead()),this,SLOT(getComData())); //подключаем чтение с порта по сигналу readyRead()

    listSP = new QStringList();

    listSP_Amplitude = new QStringList();
    listSP_Name = new QStringList();

    container = new frameExample();

    for(int i=0; i < 24;i++)
    {
        frame[i] = new frame1();
    }


    connect(  thisPort->thread(),&QThread::started, this,&Port::process_Port); // Переназначения метода run
    connect(  this, &Port::finihed_Port,  thisPort->thread(),&QThread::quit); // Переназначение метода выход
    connect(  thisPort->thread(),&QThread::finished, this,&Port::deleteLater); // Удалить к чертям поток
    connect(  this,&Port::finihed_Port,  thisPort->thread(),&QThread::deleteLater); // Удалить к чертям поток


    //Инициализация таймеров
    timer_MRK_Data = new QTimer();

    connect(timer_MRK_Data,&QTimer::timeout,this,&Port::GetMrk);
    connect(this,SIGNAL(startTimerMrk(int)),timer_MRK_Data,SLOT(start(int)));
    connect(this,&Port::stopTimerMrk,timer_MRK_Data,&QTimer::stop);


    connect(this, &Port::start_UdpZapros,this,&Port::GetMrk);


    //connect(this,SIGNAL(writeData(QByteArray)),this,SLOT(WriteToPort(QByteArray))); // отправить данные
    connect(this,&Port::writeData,this,&Port::WriteToPort);


    qRegisterMetaType<QVector<int>>();

}


void Port::process_Port() //Выполняется при старте класса
{

    qDebug("Open the port.cpp on the new Thread");

    connect(thisPort, SIGNAL(readyRead()),this,SLOT(ReadInProt())); //подключаем чтение с порта по сигналу readyRead()


    listSP = new QStringList();

    listSP_Amplitude = new QStringList();
    listSP_Name = new QStringList();

    container = new frameExample();

    for(int i=0; i < 24;i++)
    {
        frame[i] = new frame1();
    }


}

void Port::Write_Setting_Port(QString name, int baudrate, int DataBits, int parity, int StopBits, int FlowControl) //заносим параметры порта в структуру данных
{
    SettingsPort.name = name;

    SettingsPort.baudRate = static_cast<QSerialPort::BaudRate>(baudrate);
    SettingsPort.dataBits = static_cast<QSerialPort::DataBits>(DataBits);
    SettingsPort.parity = static_cast<QSerialPort::Parity>(parity);
    SettingsPort.stopBits = static_cast<QSerialPort::StopBits>(StopBits);
    SettingsPort.flowControl = static_cast<QSerialPort::FlowControl>(FlowControl);

    //        SettingsPort.baudRate = (QSerialPort::BaudRate) baudrate;
    //        SettingsPort.dataBits = QSerialPort::Data8;
    //        SettingsPort.parity = QSerialPort::NoParity;
    //        SettingsPort.stopBits = QSerialPort::OneStop;
    //        SettingsPort.flowControl = QSerialPort::NoFlowControl;
}


void Port::ConnectPort(void) //Процедура подключения
{
    thisPort->setPortName(SettingsPort.name);


    if(thisPort->open(QIODevice::ReadWrite))
    {
        if(thisPort->setBaudRate(SettingsPort.baudRate) && thisPort->setDataBits(SettingsPort.dataBits)&& thisPort->setParity(SettingsPort.parity)&& thisPort->setStopBits(SettingsPort.stopBits)&& thisPort->setFlowControl(SettingsPort.flowControl))
        {
            if(thisPort->isOpen())
            {
                //Открыт порт
                flag_work = true;

                emit outPortOpen(SettingsPort.name);
            }
        }
        else
        {
            flag_work = false;
            flag_start_MRK = false;

            emit outPortOpen(thisPort->errorString());

            thisPort->close();
        }
    }else
    {
        thisPort->close();
        emit outPortOpen(thisPort->errorString());
    }
}


void Port::DisconnectPort() //Отключаем порт
{

    if(thisPort->isOpen())
    {
        flag_work = false;
        flag_start_MRK = false;
        dataBuild.clear();
        emit stopTimerMrk();
        thisPort->close();
        error_(SettingsPort.name.toLocal8Bit() + " >> Закрыт!\r");

        emit outPortOpen("Device is not open");
    }
}

void Port::WriteToPort(QByteArray data)//запись данных в порт
{

    if(thisPort->isOpen())
    {
        thisPort->write(data);
        thisPort->flush();
    }
}


//Остановка таймера без закрытия порта
void Port::END()
{
    emit stopTimerMrk();
}

void Port::Work()
{
    emit startTimerMrk(500);
}

void Port::GetMrk()
{

    //Запрос 1 кадра
    QByteArray b;
    b.append(static_cast<char>(0xfe));
    b.append(static_cast<char>(0xfe));
    b.append(0x02);
    b.append(0x13);
    b.append(0x01);
    b.append(0x01);
    b.append(static_cast<char>(0xff));
    b.append(static_cast<char>(0xff));

    char c = '\0';

    for(int i=2; i < b.count()-3;i++)
    {
        c ^=b[i];
        b[b.count()-3] = c;
    }

    emit writeData(b);

}

void Port::GetMrk_OT()
{
    emit stopTimerMrk();

    flag_GetMrk_liters = true;
    //qDebug() << " START 2 ZAPROS  LITERS OT ";

    QByteArray b;
    // Запрос 2 кадра
    b.append(static_cast<char>(0xfe));
    b.append(static_cast<char>(0xfe));
    b.append(0x02);
    b.append(0x13);
    b.append(0x02);
    b.append(0x01);
    b.append(static_cast<char>(0xff));
    b.append(static_cast<char>(0xff));

    char c = '\0';

    for(int i=2; i < b.count()-3;i++)
    {
        c ^=b[i];
        b[b.count()-3] = c;
    }

    emit writeData(b);
}




void Port::GetMrk_liters(int liter)
{
    emit stopTimerMrk();

    //qDebug() << "Установка литеры в 24 канала ";

    QByteArray b;

    b.append(static_cast<char>(0xfe));
    b.append(static_cast<char>(0xfe));
    b.append(0x02);
    b.append(0x15);
    b.append(0x01);
    for(int i =0 ; i < 24;i++)
    {
        b.append(static_cast<char>(UINT8(i)));
        b.append(static_cast<char>(UINT8(liter)));          //UINT8(2) - вторая литера это 24 спутник

        b.append(0x78);
        b.append(static_cast<char>(0xEC));

        b.append(static_cast<char>(0x88));
        b.append(0x13);
    }

    b.append(0x01);
    b.append(static_cast<char>(0xff));
    b.append(static_cast<char>(0xff));


    char c = '\0';

    for(int i=2; i < b.count()-3;i++)
    {
        c ^=b[i];
        b[b.count()-3] = c;
    }

    thisPort->clear();

    emit writeData(b);

    emit startTimerMrk(500);

}


void Port::GetMrk_liters_2(int liter)
{
    QByteArray b;

    b.append(static_cast<char>(0xfe));
    b.append(static_cast<char>(0xfe));
    b.append(0x02);
    b.append(0x15);
    b.append(0x01);
    for(int i =22 ; i < 24;i++)
    {
        //qDebug() << i;
        b.append(static_cast<char>(UINT8(i)));
        b.append(static_cast<char>(UINT8(liter)));          //UINT8(2) - вторая литера это 24 спутник

        b.append(0x78);
        b.append(static_cast<char>(0xEC));

        b.append(static_cast<char>(0x88));
        b.append(0x13);
    }

    b.append(0x01);
    b.append(static_cast<char>(0xff));
    b.append(static_cast<char>(0xff));


    char c = '\0';

    for(int i=2; i < b.count()-3;i++)
    {
        c ^=b[i];
        b[b.count()-3] = c;
    }


    thisPort->clear();

    emit writeData(b);

    emit startTimerMrk(200);
}

void Port::GetMrk_Name()
{
    //Запрос Идентификационный номер платы навига-ционного приемника в составе МРК

    flag_getName = true;

    QByteArray b;
    b.append(static_cast<char>(0xfe));
    b.append(static_cast<char>(0xfe));
    b.append(0x02);
    b.append(0x18);
    b.append(static_cast<char>(0xA9));
    b.append('\0');
    b.append(0x01); //контрольная сумма
    b.append(static_cast<char>(0xff));
    b.append(static_cast<char>(0xff));

    char c = '\0';

    for(int i=2; i < b.count()-3;i++)
    {
        c ^=b[i];
        b[b.count()-3] = c;
    }

    emit writeData(b);
}




bool Proverka(QByteArray data)
{
    QByteArray ok;
    ok.append(static_cast<char>(0xfe));
    ok.append(static_cast<char>(0xfe));
    ok.append(0x02);
    ok.append('\0');
    ok.append(0x02);
    ok.append(static_cast<char>(0xff));
    ok.append(static_cast<char>(0xff));

    if(data == ok)
    {
        return true;
    }
    else
    {
        return false;
    }
}


//void Port::ReadInProt() // чтение данных из порта
//{


//    dataBuild.append(thisPort->readAll());


//    QByteArray data;


//    for(int i=0; i < dataBuild.count();i++)
//    {

//        data.append(dataBuild[i]);

//        if(dataBuild[i] == 0xff && dataBuild[i-1] == 0xff)
//        {
//            dataBuild.clear();
//            break;
//        }
//    }


//    if(dataBuild.count() > 0)
//    {
//        return ;
//    }




//  //  //qDebug() << "ReadInProt  << " <<  data;



//    if (data != NULL)
//    {

//        if(data.count() < 10)
//        {
//            flag = false;
//            flag = Proverka(data);
//            return ;

//        }


//        if(flag_GetMrk_liters == true && data[4] == 0x02  && data.count() > 10  && ( data[data.count()-1] == 0xff && data[data.count()-2] == 0xff))
//        {

//            //qDebug() << "START LITERS OT ";

//            flag_GetMrk_liters = false;

//            QByteArray b;

//            data[11] = 0x03;

//            //        auto lol = dataBuild[11]&0xf8;
//            //        dataBuild[11]=lol+2;

//            data.remove(0,4);
//            data.remove(data.count()-3,3);


//           b.append(static_cast<char>(0xfe));
//           b.append(static_cast<char>(0xfe));
//            b.append(0x02);
//            b.append(0x15);

//            for(int i = 0;i < data.count();i++)
//            {
//                b.append(data[i]);
//            }
//            b.append(0x01);
//           b.append(static_cast<char>(0xff));
//           b.append(static_cast<char>(0xff));
//            char c = NULL;

//            char c1 = NULL;

//            c1 = b[2]^b[3]^b[4];

//            for(int i=2; i < b.count()-3;i++)
//            {
//                c ^=b[i];
//                b[b.count()-3] = c;
//            }

//           // this->WriteToPort(b);
//            emit writeData(b);

//            dataBuild.clear();
//        }
//        else
//        {


//            if(data[data.count()-1] == 0xff && data[data.count()-2] == 0xff)
//            {

//             //   //qDebug() << "flag_end_MRK   << " <<  flag_end_MRK;

//                if(flag_end_MRK== true)
//                {
//                    dataBuild.clear();

//                    data = dataBuild;

//                    //                    CountFindGLONASS=0;
//                    //                    CountFindGPS=0;
//                    //                    CountFindALL=0;
//                    //                    emit UpdateCountFind(CountFindGLONASS,CountFindGPS,CountFindALL);
//                    return ;
//                }

//             //   //qDebug() << "flag_start_MRK   << " <<  flag_start_MRK;
//            //    //qDebug() << "flag   << " <<  flag;


//                if(flag)
//                {
//                    flag_start_MRK = true;

//                    //Удаление начала и конца принятого кадра
//                    data.remove(data.count()-3,3);
//                    data.remove(0,4);


//                    container = (frameExample *)data.data();

//                    //qDebug() << container->A1L1[0];

//                    RazborFrame(*container);

//                    emit outMRKdata(*container);

//                }

//                dataBuild.clear();
//            }
//        }
//    }
//}

QByteArray Port::Reverse(QByteArray _b)
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

void Port::ReadInProt() // чтение данных из порта
{

    dataBuild.append(thisPort->readAll());

    qDebug () << dataBuild;

    if(dataBuild[0] == static_cast<char>(0x2D))
    {
        dataBuild.remove(0,4);
        dataBuild.remove(dataBuild.count()-1,1);
        dataBuild = Reverse(dataBuild);

        currentFreq =0;
        memcpy(&currentFreq, dataBuild.data(),dataBuild.size());


       // currentFreq = *(int32_t*)dataBuild.data();

        qDebug() <<"currentFreq = " <<  currentFreq;

        dataBuild.clear();

        emit getCurrentFreq(currentFreq);

        return;
    }

    if(dataBuild[0] == static_cast<char>(0x2E))
    {

    }



//    //если не FE то выйти
//    if(dataBuild[0] != static_cast<char>(0xFE))
//    {

//        if(dataBuild.count() > 5)
//        {
//            thisPort->flush();
//            dataBuild.clear();
//            return;
//        }

//        return;
//    }

//    if(dataBuild.count() < 5)
//    {
//        return;
//    }


//    if((dataBuild[1]^dataBuild[2]^dataBuild[3]) != dataBuild[4])
//    {
//        qDebug () << "KS ERROR";
//        dataBuild.clear();
//        return;
//    }

//    qDebug () << "KS GOOD";

//    if(dataBuild[0] == static_cast<char>(0xFE)&&
//            dataBuild[1] == static_cast<char>(0xBB)&&
//            dataBuild[2] == static_cast<char>(0xDD)&&
//            dataBuild[3] == static_cast<char>(0x11))
//    {
//        qDebug () << "Get OFF Device";

//        D0 ='\0';
//        D1 ='\0';
//        T = '\0';
//        H = '\0';
//        P = '\0';

//        emit getMessage(dataBuild,"");

//        dataBuild.clear();
//        return;
//    }

//    if((dataBuild[1]&0x01))
//    {
//        emit openK2();
//    }
//    else
//    {
//        emit openK1();
//    }




//    QByteArray message;
//    message.append(static_cast<char>(0xFE));
//    message.append(D0);
//    message.append(D1);
//    message.append(T);
//    message.append(H);
//    message.append(P);
//    char KS = '\0';

//    qDebug() << "============";
//    for(int i=1; i < message.count();i++)
//    {
//        qDebug() <<QString::number(message[i]);
//        KS ^=message[i];
//    }

//    if(flagErrorKS)
//        message.append(KS+0x01);
//    else
//        message.append(KS);


//    qDebug() << QString::number(message[message.count()-1]);


//    emit getMessage(dataBuild,message);

//    emit WriteToPort(message);


    thisPort->flush();
    dataBuild.clear();


}

void Port::ReadInProt_nameNP101(QByteArray _nameMRK)
{
    nameMRK.clear();

    for(int i=0;i < _nameMRK.count();i++)
    {
        nameMRK.append(_nameMRK[i]);
    }

    if(nameMRK.count() < 10)
    {
        flag = false;
        flag = Proverka(nameMRK);

        nameMRK.clear();
        return ;

    }

    nameMRK.remove(0,4);
    nameMRK.remove(nameMRK.count()-3,3);
    qDebug() << "nameMRK_String = " << QString::fromLocal8Bit(nameMRK) << "nameMRK = " << nameMRK;

    emit signal_MRkgetName(nameMRK);
    flag_getName = false;
}






int Port::RecvByte(unsigned char ch)
{

    buf = (buf << 8) | ch;
    if (begin_flag == true){	// принимаем пакет
        if (ch == 0xFE){
            if (buf == 0xFEFE){
                //					Memo1->Lines->Add("ERR: another start of packet...\r\n");
                n_rx = 0; // начинаем заново собирать пакет
                crc = 0;
                buf = 0;
            }else{
                RX[n_rx] = ch;	n_rx++;	// записываем в буфер
                crc ^= ch;
            }
        }else if (ch == 0xFF){
            if (buf == 0xFFFF){
                n_rx -= 2;	// убиваем лишний FF от конца пакета, и контрольную сумму
                crc ^= ch;
                begin_flag = false;


                if (crc == 0){
                    //						ProcessPacket(&RX[0], n);
                    return 2;	//	пакет принят, можно обрабатывать
                }else{
                    //						Memo1->Lines->Add("crc error...\r\n");
                    n_rx = 0;
                    crc = 0;
                    return -1;
                }
            }else{
                RX[n_rx] = ch;	n_rx++;	// записываем в буфер
                crc ^= ch;
            }
        }else if (ch == 0x00){
            if (!(((buf&0xFF00) == 0xFE00)||((buf&0xFF00) == 0xFF00))){
                RX[n_rx] = ch;	n_rx++;
                crc ^= ch;
            }
        }else{
            if ((((buf&0xFF00) == 0xFE00)||((buf&0xFF00) == 0xFF00))){
                //					Memo1->Lines->Add("ERR: not zeros after FE or FF...\r\n");
            }else{
                RX[n_rx] = ch;	n_rx++;
                crc ^= ch;
            }
        }
    }else{
        if (buf == 0xFEFE){	// пришло начало пакета
            buf = 0;
            begin_flag = true;
            crc = 0;
            return 1;
        }
    }

    return 0;
}

void Port::getComData()
{
    bool recvFlag;
    QByteArray data;
    //char data[1];
    QByteArray receiveBuf;
    int i = 0;

    if(thisPort->bytesAvailable() != 0)
    {
        //connect(comPort, &QSerialPort::readyRead, this, &InterfaceCom::ignore);
        while((!recvFlag) && (thisPort->bytesAvailable() != 0))
        {
            data = thisPort->readAll();
            //comPort->getChar(data);
            //getThreadData(data,10);
            i = 0;
            while((i < data.count()) && (!recvFlag))
                //recvFlag = (builder->unpackFrame(tempArr[0]) == 2);
            {
                recvFlag = (unpackWINFrame(static_cast<unsigned char>(data[i])) == 2);

                if(recvFlag && data.count() > 10)
                {

                    receiveBuf = this->receiveBuf.remove(receiveBuf.count() - 1,1);




                    if((dataBuild == "") || ((static_cast<unsigned char>(dataBuild[dataBuild.count()-1]) != 0xFF) &&(static_cast<unsigned char>(dataBuild[dataBuild.count()-2]) != 0xFF)))
                    {
                        dataBuild.append(receiveBuf);
                    }
                    else
                    {

                        //Удаление начала и конца принятого кадра
                        dataBuild.remove(data.count()-2,2);
                        dataBuild.remove(0,3);


                        position_liters = 0;
                        position_number = 1;
                        position_Mode = 2;
                        position_A= 18;



                        container->NkaLit[0] = static_cast<unsigned char>(dataBuild[position_liters]);
                        container->NkaNum[0] = static_cast<unsigned char>(dataBuild[position_number]);
                        container->Ka1Mode[0] = static_cast<unsigned char>(dataBuild[position_Mode]);


                        lol.append(dataBuild[position_A]);
                        lol.append(dataBuild[position_A+1]);


                        for(int i=1;  i < 24;i++) //73
                        {
                            position_liters += 24;
                            position_number += 24;
                            position_Mode += 24;
                            position_A += 24;

                            container->NkaLit[i] = static_cast<unsigned char>(dataBuild[position_liters]);
                            container->NkaNum[i] = static_cast<unsigned char>(dataBuild[position_number]);
                            container->Ka1Mode[i] = static_cast<unsigned char>(dataBuild[position_Mode]);

                            lol.append(dataBuild[position_A]);
                            lol.append(dataBuild[position_A+1]);
                        }




                        container->A1L1 = reinterpret_cast<amplituda*>(lol.data());

                        RazborFrame(*container);

                        dataBuild.clear();
                    }

                    // getThreadData(builder->getRecvBuf(),builder->getCurProtocol());

                    receiveBuf.clear();
                    this->receiveBuf.clear();
                    recvFlag = false;
                }

                i++;
            }
        }
    }
}

int Port::unpackWINFrame(unsigned char ch)
{
    buf = (buf << 8) | ch;

    if (begin_flag == true){	// принимаем пакет
        if (ch == 0xFE){
            if (buf == 0xFEFE){
                //Memo1->Lines->Add("ERR: another start of packet...\r\n");

                receiveBuf.append(static_cast<char>(ch));
                receiveBuf.append(static_cast<char>(ch));

                n_rx = 0; // начинаем заново собирать пакет
                crc = 0;
                buf = 0;
            }else{
                receiveBuf.append(static_cast<char>(ch));
                //RX[n_rx] = ch;	n_rx++;	// записываем в буфер
                crc ^= ch;
            }
        }else if (ch == 0xFF){
            if (buf == 0xFFFF){
                receiveBuf.append(static_cast<char>(ch));
                receiveBuf.append(static_cast<char>(ch));

                buf = 0;
                n_rx -= 2;	// убиваем лишний FF от конца пакета, и контрольную сумму
                crc ^= ch;
                begin_flag = false;


                if (crc == 0){
                    //						ProcessPacket(&RX[0], n);
                    return 2;	//	пакет принят, можно обрабатывать
                }else{
                    //Memo1->Lines->Add("ERR: crc error...\r\n");
                    n_rx = 0;
                    crc = 0;
                    return -1;
                }
            }else{
                receiveBuf.append(static_cast<char>(ch));
                n_rx++;	// записываем в буфер
                crc ^= ch;
            }
        }else if (ch == 0x00){
            if (!(((buf&0xFF00) == 0xFE00)||((buf&0xFF00) == 0xFF00))){
                //receiveBuf.append(ch);
                n_rx++;
                crc ^= ch;
            }
        }else{
            if ((((buf&0xFF00) == 0xFE00)||((buf&0xFF00) == 0xFF00))){
                //Memo1->Lines->Add("ERR: not zeros after FE or FF...\r\n");
            }else{
                // receiveBuf.append(ch);
                n_rx++;
                crc ^= ch;
            }
        }
    }else{
        if (buf == 0xFEFE){

            receiveBuf.append(static_cast<char>(ch));
            receiveBuf.append(static_cast<char>(ch));

            // пришло начало пакета
            buf = 0;
            begin_flag = true;
            crc = 0;
            return 1;
        }
    }
    return 0;
}




void Port::RazborFrame(frameExample container)
{

    CountFindGLONASS=0;
    CountFindGPS=0;
    CountFindALL=0;



    listSP->clear();
    listSP_Amplitude->clear();
    listSP_Name->clear();

    for(int i=0; i < 24;i++)
    {
        listSP->append(QString::number(container.Ka1Mode[i]));

        listSP_Amplitude->append(QString::number(container.A1L1->A1L1[i]));

        if(container.NkaNum[i] > 100)
        {
            auto positionGPS =  QString::number(container.NkaLit[i],2);

            bool ok;
            auto lol =  QString::number(container.NkaLit[i],2);
            lol.remove(0,1);

            listSP_Name->append("G"+QString::number(lol.toInt(&ok,2)));

        }
        else
        {
            if(container.NkaLit[i] < 32 && container.NkaLit[i] > 24)
            {
                //Преобразование отрицательной литеры

                listSP_Name->append("R"+QString::number(container.NkaNum[i])+"/"+QString::number(QString::number(container.NkaLit[i],10).toInt()-32));
            }
            else
            {
                listSP_Name->append("R"+QString::number(container.NkaNum[i])+"/"+QString::number(container.NkaLit[i]));
            }
        }

        if(container.Ka1Mode[i] == 7)
        {
            CountFindALL++;

            if(container.NkaNum[i] > 100)
            {
                auto positionGPS =  QString::number(container.NkaLit[i],2);


                CountFindGPS++;
            }
            else
            {
                if(container.NkaLit[i] < 32 && container.NkaLit[i] > 24)
                {
                    //Преобразование отрицательной литеры

                    CountFindGLONASS++;
                }
                else
                {

                    CountFindGLONASS++;
                }
            }
        }
    }

    //  qDebug() <<"listSP = " << listSP;

    if(flag_waitloadingMRK)
    {
        if(flag_waitloadingMRK && flag_waitloadingMRK_proverka)
        {
            emit signal_MRkLoadProverka(true);
            flag_waitloadingMRK = false;
            flag_waitloadingMRK_proverka = false;
        }
        else
        {
            emit stopTimerMrk();
            emit signal_MRkLoad(true);
            flag_waitloadingMRK = false;
            flag_waitloadingMRK_proverka = false;
        }

    }
    else
    {
        emit signal_GoTORelizproverka(*listSP,*listSP_Amplitude,*listSP_Name);
        emit UpdateCountFind(CountFindGLONASS,CountFindGPS,CountFindALL);

    }

    listSP->clear();
    listSP_Amplitude->clear();
    listSP_Name->clear();


    data.clear();
    dataBuild.clear();

    lol.clear();

}

