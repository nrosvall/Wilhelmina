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
#include <qbytearray.h>
#include <QAbstractNativeEventFilter>
#include "CryptoState.h"
#include <qmainwindow.h>

class MyNativeEventFilter : public QAbstractNativeEventFilter
{
public:
	MyNativeEventFilter(CryptoState* cryptoState, QMainWindow* client);
	bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr*);

private:
	CryptoState* m_cryptoState;
	QMainWindow* m_Client;
};

