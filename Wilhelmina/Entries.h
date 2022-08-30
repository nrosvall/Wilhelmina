#pragma once

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>

class Entries
{
private:
	QJsonDocument* m_EntriesDoc;
	QJsonObject m_rootEntry;
	QJsonArray m_EntryArray;
public:
	void AddEntry(QString title, QString user, QString password, QString url, QString notes, QString ID);
	bool Encrypt(QString &password);
	QJsonObject GetObject(QString ID);
	Entries();
	~Entries();

};

