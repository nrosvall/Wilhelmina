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

#include "IdleFilter.h"

IdleFilter::IdleFilter(QObject* parent, int interval) : QObject(parent)
{
    m_Interval = interval;
    m_Timer = new QTimer(this);
    m_Timer->start(m_Interval);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(Timeout()));
}

IdleFilter::~IdleFilter() {
    delete m_Timer;
}

int IdleFilter::Interval() {
    return m_Interval;
}

void IdleFilter::setClient(QMainWindow* client) {
    m_Client = client;
}

bool IdleFilter::eventFilter(QObject* obj, QEvent* ev)
{
    if (ev->type() != QEvent::Timer)
        m_Timer->start(m_Interval);

    return QObject::eventFilter(obj, ev);
}

void IdleFilter::Timeout()
{
    if (m_Client) {
        m_Client->showMinimized();
    }

    emit dummy_userInactive();
}
