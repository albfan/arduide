/**
 * \file DeviceChooser.cpp
 * \author Denis Martinez
 */

#include "DeviceChooser.h"

#include "env/Device.h"
#include "env/Settings.h"
#include "IDEApplication.h"

DeviceChooser::DeviceChooser(QWidget *parent)
    : QMenu(parent),
      actionGroup(NULL)
{
    setTitle(tr("Device"));
    refresh();
}

void DeviceChooser::refresh()
{
    QString defaultPort = ideApp->settings()->devicePort();
    delete actionGroup;
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onActionTriggered(QAction *)));

    clear();

    QAction *action;
    foreach (const Device &dev, Device::listDevices())
    {
        QString name = QString("%0: %1").arg(dev.port()).arg(dev.description());
        const QString &port = dev.port();
        action = new QAction(name, actionGroup);
        action->setData(port);
        action->setCheckable(true);
        if (port == defaultPort)
            action->setChecked(true);
        addAction(action);
    }
}

void DeviceChooser::onActionTriggered(QAction *action)
{
    emit deviceChosen(action->data().toString());
}