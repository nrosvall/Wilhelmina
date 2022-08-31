#include "Entries.h"
#include "Crypto.h"

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

bool Entries::Encrypt(QString &master_passphrase) {
	Crypto crypto;
	Key key;
	bool keyOk;
	key = crypto.generate_key(master_passphrase.toLocal8Bit(), nullptr, &keyOk);
	int ret = -1;

	if (keyOk) {
		m_rootEntry.insert("Entries", m_EntryArray);
		m_EntriesDoc->setObject(m_rootEntry);
		QByteArray plainBytes = m_EntriesDoc->toJson();

		unsigned char* cipher = (unsigned char*)malloc(plainBytes.length());
		unsigned char* tag = (unsigned char*)malloc(16 * sizeof(char));

		ret = crypto.encryptData((unsigned char*)plainBytes.data(), plainBytes.length(), (unsigned char*)key.data, cipher, tag);

		//After encryption clear plainbytes and the key
	}

	return ret > 0;
}

QJsonObject Entries::GetJObject(QString ID) {
	for (const auto obj : m_EntryArray) {
		if (obj.toObject().value("ID") == ID)
			return obj.toObject();
	}
	return QJsonObject();
}
