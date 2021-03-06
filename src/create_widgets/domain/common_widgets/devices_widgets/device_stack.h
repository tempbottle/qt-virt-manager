#ifndef DEVICE_STACK_H
#define DEVICE_STACK_H

#include <QDialog>
#include <QSettings>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>
#include <QScrollArea>
#include "create_widgets/common/_qwidget.h"
#include "virt_objects/_virt_thread.h"

class devstackHelpThread : public _VirtThread
{
public:
    explicit devstackHelpThread(
            QObject         *parent     = nullptr,
            virConnectPtr   *connPtrPtr = nullptr);
    QString         connType;
    virtual void    run();
};

class DeviceStack : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceStack(
            QWidget        *parent  = nullptr,
            virConnectPtr*  conn    = nullptr);

signals:
    void             errorMsg(const QString&);
    void             ptrIsNull();

private:
    QSettings        settings;
    QStringList      devices;
    QStringList      nets;
    QListWidget     *deviceList;
    QPushButton     *addDevice;
    QPushButton     *cancel;

    QVBoxLayout     *infoLayout;
    QHBoxLayout     *listLayout;
    QHBoxLayout     *buttonlayout;
    QVBoxLayout     *commonLayout;

    QScrollArea     *infoWidget;
    QWidget         *scrolled = nullptr;
    QWidget         *listWidget;
    QWidget         *buttons;

    virConnectPtr*   ptr_ConnPtr;
    virErrorPtr      virtErrors;

    _QWidget        *device = nullptr;
    QLabel          *devIcon = nullptr;
    devstackHelpThread
                    *hlpThread;

public slots:
    QDomDocument     getResult() const;
    void             clearDevice();

private slots:
    void             init_wdg();
    void             showDevice(QListWidgetItem*);
    void             showDevice();
    void             set_Result();
    void             deviceDataProcessed();
};

#endif // DEVICE_STACK_H
