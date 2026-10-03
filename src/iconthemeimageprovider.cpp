#include "iconthemeimageprovider.h"
#include <QFile>
#include <QIcon>
#include <QCache>

static QCache<QString, QPixmap> s_dockIconCache(150);

IconThemeImageProvider::IconThemeImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap IconThemeImageProvider::requestPixmap(const QString &id, QSize *realSize,
                                              const QSize &requestedSize)
{
    // Sanitize requested size: default to 128 if not specified
    QSize size(requestedSize);
    if (size.width() < 1)
        size.setWidth(128);
    if (size.height() < 1)
        size.setHeight(128);

    if (realSize)
        *realSize = size;

    QString cacheKey = QString("%1_%2x%3").arg(id).arg(size.width()).arg(size.height());
    if (s_dockIconCache.contains(cacheKey)) {
        return *s_dockIconCache.object(cacheKey);
    }

    QPixmap resultPixmap;

    if (QFile::exists(id) || id.startsWith(QLatin1Char('/'))) {
        if (id.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)
            || id.endsWith(QLatin1String(".svgz"), Qt::CaseInsensitive)) {
            QIcon svgIcon(id);
            if (!svgIcon.isNull())
                resultPixmap = svgIcon.pixmap(size);
        }
        if (resultPixmap.isNull()) {
            QPixmap pix(id);
            if (!pix.isNull())
                resultPixmap = pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }

    if (resultPixmap.isNull()) {
        QIcon icon = QIcon::fromTheme(id);
        if (icon.isNull()) {
            const QString cur = QIcon::themeName();
            if (cur != QLatin1String("Cutefish")) {
                QIcon::setThemeName(QLatin1String("Cutefish"));
                icon = QIcon::fromTheme(id);
                QIcon::setThemeName(cur);
            }
        }
        if (icon.isNull())
            icon = QIcon::fromTheme(QLatin1String("application-x-desktop"));

        resultPixmap = icon.pixmap(size);
    }

    if (!resultPixmap.isNull()) {
        s_dockIconCache.insert(cacheKey, new QPixmap(resultPixmap));
    }

    return resultPixmap;
}
