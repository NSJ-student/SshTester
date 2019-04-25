/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "shell.h"

#include <sshconnection.h>
#include <sshremoteprocess.h>

#include <QCoreApplication>
#include <QFile>
#include <QSocketNotifier>

#include <cstdlib>
#include <iostream>

using namespace QSsh;

Shell::Shell(const SshConnectionParameters &parameters, QObject *parent)
    : QObject(parent),
      m_connection(new SshConnection(parameters)),
      m_shell(Q_NULLPTR)
{
    connect(m_connection, &SshConnection::connected, this, &Shell::handleConnected);
    connect(m_connection, &SshConnection::dataAvailable, this, &Shell::handleShellMessage);
    connect(m_connection, &SshConnection::error, this, &Shell::handleConnectionError);
}

Shell::~Shell()
{
    delete m_connection;
}

void Shell::run()
{
    m_connection->connectToHost();
}

void Shell::stop()
{
    if(m_shell != Q_NULLPTR)
    {
        m_shell->close();
    }
}

void Shell::handleConnectionError()
{
    qDebug() << "SSH connection error: " << qPrintable(m_connection->errorString());
    QCoreApplication::exit(EXIT_FAILURE);
}

void Shell::handleShellMessage(const QString &message)
{
    emit readyShellMessage(message);
}

void Shell::handleConnected()
{
    m_shell = m_connection->createRemoteShell();
    connect(m_shell.data(), &SshRemoteProcess::started, this, &Shell::handleShellStarted);
    connect(m_shell.data(), &SshRemoteProcess::readyReadStandardOutput,
            this, &Shell::handleRemoteStdout);
    connect(m_shell.data(), &SshRemoteProcess::readyReadStandardError,
            this, &Shell::handleRemoteStderr);
    connect(m_shell.data(), &SshRemoteProcess::closed, this, &Shell::handleChannelClosed);
    m_shell->start();

    emit connected();
}

void Shell::handleShellStarted()
{
    qDebug() << "shell started";
}

void Shell::handleRemoteStdout()
{
    QString output(m_shell->readAllStandardOutput());
    emit readyRemoteOutput(output);
}

void Shell::handleRemoteStderr()
{
    QString error(m_shell->readAllStandardError());
    emit readyRemoteError(error);
}

void Shell::handleChannelClosed(int exitStatus)
{
    qDebug() << "Shell closed. Exit status was " << exitStatus << ", exit code was "
             << m_shell->exitCode() << ".";
    QCoreApplication::exit(exitStatus == SshRemoteProcess::NormalExit && m_shell->exitCode() == 0
        ? EXIT_SUCCESS : EXIT_FAILURE);
}

void Shell::onLocalMessage(const QString &message)
{
    m_shell->write(message.toLatin1());
}
