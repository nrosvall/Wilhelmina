#pragma once
#include <QtGui>
#include <QtCore>
#include <QtWidgets/QMainWindow>

class IdleFilter : public QObject
{
	Q_OBJECT
public:
	IdleFilter(QObject* parent, int interval);
	~IdleFilter();
	int Interval();
	void setClient(QMainWindow* client);
signals:
	void dummy_userInactive();
public slots:
	void Timeout();
protected:
	bool eventFilter(QObject* obj, QEvent* ev);
private:
	QTimer *m_Timer;
	int m_Interval;
	QMainWindow* m_Client = nullptr;
};

