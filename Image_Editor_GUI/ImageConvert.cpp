#include <QPainter>

#include "ImageConvert.h"

#include <cstdlib>
#include <cstring>

namespace ImageConvert {

QImage wrap(const Image &image)
{
    if (!image.imageData || image.width <= 0 || image.height <= 0 || image.channels != 3)
        return QImage();
    return QImage(image.imageData, image.width, image.height, image.width * 3,
                  QImage::Format_RGB888);
}

QImage toQImage(const Image &image)
{
    if (!image.imageData || image.width <= 0 || image.height <= 0)
        return QImage();

    const int w = image.width;
    const int h = image.height;
    const int c = image.channels;

    switch (c) {
    case 3:
        return wrap(image).copy();
    case 1: {
        QImage out(w, h, QImage::Format_Grayscale8);
        for (int y = 0; y < h; ++y)
            std::memcpy(out.scanLine(y), image.imageData + static_cast<size_t>(y) * w, w);
        return out;
    }
    case 2: {
        QImage out(w, h, QImage::Format_ARGB32);
        for (int y = 0; y < h; ++y) {
            QRgb *dst = reinterpret_cast<QRgb *>(out.scanLine(y));
            const unsigned char *src = image.imageData + static_cast<size_t>(y) * w * 2;
            for (int x = 0; x < w; ++x)
                dst[x] = qRgba(src[x * 2], src[x * 2], src[x * 2], src[x * 2 + 1]);
        }
        return out;
    }
    case 4: {
        QImage out(w, h, QImage::Format_RGBA8888);
        for (int y = 0; y < h; ++y)
            std::memcpy(out.scanLine(y), image.imageData + static_cast<size_t>(y) * w * 4,
                        static_cast<size_t>(w) * 4);
        return out;
    }
    default:
        return QImage();
    }
}

std::shared_ptr<Image> fromQImage(const QImage &source)
{
    auto result = std::make_shared<Image>();
    if (source.isNull())
        return result;

    QImage rgb = source;
    if (rgb.hasAlphaChannel()) {
        QImage flat(rgb.size(), QImage::Format_RGB888);
        flat.fill(Qt::white);
        QPainter p(&flat);
        p.drawImage(0, 0, rgb);
        p.end();
        rgb = flat;
    } else if (rgb.format() != QImage::Format_RGB888) {
        rgb = rgb.convertToFormat(QImage::Format_RGB888);
    }

    const int w = rgb.width();
    const int h = rgb.height();
    // Image releases its buffer with free(), so allocate with malloc().
    unsigned char *buffer = static_cast<unsigned char *>(std::malloc(static_cast<size_t>(w) * h * 3));
    if (!buffer)
        return result;
    for (int y = 0; y < h; ++y)
        std::memcpy(buffer + static_cast<size_t>(y) * w * 3, rgb.constScanLine(y),
                    static_cast<size_t>(w) * 3);

    result->width = w;
    result->height = h;
    result->channels = 3;
    result->imageData = buffer;
    return result;
}

QPixmap toQPixmap(const Image &image)
{
    return QPixmap::fromImage(toQImage(image));
}

QString channelLabel(int channels)
{
    switch (channels) {
    case 1: return QStringLiteral("Gray");
    case 2: return QStringLiteral("Gray+Alpha");
    case 3: return QStringLiteral("RGB");
    case 4: return QStringLiteral("RGBA");
    default: return QStringLiteral("?");
    }
}

qint64 memoryBytes(const Image &image)
{
    return static_cast<qint64>(image.width) * image.height * image.channels;
}

QString formatBytes(qint64 bytes)
{
    const double kb = 1024.0;
    if (bytes < kb * kb)
        return QString::number(bytes / kb, 'f', 1) + QStringLiteral(" KB");
    if (bytes < kb * kb * kb)
        return QString::number(bytes / (kb * kb), 'f', 1) + QStringLiteral(" MB");
    return QString::number(bytes / (kb * kb * kb), 'f', 2) + QStringLiteral(" GB");
}

} // namespace ImageConvert
