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

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qsettings.h>
#include <qwidget.h>
#include <qstatusbar.h>

class Entries
{
private:
	QJsonDocument m_EntriesDoc;
	QJsonObject m_rootEntry;
	QJsonArray m_EntryArray;

public:
	void AddEntry(QString const & title, QString const & user, QString& password, QString const & url, QString const & notes, bool pinned, QString const & ID);
	bool Encrypt(QWidget* parentWindow, QStatusBar* statusBar, QSettings* settings, wchar_t *master_passphrase, QString& dataPath, bool dataClearing);
	bool Decrypt(wchar_t* master_passphrase, QString& dataPath);
	QJsonObject GetJObject(QString ID);
	static QString encryptedBlobFile();
	QJsonArray& entryArray();
	void deleteEntry(QString ID);
	QString getJson();
	void setDocumentFromImportData(QJsonDocument& doc);

};

