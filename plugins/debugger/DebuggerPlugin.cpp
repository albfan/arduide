#include "DebuggerPlugin.h"
#include "DebuggerWidget.h"

#include "IDEApplication.h"

bool DebuggerPlugin::setup(IDEApplication *app)                                                  
{
	mApp = app;
	mName = tr("Debugger");
	widget.reset(new DebuggerWidget);
	
    app->mainWindow()->utilityTabWidget()->addTab(widget.data(), name());

    connect(widget.data(), SIGNAL(debuggerStarted()), this, SLOT(startDebugging()));
    connect(widget.data(), SIGNAL(debuggerStopped()), this, SLOT(stopDebugging()));
    /*connect(widget, SIGNAL(readRequested()), this, SLOT(read()));
    connect(widget, SIGNAL(writeRequested(const QByteArray &)), this, SLOT(write(const QByteArray &)));*/


    return true;
}

bool DebuggerPlugin::startDebugging()
{
    // Recompile and upload the current program
    widget->setStatus(tr("Compile & Upload"));
    if(!mApp->mainWindow()->upload())
    {
	    widget->logError(tr("Compilation or Upload failed. Please take a look at the 'Output' tab."));
        widget->stopDebugging();
	    return false;
    }

    // Open the serial connection
    if(!openSerial())
        return false;

    return true;
}

void DebuggerPlugin::stopDebugging()
{
    // Close the serial connection
    if (serial.data() != NULL)
        serial->close();

    widget->setStatus(tr("Serial port closed."));
}

// Private slots
bool DebuggerPlugin::openSerial()
{
    // Close the connection
    if (serial.data() != NULL)
        serial->close();

    // Get the port
    QString port = mApp->settings()->devicePort();
    if (port.isEmpty())
    {
        widget->setStatus(tr("Unable to open, you must select a device first."));
        return false;
    }

    // Get the baudRate
    int baudRate = widget->baudRate();

    // Open the connexion
    serial.reset(new Serial(port, baudRate));
    if (! serial->open(QIODevice::ReadWrite))
    {
        widget->setStatus(tr("Open failed: %0").arg(serial->errorString()));
        return false;
    }

    // Set the status
    widget->setStatus(tr("Debugging serial port opened successfully at %1 bauds.").arg(widget->baudRate()));

    return true;
}

void DebuggerPlugin::closeSerial()
{
    if (serial.data() != NULL)
        serial->close();

    widget->setStatus(tr("Serial port closed."));
}

// Private
QByteArray DebuggerPlugin::readSerial(qint64 readCount)
{
    if (serial.data() != NULL && serial->isOpen())
    {
        QByteArray pData;
        pData.resize(readCount);
        readCount = serial->readData(pData.data(), readCount);
        if (readCount < 0)
            widget->setStatus(tr("Read error: %0").arg(serial->errorString()));
        else
        {
            widget->setStatus(tr("Read %0 bytes of data.").arg(readCount));
            pData.resize(readCount);

            return pData;
        }
    }
    else
        widget->setStatus(tr("Unable to read, the port is not opened."));

    return QByteArray();
}

bool DebuggerPlugin::writeSerial(const QByteArray &data)
{
    if (serial.data() != NULL && serial->isOpen())
        return serial->write(data) == data.size();
    else
        widget->setStatus(tr("Unable to write, the port is not opened."));

    return false;
}

Q_EXPORT_PLUGIN2(debugger, DebuggerPlugin)