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

#pragma once
#include <qsettings.h>
#include <qstring.h>
#include <libssh/libssh.h>
#include <qmainwindow.h>

class SSHsync
{
public:
	SSHsync(QSettings* settings, QMainWindow* parentWindow);
	bool toRemote(QString& FullDataFilepath);
	bool fromRemote(QString& fullDataFilepath);
	QString lastErrorMessage();
private:
	QString m_LastErrorMessage;
	ssh_session initSession();
	bool verifySession(ssh_session session);
	QMainWindow* m_parentWindow;
	QString getRemoteFilename(QString& fullDataFilePath);

private:
	QSettings* m_Settings;
};

