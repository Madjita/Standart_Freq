#include "portmodel.h"

PortModel::PortModel(QObject *parent) : QObject(parent)
{

    PortNew = new Port();

  //  connect(PortNew,SIGNAL(outMRKdata(frameExample)), this,SLOT(Update_MRK_Ttable(frameExample))); //По нажатию кнопки подключить порт


    qRegisterMetaType<frameExample>();

    //Сигнал на подключение порта
    connect(this,&PortModel::open, PortNew,&Port::ConnectPort);
    //Сигнал от порта к QML , что он открыт или ошибка
    connect(PortNew,&Port::outPortOpen, this,&PortModel::openOK);



    connect(PortNew,&Port::signal_MRkLoad,this,&PortModel::slot_MRkLoad);
    connect(PortNew,&Port::signal_MRkLoadProverka,this,&PortModel::slot_MRkLoadProverka);
    connect(PortNew,&Port::signal_MRkgetName,this,&PortModel::slot_MRkGetName);


    m_listCOM.clear();

    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_listCOM << info.portName(); // Находит доступные порты

    }

    qDebug () <<"Доступные порты: " <<  m_listCOM.toVector();

    emit listCOMChanged(m_listCOM);

    connect(this,&PortModel::Work,PortNew,&Port::Work);

    connect(this,&PortModel::endWork,PortNew,&Port::END);

    connect(this,&PortModel::Write_Setting_Port,PortNew,&Port::Write_Setting_Port);

    connect(this,&PortModel::DisconnectPort,PortNew,&Port::DisconnectPort);

    connect(PortNew,&Port::UpdateCountFind,this,&PortModel::UpdateCountFind);


   // connect(PortNew,&Port::signal_GoTORelizproverka,this,&PortModel::setListSpytnik);


}

const QStringList PortModel::listCOM() const
{
    return   m_listCOM;
}

const QString PortModel::nameOpenPort() const
{
    return name;
}

bool PortModel::mrkLoad() const
{
    return _mrkLoad;
}

void PortModel::openPort(QString name)
{
   emit Write_Setting_Port(name,9600,8,0,1,0);

   qDebug() << " PortNew->Write_Setting_Port = " << name;

   emit open();
}

void PortModel::openOK(QString ok)
{

    if("Device is not open" == ok || "Неизвестная ошибка" == ok)
    {
        emit signal_Ok(false);
    }
    else
    {
        emit signal_Ok(true);
    }


    name = ok;

    qDebug() << "Open port name = " << name;
}

void PortModel::closePort()
{
    emit  DisconnectPort();
}

void PortModel::slot_Work()
{
   emit Work();
}

void PortModel::slot_endWork()
{
    emit endWork();
}


void PortModel::setListSpytnik(QStringList listSP,QStringList listSP_Amplitude,QStringList listSP_Name)
{



    GetlistSP = listSP;

    GetlistSP_Amplitude = listSP_Amplitude;

    GetlistSP_Name = listSP_Name;

    emit signal_setListSpytnik(GetlistSP,GetlistSP_Amplitude,GetlistSP_Name);

    GetlistSP.clear();
    GetlistSP_Amplitude.clear();
    GetlistSP_Name.clear();


}

void PortModel::slot_getNameMRK()
{
    emit signal_GetNameMRK();
}

void PortModel::slot_MRkLoad(bool load)
{
   _mrkLoad = load;

   emit mrkLoadChanged(load);
}

void PortModel::slot_MRkLoadProverka(bool load)
{
    _mrkLoadProverka = load;

    emit mrkLoadProverkaChanged(load);
}

void PortModel::slot_MRkGetName(QByteArray nameMRK)
{
    emit sendNameMRK();
}

void PortModel::slot_updateListCOM()
{
    m_listCOM.clear();

//    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();

//    for(int i=0; i < list.count();i++)
//    {
//        m_listCOM.append(list[i].portName());

//    }


    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_listCOM.append(info.portName()); // Находит доступные порты

    }

    qDebug () <<"Доступные порты: " <<  m_listCOM.toVector();

    emit listCOMChanged(m_listCOM);
}

void PortModel::UpdateCountFind(int CountFindGLONASSGet,int CountFindGPSGet,  int CountFindALLGet)
{

    CountFindGPS = CountFindGPSGet;
    CountFindGLONASS = CountFindGLONASSGet;
    CountFindALL = CountFindALLGet;

    emit UpdateCountFindQML(CountFindGLONASS,CountFindGPS,CountFindALL);
}
