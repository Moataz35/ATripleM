#include "ImageOps.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUuid>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace ImageOps {

namespace {

inline unsigned char clamp8(double v)
{
    return static_cast<unsigned char>(v < 0.0 ? 0 : (v > 255.0 ? 255 : static_cast<int>(v + 0.5)));
}

std::shared_ptr<Image> makeImage(int w, int h, int channels)
{
    auto img = std::make_shared<Image>();
    img->width = w;
    img->height = h;
    img->channels = channels;
    // Image frees with free(); allocate with malloc.
    img->imageData = static_cast<unsigned char *>(
        std::malloc(static_cast<size_t>(w) * h * channels));
    return img;
}

bool isAnsiSafe(const QString &path)
{
    return QString::fromLocal8Bit(path.toLocal8Bit()) == path;
}

QString stagingPath(const QString &path)
{
    const QString suffix = QFileInfo(path).suffix();
    return QDir::temp().filePath(QStringLiteral("qtphoto_%1.%2")
                                     .arg(QUuid::createUuid().toString(QUuid::Id128), suffix));
}

void writeCallback(void *context, void *data, int size)
{
    static_cast<QByteArray *>(context)->append(static_cast<const char *>(data), size);
}

} // namespace

void hueShift(Image &image, int degrees)
{
    if (!image.imageData || image.channels < 3 || degrees == 0)
        return;
    const double a = degrees * 3.14159265358979323846 / 180.0;
    const double c = std::cos(a), s = std::sin(a);
    const double m[3][3] = {
        {0.213 + c * 0.787 - s * 0.213, 0.715 - c * 0.715 - s * 0.715, 0.072 - c * 0.072 + s * 0.928},
        {0.213 - c * 0.213 + s * 0.143, 0.715 + c * 0.285 + s * 0.140, 0.072 - c * 0.072 - s * 0.283},
        {0.213 - c * 0.213 - s * 0.787, 0.715 - c * 0.715 + s * 0.715, 0.072 + c * 0.928 + s * 0.072}};

    const size_t count = static_cast<size_t>(image.width) * image.height;
    unsigned char *p = image.imageData;
    for (size_t i = 0; i < count; ++i, p += image.channels) {
        const double r = p[0], g = p[1], b = p[2];
        p[0] = clamp8(m[0][0] * r + m[0][1] * g + m[0][2] * b);
        p[1] = clamp8(m[1][0] * r + m[1][1] * g + m[1][2] * b);
        p[2] = clamp8(m[2][0] * r + m[2][1] * g + m[2][2] * b);
    }
}

void sharpen(Image &image, int amount)
{
    if (!image.imageData || image.channels < 3 || amount <= 0 || image.width < 3 || image.height < 3)
        return;
    const double k = std::min(amount, 100) / 100.0 * 2.0;
    const int W = image.width, H = image.height, C = image.channels;
    std::vector<unsigned char> src(image.imageData, image.imageData + static_cast<size_t>(W) * H * C);

    for (int y = 0; y < H; ++y) {
        const int y0 = std::max(0, y - 1), y1 = std::min(H - 1, y + 1);
        for (int x = 0; x < W; ++x) {
            const int x0 = std::max(0, x - 1), x1 = std::min(W - 1, x + 1);
            for (int c = 0; c < 3; ++c) {
                int sum = 0, n = 0;
                for (int yy = y0; yy <= y1; ++yy)
                    for (int xx = x0; xx <= x1; ++xx) {
                        sum += src[(static_cast<size_t>(yy) * W + xx) * C + c];
                        ++n;
                    }
                const double blur = static_cast<double>(sum) / n;
                const double v = src[(static_cast<size_t>(y) * W + x) * C + c];
                image.imageData[(static_cast<size_t>(y) * W + x) * C + c] = clamp8(v + k * (v - blur));
            }
        }
    }
}

