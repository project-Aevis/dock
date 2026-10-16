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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QTranslator>
#include <QLocale>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QPixmapCache>
#include <QTimer>
#include <QFile>

#if defined(__linux__)
#include <malloc.h>
#endif

#include "applicationmodel.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if defined(__linux__)
    mallopt(M_ARENA_MAX, 1);
    mallopt(M_TRIM_THRESHOLD, 64 * 1024);
    mallopt(M_MMAP_THRESHOLD, 32 * 1024);
#endif

    if (!QFile::exists(QStringLiteral("/dev/nvidia0")) &&
        QFile::exists(QStringLiteral("/usr/share/glvnd/egl_vendor.d/50_mesa.json"))) {
        qputenv("__EGL_VENDOR_LIBRARY_FILENAMES", "/usr/share/glvnd/egl_vendor.d/50_mesa.json");
    }

    if (qEnvironmentVariableIsEmpty("QSG_RENDER_LOOP")) {
        qputenv("QSG_RENDER_LOOP", "basic");
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("aevis-dock"));
    app.setDesktopFileName(QStringLiteral("aevis-dock"));
    app.setOrganizationName(QStringLiteral("aevis"));
    QPixmapCache::setCacheLimit(2048);

#if defined(__linux__)
    QTimer *trimTimer = new QTimer(&app);
    QObject::connect(trimTimer, &QTimer::timeout, []() {
        malloc_trim(0);
    });
    trimTimer->start(30000);
    QTimer::singleShot(2000, []() {
        malloc_trim(0);
    });
#endif

    QDBusConnection::sessionBus().registerService(QStringLiteral("org.aevis.Dock"));
    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("com.cutefish.Dock")) &&
        !QDBusConnection::sessionBus().interface()->isServiceRegistered(QStringLiteral("org.aevis.Dock"))) {
        return -1;
    }

    qmlRegisterType<DockSettings>("Aevis.Dock", 1, 0, "DockSettings");
    qmlRegisterType<DockSettings>("Cutefish.Dock", 1, 0, "DockSettings");

    QString qmFilePath = QString("%1/%2.qm").arg("/usr/share/aevis-dock/translations/").arg(QLocale::system().name());
    if (!QFile::exists(qmFilePath)) {
        qmFilePath = QString("%1/%2.qm").arg("/usr/share/cutefish-dock/translations/").arg(QLocale::system().name());
    }
    if (QFile::exists(qmFilePath)) {
        QTranslator *translator = new QTranslator(QApplication::instance());
        if (translator->load(qmFilePath)) {
            QGuiApplication::installTranslator(translator);
        } else {
            translator->deleteLater();
        }
    }

    MainWindow w;

    if (!QDBusConnection::sessionBus().registerObject("/Dock", &w)) {
        return -1;
    }

    return app.exec();
}
