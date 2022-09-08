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

SSHsync::SSHsync(QSettings* settings, QMainWindow *parentWindow) {
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
    
    if(hlen < 0)
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
    }
        break;

    case SSH_SERVER_ERROR:
        QString err(ssh_get_error(session));
        m_LastErrorMessage = err;
        free(hash);
        return false;
    }

    free(hash);

	return true;
}

bool SSHsync::toRemote() {
	
	sftp_session sftp;
	ssh_session session;
	ssh_key pubkey = NULL;
	ssh_channel channel;

	session = ssh_new();

	if (!session)
		return false;

	int port = 22; //TODO: get from settings;

	ssh_options_set(session, SSH_OPTIONS_HOST, "localhost"); //TODO: get server from settings
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
	ssh_options_set(session, SSH_OPTIONS_USER, "root"); //TODO: get server from settings

	int rc = 0;

	rc = ssh_connect(session);

	if (rc != SSH_OK) {
		ssh_free(session);
		QString err(ssh_get_error(session));
		m_LastErrorMessage = "Connection failed: " + err;
		return false;
	}

	if (!verifySession(session)) {
		ssh_disconnect(session);
		ssh_free(session);
		return false;
	}

	sftp = sftp_new(nullptr);

	ssh_disconnect(session);
	ssh_free(session);

	return true;
}

bool SSHsync::fromRemote() {

	return true;
}