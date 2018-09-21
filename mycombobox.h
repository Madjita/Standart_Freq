#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QObject>
#include <QComboBox>
#include <QDebug>

#include <portmodel.h>

class MyComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MyComboBox(QObject* parent);

    PortModel* port;

signals:

public slots:
        void setPort(PortModel* _port);

private slots:
    void mousePressEvent(QMouseEvent *e);
};

#endif // MYCOMBOBOX_H
