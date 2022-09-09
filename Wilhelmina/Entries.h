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
#include <qmainwindow.h>

class Entries
{
private:
	QJsonDocument m_EntriesDoc;
	QJsonObject m_rootEntry;
	QJsonArray m_EntryArray;
	QString m_encryptedBlobFile;

public:
	void AddEntry(QString& title, QString& user, QString& password, QString& url, QString& notes, QString& ID);
	bool Encrypt(QMainWindow* parentWindow, QSettings* settings, QString &master_passphrase, QString& dataPath, bool dataClearing);
	bool Decrypt(QString &master_passphrase, QString& dataPath);
	QJsonObject GetJObject(QString ID);
	QString encryptedBlobFile();
	QJsonArray& entryArray();
	void deleteItem(QString ID);
	QString getJson();
	void setDocument(QJsonDocument& doc);
	Entries();
	~Entries();

};

