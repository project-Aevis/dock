/*
 * Copyright (C) 2021 CutefishOS Team.
 * Copyright (C) 2026 Project Aevis Contributors.
 *
 * Author:     rekols <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "processprovider.h"
#include <QProcess>

ProcessProvider::ProcessProvider(QObject *parent)
    : QObject(parent)
{
}

bool ProcessProvider::startDetached(const QString &exec, QStringList args)
{
    QString aevisExec = exec;
    if (aevisExec.startsWith(QStringLiteral("cutefish-"))) {
        aevisExec.replace(0, 9, QStringLiteral("aevis-"));
        if (QProcess::startDetached(aevisExec, args))
            return true;
    }

    if (QProcess::startDetached(exec, args))
        return true;

    if (exec == QStringLiteral("cutefish-settings") || aevisExec == QStringLiteral("aevis-settings")) {
        if (QProcess::startDetached(QStringLiteral("/usr/bin/aevis-settings"), args))
            return true;
        if (QProcess::startDetached(QStringLiteral("/usr/bin/cutefish-settings"), args))
            return true;
    }
    return false;
}
