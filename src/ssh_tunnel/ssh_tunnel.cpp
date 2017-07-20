//extern "C" {
//#include "ssh_common.c"
//}
#include "ssh_tunnel.h"
#include <QHostAddress>
#include <QTextStream>
//#include <signal.h>

#define buffSize  4096

SSH_Tunnel::SSH_Tunnel(QObject *parent) :
    QThread(parent)
{
    socketToViewerPort = new QTcpSocket(this);
    //socketToViewerPort->setSocketOption(
    //            QAbstractSocket::KeepAliveOption, true);
    server = new TCP_Server(this);
    connect( server, SIGNAL(readyRead(QTcpSocket*)),
             this, SLOT(write_to_remote_graphic_channel(QTcpSocket*)) );
    ssh_tunnel = new QProcess(this);
    connect(this, SIGNAL(finished()),
            this, SLOT(thread_finished()));
}
SSH_Tunnel::~SSH_Tunnel()
{
    QTextStream s(stdout);
    if ( ssh_tunnel!=nullptr ) {
        //if ( ssh_tunnel->isOpen() ) {
        //    ssh_tunnel->close();
        //    //ssh_tunnel->waitForFinished();
        //    s<< "ssh_tunnel is finished at thread deletion" << endl;
        //} else {
        //    ssh_tunnel->kill();
        //    s<< "ssh_tunnel is killed at thread deletion" << endl;
        //};
        //kill(pid, SIGKILL);
        ssh_tunnel->kill();
        delete ssh_tunnel;
        ssh_tunnel = nullptr;
    };
    server->disconnect();
    server->close();
    socketToViewerPort->disconnectFromHost();
    socketToViewerPort->waitForDisconnected();
    s<< "~SSH_Tunnel()" << endl;
}

void SSH_Tunnel::setData(QVariantMap _data)
{
    User = _data.value("User").toString();
    remoteHost = _data.value("RemoteHost").toString();
    remotePort = _data.value("RemotePort").toString();
    graphicsAddr = _data.value("GraphicsAddr").toString();
    graphicsPort = _data.value("GraphicsPort").toString();
}
void SSH_Tunnel::run()
{
    /*
    ssh_session session;
    ssh_channel channel;
    char buffer[256];
    int nbytes;
    int rc;

    session = connect_ssh("stationar", "root", 0);
    if (session == nullptr) {
        return;
    };

    channel = ssh_channel_new(session);
    if (channel == nullptr) {
        ssh_disconnect(session);
        ssh_free(session);
        return;
    };

    rc = ssh_channel_open_session(channel);
    if (rc < 0) {
        goto failed;
    };

    rc = ssh_channel_request_exec(channel, "lsof");
    if (rc < 0) {
        goto failed;
    };

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        if (fwrite(buffer, 1, nbytes, stdout) != (unsigned int) nbytes) {
            goto failed;
        };
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    };

    if (nbytes < 0) {
        goto failed;
    };

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_free(session);

    return;
failed:
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    return;
    */
    QTextStream s(stdout);
    quint16 viewerPort = 33333;
    bool connected = false;
    bool finished = false;
    QStringList _args;
    QString nc_command;
    /*
    while ( !connected) {
        bool bound = socketToViewerPort->bind(
                    QHostAddress("127.0.0.1"),
                    viewerPort,
                    QAbstractSocket::ShareAddress); // as listen service
        if ( !bound ) {
            viewerPort = 0;
            continue;
        };
        viewerPort = socketToViewerPort->localPort();
        socketToViewerPort->connectToHost(
                    "127.0.0.1",
                    viewerPort,
                    QIODevice::ReadWrite,
                    QAbstractSocket::IPv4Protocol);
        if ( !socketToViewerPort->waitForConnected() ) {
            viewerPort = 0;
            continue;
        };
        if ( !socketToViewerPort->open(QIODevice::ReadWrite) ) {
            viewerPort = 0;
            continue;
        } else {
            connected = true;
        };
    };
    if ( !connected ) {
        // emit error to Viewer
        emit errMsg("Socket to Viwer port not established");
        s<< "not connected" << endl;
        goto thread_exit;
    };
    // socket signal connections;
    connect(socketToViewerPort, SIGNAL(readyRead()),
            this, SLOT(write_to_remote_graphic_channel()));
    connect(socketToViewerPort, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(send_socket_errors(QAbstractSocket::SocketError)));
    */
    while ( !server->start_listen(viewerPort) ) {
        ++viewerPort;
        if (viewerPort>65536) break;
    };

    // create SSH tunnel
    ssh_tunnel->setProcessChannelMode(QProcess::SeparateChannels);
    connect(ssh_tunnel, SIGNAL(readyRead()),
            this, SLOT(write_to_viewer()));
    connect(ssh_tunnel, SIGNAL(finished(int)),
            this, SIGNAL(tunnel_finished()));
    connect(ssh_tunnel, SIGNAL(destroyed(QObject*)),
            this, SIGNAL(tunnel_finished()));
    connect(ssh_tunnel, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(resend_tunnel_errors(QProcess::ProcessError)));
    ///*
    nc_command = QString(
"'nc -q 2>&1 | grep \"requires an argument\" >/dev/null;\
if [ $? -eq 0 ] ; then\
    CMD=\"nc -q 0 %1 %2\";\
else\
    CMD=\"nc %1 %2\";\
fi;\
eval \"$CMD\";'").arg(graphicsAddr).arg(graphicsPort);
    _args   << "-p" << remotePort
            << "-l" << User << remoteHost
            << "sh" << "-c"
            << nc_command;
//*/

    /*
    while ( !connected ) {

        if ( !socketToViewerPort->bind(0) ) {
            continue;
        };
        viewerPort = socketToViewerPort->localPort();
        _args.clear();
        nc_command.clear();
        _args   << "-p" << remotePort
                << "-L"
                << QString("%1:127.0.0.1:%2").arg(viewerPort).arg(graphicsPort)
                << QString("%1@%2").arg(User).arg(remoteHost);
        ssh_tunnel->start("ssh", _args, QIODevice::ReadWrite);
        s << "conmmand args: "<< _args.join(" ") << endl;
        if ( !ssh_tunnel->waitForStarted() ) continue;
        if ( !ssh_tunnel->isOpen() ) {
            ssh_tunnel->kill();
            continue;
        };
        socketToViewerPort->close();
        pid = ssh_tunnel->pid();
        connected = true;
        break;
    };
    */

    s << "conmmand args: "<< _args.join(" ") << endl;
    ssh_tunnel->start("ssh", _args, QIODevice::ReadWrite);
    if ( !ssh_tunnel->waitForStarted() ) {
        s<< "remote redirect established: "<< "none" << endl;
        emit errMsg("SSH tunnel not established");
        goto thread_exit;
    };

    emit established(viewerPort);
    s<< "established: "<< viewerPort <<" State: "<<ssh_tunnel->state()
     << " PID: "<< pid<< endl;
    finished = ssh_tunnel->waitForFinished(-1);
    s<< "netcat is finished: "<< finished << endl;

thread_exit:
    if ( socketToViewerPort->isOpen() ) {
        socketToViewerPort->disconnectFromHost();
        socketToViewerPort->waitForDisconnected();
        s<< "socket is disconnected " << endl;
    };
    s<< "ssh tunnel thread is finished " << endl;
}

