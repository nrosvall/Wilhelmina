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

SSHsync::SSHsync(QSettings* settings) {
	m_Settings = settings;
}

QString SSHsync::lastErrorMessage() {
	return m_LastErrorMessage;
}

bool SSHsync::verifySession(ssh_session session) {


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