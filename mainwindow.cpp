#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uiStopped();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected()
{
    qDebug() << "SecureUploader: Connected";
    uiStarted();
    ui->logText->append(QString("SecureUploader: Connected"));
    /*
    qDebug() << "SecureUploader: Creating SFTP channel...";

    m_channel = m_connection->createSftpChannel();

    if (m_channel) {
        connect(m_channel.data(), SIGNAL(initialized()),
                SLOT(onChannelInitialized()));
        connect(m_channel.data(), SIGNAL(initializationFailed(QString)),
                SLOT(onChannelError(QString)));
        connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId, QString)),
                SLOT(onOpfinished(QSsh::SftpJobId, QString)));

        m_channel->initialize();

    } else {
        qDebug() << "SecureUploader: Error null channel";
    }
    */
}

void MainWindow::onConnectionError(QSsh::SshError err)
{
    qDebug() << "SecureUploader: Connection error" << err;
    ui->logText->append(QString("SecureUploader: Connection error - %1").arg(err));
}

void MainWindow::onChannelInitialized()
{

}

void MainWindow::onChannelError(const QString &err)
{
    qDebug() << "SecureUploader: Error: " << err;
}

void MainWindow::onOpfinished(QSsh::SftpJobId job, const QString &error)
{

}

void MainWindow::onDataAvailable(const QString &message)
{
    ui->logText->append(QString("data: %1").arg(message));
}

void MainWindow::onRemoteOutputAvailable(const QString &output)
{
    ui->logText->append(QString("output: %1").arg(output));
}

void MainWindow::onRemoteErrorAvailable(const QString &error)
{
    ui->logText->append(QString("error: %1").arg(error));
}

void MainWindow::on_connectBtn_clicked()
{
    QSsh::SshConnectionParameters params;
    params.setHost(ui->textHost->text());
    params.setUserName(ui->textUserName->text());
    params.setPassword(ui->textPassword->text());
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
    params.timeout = 30;
    params.setPort(22);

    m_shell = new Shell(params, this);
    m_shell->run();

    connect(m_shell, SIGNAL(readyRemoteError(QString)), SLOT(onRemoteErrorAvailable(QString)));
    connect(m_shell, SIGNAL(readyRemoteOutput(QString)), SLOT(onRemoteOutputAvailable(QString)));
    connect(m_shell, SIGNAL(readyShellMessage(QString)), SLOT(onDataAvailable(QString)));
    connect(m_shell, SIGNAL(connected()), SLOT(onConnected()));
    /*
    m_connection = new QSsh::SshConnection(params, this); // TODO free this pointer!

    connect(m_connection, SIGNAL(dataAvailable(QString)), SLOT(onDataAvailable(QString)));
    connect(m_connection, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    qDebug() << "SecureUploader: Connecting to host" << ui->textHost->text();

    m_connection->connectToHost();
    */
}

void MainWindow::on_sendBtn_clicked()
{
    m_shell->onLocalMessage(ui->textSend->text() + tr("\n"));
}

void MainWindow::on_closeBtn_clicked()
{
    m_shell->stop();
    delete m_shell;
    uiStopped();
}

void MainWindow::uiStarted()
{
    ui->closeBtn->setEnabled(true);
    ui->connectBtn->setEnabled(false);

    ui->logText->setEnabled(true);
    ui->textSend->setEnabled(true);
    ui->sendBtn->setEnabled(true);

    ui->textHost->setEnabled(false);
    ui->textUserName->setEnabled(false);
    ui->textPassword->setEnabled(false);
}

void MainWindow::uiStopped()
{
    ui->closeBtn->setEnabled(false);
    ui->connectBtn->setEnabled(true);

    ui->logText->setEnabled(false);
    ui->textSend->setEnabled(false);
    ui->sendBtn->setEnabled(false);

    ui->textHost->setEnabled(true);
    ui->textUserName->setEnabled(true);
    ui->textPassword->setEnabled(true);
}