/* private slots */
void SSH_Tunnel::write_to_viewer()
{
    QTextStream s(stdout);
    quint64 written = 0;
    char buff[buffSize];
    quint64 bytes = ssh_tunnel->read(buff, buffSize);
    while ( 0<bytes ) {
        s<<buff;
        QTcpSocket *_sock = server->nextPendingConnection();
        while ( _sock!=nullptr ) {
            written += _sock->write(buff, bytes);
            _sock = server->nextPendingConnection();
        };
        bytes = ssh_tunnel->read(buff, buffSize);
    };
    s<<"\nwrite_to_viewer: "<<written<<endl;
}
void SSH_Tunnel::write_to_remote_graphic_channel(QTcpSocket *_sock)
{
    QTextStream s(stdout);
    quint64 written = 0;
    char buff[buffSize];
    quint64 bytes = _sock->read(buff, buffSize);
    while ( 0<bytes ) {
        s<<buff;
        written += ssh_tunnel->write(buff, bytes);
        bytes = _sock->read(buff, buffSize);
    };
    s<<"\nwrite_to_remote_graphic_channel: "<<written<<endl;
}
void SSH_Tunnel::resend_socket_errors(QAbstractSocket::SocketError _err)
{
    Q_UNUSED(_err);
    QTextStream s(stdout);
    s<<socketToViewerPort->errorString()<<endl;
    emit errMsg(socketToViewerPort->errorString());
}
void SSH_Tunnel::resend_tunnel_errors(QProcess::ProcessError _err)
{
    Q_UNUSED(_err);
    QTextStream s(stdout);
    s<<ssh_tunnel->errorString()<<endl;
    emit errMsg(ssh_tunnel->errorString());
}
void SSH_Tunnel::thread_finished()
{
    QTextStream s(stdout);
    s<<"thread_finished" <<endl;
}