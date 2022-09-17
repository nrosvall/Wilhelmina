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

#include "Entries.h"
#include "Crypto.h"
#include <qfile.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <quuid.h>
#include "SSHsync.h"

Entries::Entries() {
	m_encryptedBlobFile = "entries.wil";
}

Entries::~Entries() {

}

QString Entries::encryptedBlobFile() {
	return m_encryptedBlobFile;
}

void Entries::AddEntry(QString const &title, QString const &user, QString const &password, QString const &url, QString const &notes, bool pinned, QString const &ID) {
    QJsonObject jObj;
	jObj.insert("title", title);
	jObj.insert("user", user);
	jObj.insert("password", password);
	jObj.insert("url", url);
	jObj.insert("notes", notes);
	jObj.insert("pinned", pinned);
	jObj.insert("ID", ID);

	m_EntryArray.push_back(jObj);
}

bool Entries::Encrypt(QWidget* parentWindow, QStatusBar *statusBar, QSettings *settings, QString &master_passphrase, QString &dataPath, bool dataClearing) {

	Crypto crypto;
	Key key;
	bool keyOk;
	key = crypto.generate_key(master_passphrase.toLocal8Bit(), nullptr, &keyOk);
	int ret = -1;

	if (keyOk) {
		m_EntriesDoc.setArray(m_EntryArray);
		QByteArray plainBytes = m_EntriesDoc.toJson();

		QByteArray cipher(plainBytes.length(), 'c');
		QByteArray tag(crypto.tagSize(), 't');
		QByteArray iv(crypto.ivSize(), 'i');

		crypto.genData((unsigned char*)iv.data(), crypto.ivSize());
		
		ret = crypto.encryptData((unsigned char*)plainBytes.data(), plainBytes.length(), (unsigned char*)iv.data(), (unsigned char*)key.data, 
								(unsigned char*)cipher.data(), (unsigned char*)tag.data());

		if (ret > 0) {

			if (dataClearing) {
				plainBytes.clear();
				m_rootEntry = QJsonObject();
				m_EntriesDoc = QJsonDocument();

				for (int i = 0; i < m_EntryArray.count(); i++) {
					m_EntryArray.removeAt(0);
				}

				m_EntryArray = QJsonArray();
			}

			QString fullDataPath = dataPath + m_encryptedBlobFile;

			QFile file(fullDataPath);
			if (file.open(QFile::WriteOnly | QFile::Truncate)) {
				file.write(iv.constData(), crypto.ivSize());
				file.write(reinterpret_cast<const char*>(key.salt), crypto.saltSize());
				file.write(tag.constData(), crypto.tagSize());
				file.write((const char*)(cipher), ret);
				file.close();

				if (settings->value("SSHenabled").toBool()) {
					statusBar->showMessage("Please wait...Syncing...");
					SSHsync sync(settings, parentWindow);
					if (!sync.toRemote(fullDataPath))
						QMessageBox::critical(parentWindow, "Wilhelmina", sync.lastErrorMessage(), QMessageBox::Ok);
					statusBar->showMessage("Sync Ready");
				}
			}
			else {
				return false;
			}
		}
	}

	return ret > 0;
}

bool Entries::Decrypt(QString& master_passphrase, QString &dataPath) {

	bool keyOk;
	int ret = -1;
	QFile file(dataPath + m_encryptedBlobFile);

	if (!file.open(QFile::ReadOnly)) {
		return false;
	}
	
	Crypto crypto;

	QByteArray iv = file.read(crypto.ivSize());
	QByteArray salt = file.read(crypto.saltSize());
	QByteArray tag = file.read(crypto.tagSize());
	QByteArray cipher = file.readAll();

	file.close();

	Key key;

	key = crypto.generate_key(master_passphrase.toLocal8Bit(), salt.data(), &keyOk);

	if (!keyOk)
		return false;
	
	QByteArray plainData(cipher.length(), 'p');

	ret = crypto.decryptData((unsigned char*)cipher.data(), cipher.length(), (unsigned char*)tag.data(), (unsigned char*)key.data,
		               (unsigned char*)iv.data(), crypto.ivSize(), (unsigned char*)plainData.data());

	if (ret > 0) {
		m_EntriesDoc = QJsonDocument::fromJson(plainData);
		m_EntryArray = m_EntriesDoc.array();
	}

	return ret > 0;
}

//Create a copy of the array and insert missing ID values, replace original array with the copy
void Entries::setDocumentFromImportData(QJsonDocument& doc) {
	m_EntriesDoc = doc;
	m_EntryArray = m_EntriesDoc.array();
	QJsonArray tmpArr;
	QJsonObject newObj;

	for (auto obj : m_EntryArray) {
		newObj = obj.toObject();
		if(obj.toObject().value("ID").isUndefined()) {
			QString ID = QUuid::createUuid().toString();
			newObj.insert("ID", ID);
		}
		tmpArr.push_back(newObj);
	}
	
	m_EntryArray = tmpArr;
}

QJsonObject Entries::GetJObject(QString ID) {
	for (const auto obj : m_EntryArray) {
		
		if (obj.toObject().value("ID") == ID)
			return obj.toObject();
	}
	return QJsonObject();
}

void Entries::deleteEntry(QString ID) {

	QJsonObject tmpObj;

	for (int i = 0; m_EntryArray.count(); i++) {
		tmpObj = m_EntryArray.at(i).toObject();
		if (tmpObj.value("ID") == ID) {
			m_EntryArray.removeAt(i);
			break;
		}
	}
}

QJsonArray& Entries::entryArray() {
	return m_EntryArray;
}

QString Entries::getJson() {
	QJsonDocument doc;
	doc.setArray(m_EntryArray);

	QString json = doc.toJson();

	return json;
}
