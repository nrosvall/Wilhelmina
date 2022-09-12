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

#include "Wilhelmina.h"
#include <QtWidgets/QApplication>
#include "IdleFilter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Niko Rosvall");
    QCoreApplication::setOrganizationDomain("byteptr.com");
    QCoreApplication::setApplicationName("Wilhelmina");
    QCoreApplication::setApplicationVersion("1.0");

    Wilhelmina w;

    IdleFilter* idleFilter = new IdleFilter(&a, w.Settings.value("LockInterval", 5).toInt() * 60000);
    idleFilter->setClient(&w);

    a.installEventFilter(idleFilter);
    a.installEventFilter(&w);

    w.setIdleFilter(idleFilter);

    QObject::connect(idleFilter, SIGNAL(userInactive()),
        &w, nullptr);

    w.show();

    return a.exec();
}
