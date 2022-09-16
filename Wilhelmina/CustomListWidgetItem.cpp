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

#include "CustomListWidgetItem.h"

void CustomListWidgetItem::setID(QString ID) {
	m_ID = ID;
}

QString CustomListWidgetItem::getID() {
	return m_ID;
}

void CustomListWidgetItem::setPinned(bool value) {
	m_Pinned = value;

	if (value) {
		setIcon(QIcon(":/wilhelmina/icons/pin.png"));
	}
	else {
		setIcon(QIcon(":/wilhelmina/icons/nopin.png"));
	}
}

bool CustomListWidgetItem::pinned() {
	return m_Pinned;
}
