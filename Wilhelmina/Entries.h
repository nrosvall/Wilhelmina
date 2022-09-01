#pragma once

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>

class Entries
{
private:
	QJsonDocument m_EntriesDoc;
	QJsonObject m_rootEntry;
	QJsonArray m_EntryArray;
	QString m_encryptedBlobFile;

public:
	void AddEntry(QString& title, QString& user, QString& password, QString& url, QString& notes, QString& ID);
	bool Encrypt(QString &master_passphrase, QString& dataPath);
	bool Decrypt(QString &master_passphrase, QString& dataPath);
	QJsonObject GetJObject(QString ID);
	QString encryptedBlobFile();
	QJsonArray& entryArray();
	void deleteItem(QString ID);
	Entries();
	~Entries();

};

