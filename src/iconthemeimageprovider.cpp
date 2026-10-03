#include "iconthemeimageprovider.h"
#include <QFile>
#include <QIcon>

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

    // Return real size
    if (realSize)
        *realSize = size;

    if (QFile::exists(id) || id.startsWith(QLatin1Char('/'))) {
        if (id.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)
            || id.endsWith(QLatin1String(".svgz"), Qt::CaseInsensitive)) {
            QIcon svgIcon(id);
            if (!svgIcon.isNull())
                return svgIcon.pixmap(size);
        }
        QPixmap pix(id);
        if (!pix.isNull())
            return pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // Return icon from theme or fallback to a generic icon
    QIcon icon = QIcon::fromTheme(id);
    if (icon.isNull()) {
        const QString cur = QIcon::themeName();
        if (cur != QLatin1String("Cutefish")) {
            QIcon::setThemeName(QLatin1String("Cutefish"));
            icon = QIcon::fromTheme(id);
            if (icon.isNull()) {
                QIcon::setThemeName(cur);
            }
        }
    }
    if (icon.isNull())
        icon = QIcon::fromTheme(QLatin1String("application-x-desktop"));

    return icon.pixmap(size);
}

