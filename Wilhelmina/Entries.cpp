#include "Entries.h"
#include "Crypto.h"
#include <qfile.h>
#include <qdatastream.h>

Entries::Entries() {
	m_encryptedBlobFile = "entries.wil";
}

Entries::~Entries() {

}

QString Entries::encryptedBlobFile() {
	return m_encryptedBlobFile;
}

void Entries::AddEntry(QString &title, QString &user, QString &password, QString &url, QString &notes, QString &ID) {
    QJsonObject jObj;
	jObj.insert("title", title);
	jObj.insert("user", user);
	jObj.insert("password", password);
	jObj.insert("url", url);
	jObj.insert("notes", notes);
	jObj.insert("ID", ID);

	m_EntryArray.push_back(jObj);
}

bool Entries::Encrypt(QString &master_passphrase, QString &dataPath) {

	Crypto crypto;
	Key key;
	bool keyOk;
	key = crypto.generate_key(master_passphrase.toLocal8Bit(), nullptr, &keyOk);
	size_t ret = -1;

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

			plainBytes.clear();
			m_rootEntry = QJsonObject();
			m_EntriesDoc = QJsonDocument();

			for (int i = 0; i < m_EntryArray.count(); i++) {
				m_EntryArray.removeAt(0);
			}

			m_EntryArray = QJsonArray();

			QFile file(dataPath + m_encryptedBlobFile);
			file.open(QFile::WriteOnly | QFile::Truncate ); //TODO: error check
			
			file.write(iv.constData(), crypto.ivSize());
			file.write(reinterpret_cast<const char*>(key.salt), crypto.saltSize());
			file.write(tag.constData(), crypto.tagSize());
			file.write((const char *)(cipher), ret);
			file.close();

		}
		
	}

	return ret > 0;
}

bool Entries::Decrypt(QString& master_passphrase, QString &dataPath) {

	bool keyOk;
	size_t ret = -1;
	QFile file(dataPath + m_encryptedBlobFile);
	file.open(QFile::ReadOnly);
	
	Crypto crypto;

	QByteArray iv = file.read(crypto.ivSize());
	QByteArray salt = file.read(crypto.saltSize());
	QByteArray tag = file.read(crypto.tagSize());
	QByteArray cipher = file.readAll();

	file.close();

	Key key;

	key = crypto.generate_key(master_passphrase.toLocal8Bit(), salt.data(), &keyOk);
	
	QByteArray plainData(cipher.length(), 'p');

	ret = crypto.decryptData((unsigned char*)cipher.data(), cipher.length(), (unsigned char*)tag.data(), (unsigned char*)key.data,
		               (unsigned char*)iv.data(), crypto.ivSize(), (unsigned char*)plainData.data());

	if (ret > 0) {
		m_EntriesDoc = QJsonDocument::fromJson(plainData);
		m_EntryArray = m_EntriesDoc.array();
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

void Entries::deleteItem(QString ID) {

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