void oilPainting(Image &image, int radius, int levels)
{
    if (!image.imageData || image.channels < 3 || radius < 1)
        return;
    levels = std::clamp(levels, 2, 255);
    const int W = image.width, H = image.height, C = image.channels;
    std::vector<unsigned char> src(image.imageData, image.imageData + static_cast<size_t>(W) * H * C);
    std::vector<int> count(levels), sr(levels), sg(levels), sb(levels);

    for (int y = 0; y < H; ++y) {
        const int y0 = std::max(0, y - radius), y1 = std::min(H - 1, y + radius);
        for (int x = 0; x < W; ++x) {
            const int x0 = std::max(0, x - radius), x1 = std::min(W - 1, x + radius);
            std::fill(count.begin(), count.end(), 0);
            std::fill(sr.begin(), sr.end(), 0);
            std::fill(sg.begin(), sg.end(), 0);
            std::fill(sb.begin(), sb.end(), 0);
            for (int yy = y0; yy <= y1; ++yy) {
                for (int xx = x0; xx <= x1; ++xx) {
                    const unsigned char *q = &src[(static_cast<size_t>(yy) * W + xx) * C];
                    const int bin = ((q[0] + q[1] + q[2]) / 3) * (levels - 1) / 255;
                    ++count[bin];
                    sr[bin] += q[0];
                    sg[bin] += q[1];
                    sb[bin] += q[2];
                }
            }
            int best = 0;
            for (int i = 1; i < levels; ++i)
                if (count[i] > count[best])
                    best = i;
            unsigned char *dst = &image.imageData[(static_cast<size_t>(y) * W + x) * C];
            dst[0] = static_cast<unsigned char>(sr[best] / count[best]);
            dst[1] = static_cast<unsigned char>(sg[best] / count[best]);
            dst[2] = static_cast<unsigned char>(sb[best] / count[best]);
        }
    }
}

void skew(Image &image, double degrees, Color fill)
{
    if (!image.imageData || image.channels < 3)
        return;
    degrees = std::clamp(degrees, -75.0, 75.0);
    const double t = std::tan(degrees * 3.14159265358979323846 / 180.0);
    const int extra = static_cast<int>(std::ceil(std::fabs(t) * (image.height - 1)));
    const int W = image.width, H = image.height, C = image.channels;
    const int newW = W + extra;
    auto out = makeImage(newW, H, C);
    for (size_t i = 0; i < static_cast<size_t>(newW) * H; ++i) {
        out->imageData[i * C] = fill.red;
        out->imageData[i * C + 1] = fill.green;
        out->imageData[i * C + 2] = fill.blue;
        for (int c = 3; c < C; ++c)
            out->imageData[i * C + c] = 255;
    }
    for (int y = 0; y < H; ++y) {
        const double shift = t >= 0 ? t * (H - 1 - y) : -t * y;
        const int off = static_cast<int>(std::lround(shift));
        std::memcpy(&out->imageData[(static_cast<size_t>(y) * newW + off) * C],
                    &image.imageData[static_cast<size_t>(y) * W * C], static_cast<size_t>(W) * C);
    }
    image = *out;
}

void applyAdjustments(Image &image, const AdjustParams &p)
{
    if (!image.imageData)
        return;
    if (p.brightness) Adjustments::Brightness(image, p.brightness);
    if (p.contrast)   Adjustments::Contrast(image, p.contrast);
    if (p.saturation) Adjustments::Saturation(image, p.saturation);
    if (p.hue)        hueShift(image, p.hue);
    if (p.blur)       ImageFilter::Blur(image, p.blur);
    if (p.sharpen)    sharpen(image, p.sharpen);
}

