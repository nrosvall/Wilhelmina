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
	void setInterval(int interval);
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

