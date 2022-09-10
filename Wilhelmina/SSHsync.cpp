/*
* Wilhelmina
*
* Copyright (C) 2022  Niko Rosvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SSHsync.h"
#include <libssh/sftp.h>
#include <qmessagebox.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <qfile.h>
#include <qbytearray.h>
#include <qfileinfo.h>
#include <qdir.h>

SSHsync::SSHsync(QSettings* settings, QMainWindow* parentWindow) {
	m_Settings = settings;
	m_parentWindow = parentWindow;
}

QString SSHsync::lastErrorMessage() {
	return m_LastErrorMessage;
}

bool SSHsync::verifySession(ssh_session session) {

	int state, hlen;
	unsigned char* hash = NULL;
	char* hexa;
	state = ssh_is_server_known(session);
	hlen = ssh_get_pubkey_hash(session, &hash);

	if (hlen < 0)
		return false;

	switch (state) {
	case SSH_SERVER_KNOWN_OK:
		break; //Everything OK

	case SSH_SERVER_KNOWN_CHANGED:
		m_LastErrorMessage = "Host key for server changed. For security reasons,\n connection will be stopped.";
		free(hash);
		return false;

	case SSH_SERVER_FOUND_OTHER:
		m_LastErrorMessage = "The host key for this server was not found but an other"
			"type of key exists.\n An attacker might change the default server key to "
			"confuse your client into thinking the key does not exist. Aborting.";
		free(hash);
		return false;

	case SSH_SERVER_FILE_NOT_FOUND:
		//fall to SSH_SERVER_NOT_KNOWN behavior
		[[fallthrough]];

	case SSH_SERVER_NOT_KNOWN: {
		hexa = ssh_get_hexa(hash, hlen);
		QString hex(hexa);
		free(hexa);

		QMessageBox msgBox(m_parentWindow);
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setText("The server is unknown. Do you trust the host key?");
		msgBox.setInformativeText("Public key hash: " + hex);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Yes);

		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes) {
			if (ssh_write_knownhost(session) < 0) {
				QString err(strerror(errno));
				m_LastErrorMessage = err;
				free(hash);
				return false;
			}
		}

		break;
	}
	case SSH_SERVER_ERROR:
		QString err(ssh_get_error(session));
		m_LastErrorMessage = err;
		free(hash);
		return false;
	}

	free(hash);

	return true;
}

ssh_session SSHsync::initSession() {

	ssh_session session;

	session = ssh_new();

	if (!session)
		return nullptr;

	int port = m_Settings->value("SSHport").toInt();

	ssh_options_set(session, SSH_OPTIONS_HOST, m_Settings->value("SSHserver").toString().toLocal8Bit());
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);

	int rc = 0;

	rc = ssh_connect(session);

	if (rc != SSH_OK) {
		ssh_free(session);
		m_LastErrorMessage = "SSH connection failed, check preferences?";
		return nullptr;
	}

	if (!verifySession(session)) {
		ssh_disconnect(session);
		ssh_free(session);
		return nullptr;
	}

	rc = ssh_userauth_publickey_auto(session, m_Settings->value("SSHuser").toString().toLocal8Bit(), nullptr);

	if (rc != SSH_AUTH_SUCCESS) {
		m_LastErrorMessage = "SSH authentication failed";
		ssh_disconnect(session);
		ssh_free(session);
		return nullptr;
	}

	return session;
}

QString SSHsync::makeUniqueRemoteFilename(QString& fullDataFilePath) {
	QFileInfo fInfo(fullDataFilePath);
	QString identifier = fInfo.dir().dirName();

	return "." + identifier + "_wilhelmina.sync";
}

bool SSHsync::toRemote(QString& fullDataFilepath) {

	sftp_session sftp;
	ssh_session session = nullptr;

	int access_type = O_WRONLY | O_CREAT | O_TRUNC;
	sftp_file file;
	int rc, count_written;

	session = initSession();

	if (session == nullptr)
		return false;

	sftp = sftp_new(session);
	rc = sftp_init(sftp);

	if (rc != SSH_OK) {
		m_LastErrorMessage = "SFTP initialization failed.";
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		return false;
	}

	QString remoteFilename = makeUniqueRemoteFilename(fullDataFilepath);
	file = sftp_open(sftp, remoteFilename.toLocal8Bit(), access_type, 0644);

	QFile dataFile(fullDataFilepath);

	if (!dataFile.open(QFile::ReadOnly)) {
		sftp_close(file);
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		m_LastErrorMessage = "Unable to open " + fullDataFilepath;
		return false;
	}

	QByteArray data = dataFile.readAll();
	dataFile.close();

	count_written = sftp_write(file, data.constData(), data.length());

	if (count_written != data.length()) {
		m_LastErrorMessage = "Copying file to sftp server failed.";
		sftp_close(file);
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		return false;
	}

	rc = sftp_close(file);

	if (rc != SSH_OK) {
		m_LastErrorMessage = "Unable to close remote file.";
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		return false;
	}

	sftp_free(sftp);
	ssh_disconnect(session);
	ssh_free(session);

	return true;
}

bool SSHsync::fromRemote(QString &fullDataFilepath) {

	sftp_session sftp;
	ssh_session session = nullptr;

	int access_type = O_RDONLY;
	sftp_file file;
	int rc;

	session = initSession();

	if (session == nullptr)
		return false;

	sftp = sftp_new(session);
	rc = sftp_init(sftp);

	if (rc != SSH_OK) {
		m_LastErrorMessage = "SFTP initialization failed.";
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		return false;
	}

	QString remoteFilename = makeUniqueRemoteFilename(fullDataFilepath);
	file = sftp_open(sftp, remoteFilename.toLocal8Bit(), access_type, 0);

	if (file == nullptr) {
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		m_LastErrorMessage = "Unable to open remote file: " + remoteFilename;
		return false;
	}

	QFile localFile(fullDataFilepath);

	if(!localFile.open(QFile::WriteOnly | QFile::Truncate)) {
		sftp_close(file);
		sftp_free(sftp);
		ssh_disconnect(session);
		ssh_free(session);
		m_LastErrorMessage = "Unable to open file: " + fullDataFilepath;
		return false;
	}

	int count_bytes, count_written;
	QByteArray buffer(16384, 'b');
	//char *buffer = new char[16384];

	for (;;) {
		count_bytes = sftp_read(file, buffer.data(), buffer.length());
		if (count_bytes == 0) {
			break; // EOF
		}
		else if (count_bytes < 0) {
			m_LastErrorMessage = "Error while reading remote file.";
			sftp_close(file);
			sftp_free(sftp);
			ssh_disconnect(session);
			ssh_free(session);
			localFile.close();
			return false;
		}

		count_written = localFile.write(buffer, count_bytes);

		if (count_written != count_bytes) {
			m_LastErrorMessage = "Error writing to " + fullDataFilepath + ".";
			sftp_close(file);
			sftp_free(sftp);
			ssh_disconnect(session);
			ssh_free(session);
			localFile.close();
			return false;
		}
	}

	sftp_close(file);
	sftp_free(sftp);
	ssh_disconnect(session);
	ssh_free(session);
	localFile.close();

	return true;
}