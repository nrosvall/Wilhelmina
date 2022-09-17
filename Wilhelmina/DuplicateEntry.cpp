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

#include "DuplicateEntry.h"

DuplicateEntry::DuplicateEntry(QString title, QString ID, QString secondTitle, QString secondID) {
	m_title = title;
	m_id = ID;
	m_secondID = secondID;
	m_secondTitle = secondTitle;
}

QString DuplicateEntry::title() const {
	return m_title;
}

QString DuplicateEntry::ID() const {
	return m_id;
}

QString DuplicateEntry::secondTitle() const {
	return m_secondTitle;
}

QString DuplicateEntry::secondID() const {
	return m_secondID;
}