#ifndef IMAGECONVERT_H
#define IMAGECONVERT_H

// NOTE: Qt headers must come before Image.h (see macro guards there).
#include <QImage>
#include <QPixmap>
#include <QString>
#include <memory>

#include "Image.h"

namespace ImageConvert {

/// Deep copy of an internal Image into an independent QImage (1-4 channels supported).
QImage toQImage(const Image &image);

/// Non-owning QImage view over the Image buffer (RGB888 only; zero copy).
/// The returned QImage is only valid while `image` is alive and unmodified.
QImage wrap(const Image &image);

/// Builds an internal 3-channel RGB Image from any QImage (alpha is composited on white).
std::shared_ptr<Image> fromQImage(const QImage &image);

QPixmap toQPixmap(const Image &image);

/// "RGB", "RGBA", "Gray", "Gray+Alpha"
QString channelLabel(int channels);

/// Size of the pixel buffer in bytes.
qint64 memoryBytes(const Image &image);

/// Human readable size, e.g. "5.9 MB".
QString formatBytes(qint64 bytes);

} // namespace ImageConvert

#endif // IMAGECONVERT_H
