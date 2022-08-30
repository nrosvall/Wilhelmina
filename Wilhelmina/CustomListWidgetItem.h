#pragma once

#include <qlistwidget.h>
#include <QListWidgetItem>

class CustomListWidgetItem : public QListWidgetItem
{
public:
	void setID(QString ID);
	QString getID();
	//CustomListWidgetItem();
	//~CustomListWidgetItem();

private:
	QString m_ID;
};