void applyInRegion(Image &image, int x, int y, int w, int h, const std::function<void(Image &)> &op)
{
    if (!image.imageData)
        return;
    const int x0 = std::clamp(x, 0, image.width);
    const int y0 = std::clamp(y, 0, image.height);
    const int x1 = std::clamp(x + w, 0, image.width);
    const int y1 = std::clamp(y + h, 0, image.height);
    const int rw = x1 - x0, rh = y1 - y0;
    if (rw <= 0 || rh <= 0)
        return;

    const int C = image.channels;
    auto sub = makeImage(rw, rh, C);
    for (int row = 0; row < rh; ++row)
        std::memcpy(&sub->imageData[static_cast<size_t>(row) * rw * C],
                    &image.imageData[(static_cast<size_t>(y0 + row) * image.width + x0) * C],
                    static_cast<size_t>(rw) * C);

    op(*sub);

    if (sub->width != rw || sub->height != rh || sub->channels != C || !sub->imageData)
        return;
    for (int row = 0; row < rh; ++row)
        std::memcpy(&image.imageData[(static_cast<size_t>(y0 + row) * image.width + x0) * C],
                    &sub->imageData[static_cast<size_t>(row) * rw * C], static_cast<size_t>(rw) * C);
}

bool writeImage(const Image &image, const QString &path, int jpegQuality, QString *error)
{
    if (!image.imageData || image.width <= 0 || image.height <= 0) {
        if (error) *error = QStringLiteral("There is no image to save.");
        return false;
    }
    const QString ext = QFileInfo(path).suffix().toLower();
    QByteArray bytes;
    int ok = 0;
    if (ext == QLatin1String("png"))
        ok = stbi_write_png_to_func(writeCallback, &bytes, image.width, image.height, image.channels,
                                    image.imageData, image.width * image.channels);
    else if (ext == QLatin1String("jpg") || ext == QLatin1String("jpeg"))
        ok = stbi_write_jpg_to_func(writeCallback, &bytes, image.width, image.height, image.channels,
                                    image.imageData, std::clamp(jpegQuality, 1, 100));
    else if (ext == QLatin1String("bmp"))
        ok = stbi_write_bmp_to_func(writeCallback, &bytes, image.width, image.height, image.channels,
                                    image.imageData);
    else if (ext == QLatin1String("tga"))
        ok = stbi_write_tga_to_func(writeCallback, &bytes, image.width, image.height, image.channels,
                                    image.imageData);
    else {
        if (error) *error = QStringLiteral("Unsupported file format: .%1").arg(ext);
        return false;
    }
    if (!ok || bytes.isEmpty()) {
        if (error) *error = QStringLiteral("Failed to encode the image.");
        return false;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size()) {
        if (error) *error = QStringLiteral("Cannot write file: %1").arg(file.errorString());
        return false;
    }
    return true;
}

std::shared_ptr<ImageEditor> loadEditor(const QString &path, QString *error)
{
    QString readPath = path;
    QString staged;
    if (!isAnsiSafe(path)) {
        staged = stagingPath(path);
        if (!QFile::copy(path, staged)) {
            if (error) *error = QStringLiteral("Cannot read file: %1").arg(path);
            return nullptr;
        }
        readPath = staged;
    }
    auto editor = std::make_shared<ImageEditor>();
    try {
        editor->loadImageFromFile(readPath.toLocal8Bit().toStdString());
    } catch (const std::exception &e) {
        if (!staged.isEmpty()) QFile::remove(staged);
        if (error) *error = QString::fromLocal8Bit(e.what());
        return nullptr;
    }
    if (!staged.isEmpty()) QFile::remove(staged);
    return editor;
}

bool saveEditor(ImageEditor &editor, const QString &path, QString *error)
{
    try {
        if (isAnsiSafe(path)) {
            const std::string native = path.toLocal8Bit().toStdString();
            if (native == editor.getFilePath())
                editor.saveToSameImage();
            else
                editor.saveNewCopy(native);
        } else {
            const QString tmp = stagingPath(path);
            editor.saveNewCopy(tmp.toLocal8Bit().toStdString());
            QFile::remove(path);
            if (!QFile::copy(tmp, path)) {
                QFile::remove(tmp);
                if (error) *error = QStringLiteral("Cannot write file: %1").arg(path);
                return false;
            }
            QFile::remove(tmp);
        }
    } catch (const std::exception &e) {
        if (error) *error = QString::fromLocal8Bit(e.what());
        return false;
    }
    if (QFileInfo(path).size() <= 0) {
        if (error) *error = QStringLiteral("The file could not be written.");
        return false;
    }
    return true;
}

} // namespace ImageOps
