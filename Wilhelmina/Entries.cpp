#include "Entries.h"

Entries::Entries() {
	m_EntriesDoc = new QJsonDocument();
}

Entries::~Entries() {
	delete m_EntriesDoc;
}

void Entries::AddEntry(QString title, QString user, QString password, QString url, QString notes, QString ID) {
    QJsonObject jObj;
	jObj.insert("title", title);
	jObj.insert("user", user);
	jObj.insert("password", password);
	jObj.insert("url", url);
	jObj.insert("notes", notes);
	jObj.insert("ID", ID);

	m_EntryArray.push_back(jObj);
}

bool Entries::Encrypt(QString &password) {
	return true;
}

QJsonObject Entries::GetObject(QString ID) {
	for (const auto obj : m_EntryArray) {
		if (obj.toObject().value("ID") == ID)
			return obj.toObject();
	}
	return QJsonObject();
}
