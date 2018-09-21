#include "mycombobox.h"

MyComboBox::MyComboBox(QObject* parent)
{

}

void MyComboBox::setPort(PortModel *_port)
{
    port = _port;

}

void MyComboBox::mousePressEvent(QMouseEvent *e)
{
    this->clear();
    port->slot_updateListCOM();
    this->addItems(port->listCOM());

    if(this->count() <= 0)
        this->addItem("COM не найден");


    QComboBox::mousePressEvent(e);
}

