#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sftpchannel.h"
#include "sshconnection.h"

#include "shell.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onConnected();
    void onConnectionError(QSsh::SshError);
    void onChannelInitialized();
    void onChannelError(const QString &err);
    void onOpfinished(QSsh::SftpJobId job, const QString & error = QString());

    void onDataAvailable(const QString &message);
    void onRemoteOutputAvailable(const QString &output);
    void onRemoteErrorAvailable(const QString &error);

    void on_connectBtn_clicked();
    void on_sendBtn_clicked();
    void on_closeBtn_clicked();

private:
    void uiStarted();
    void uiStopped();

    Ui::MainWindow *ui;

    QString m_localFilename;
    QString m_remoteFilename;
    Shell * m_shell;
    QSsh::SftpChannel::Ptr m_channel;
    QSsh::SshConnection *m_connection;
};

#endif // MAINWINDOW_H
