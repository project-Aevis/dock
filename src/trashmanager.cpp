/*
 * Copyright (C) 2021 CutefishOS Team.
 * Copyright (C) 2026 Project Aevis Contributors.
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

#include "trashmanager.h"

#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QUrl>
#include <QStandardPaths>

static QString trashDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/Trash");
}

static const QDir::Filters ItemsShouldCount = QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot;

TrashManager::TrashManager(QObject *parent)
    : QObject(parent),
      m_filesWatcher(new QFileSystemWatcher(this)),
      m_count(0)
{
    const QString base = trashDir();
    m_filesWatcher->addPath(base);
    const QString files = base + QLatin1String("/files");
    if (QDir(files).exists()) {
        m_filesWatcher->addPath(files);
    }

    onDirectoryChanged();
    connect(m_filesWatcher, &QFileSystemWatcher::directoryChanged, this, &TrashManager::onDirectoryChanged, Qt::QueuedConnection);
}

void TrashManager::moveToTrash(const QList<QUrl> &urls)
{
    QStringList paths;
    paths.reserve(urls.size());

    for (const QUrl &url : urls) {
        if (!url.isLocalFile())
            continue;
        paths.append(url.toLocalFile());
    }

    const QString fileManager = QStandardPaths::findExecutable(QStringLiteral("aevis-filemanager")).isEmpty()
        ? QStringLiteral("cutefish-filemanager")
        : QStringLiteral("aevis-filemanager");
    QProcess::startDetached(fileManager, QStringList() << QStringLiteral("--move-to-trash") << paths);
}

void TrashManager::emptyTrash()
{
    const QString fileManager = QStandardPaths::findExecutable(QStringLiteral("aevis-filemanager")).isEmpty()
        ? QStringLiteral("cutefish-filemanager")
        : QStringLiteral("aevis-filemanager");
    QProcess::startDetached(fileManager, QStringList() << QStringLiteral("-e"));
}

void TrashManager::openTrash()
{
    const QString fileManager = QStandardPaths::findExecutable(QStringLiteral("aevis-filemanager")).isEmpty()
        ? QStringLiteral("cutefish-filemanager")
        : QStringLiteral("aevis-filemanager");
    QProcess::startDetached(fileManager, QStringList() << QStringLiteral("trash:///"));
}

void TrashManager::onDirectoryChanged()
{
    const QString filesPath = trashDir() + QLatin1String("/files");
    const QDir files(filesPath);

    if (files.exists()) {
        if (!m_filesWatcher->directories().contains(filesPath)) {
            m_filesWatcher->addPath(filesPath);
        }
        m_count = files.entryList(ItemsShouldCount).count();
    } else {
        m_count = 0;
    }

    emit countChanged();
}
