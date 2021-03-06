/**********************************************************************
 * Copyright 2015 Arto Jalkanen
 *
 * This file is part of RingingRestorer.
 *
 * RingingRestorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RingingRestorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RingingRestorer.  If not, see <http://www.gnu.org/licenses/>
**/
#include <QDebug>

#include "app/appmaininit.h"
#include "daemon/daemonmaininit.h"

MainInit *createMainInit(bool isDaemon) {
    if (isDaemon) return new DaemonMainInit;
    return new AppMainInit;
}

Q_DECL_EXPORT
int main(int argc, char *argv[])
{
    bool isDaemon = false;

    for (int i = 0; i < argc; ++i) {
        QString arg(argv[i]);

        if (arg == "-d") isDaemon = true;
    }

    qDebug() << Q_FUNC_INFO << "isDaemon" << isDaemon;

    QScopedPointer<MainInit> mainInit(createMainInit(isDaemon));

    return mainInit->main(argc, argv);
}

