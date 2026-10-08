#include "Widgets.h"

#include <QApplication>
#include <QDir>
#include <QColorDialog>
#include <QEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QVBoxLayout>

// ---------------------------------------------------------------- Theme

namespace {

QPixmap newPixmap(int logical, qreal dpr = 2.0)
{
    QPixmap pm(int(logical * dpr), int(logical * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);
    return pm;
}

void drawToolGlyph(QPainter &p, Tool tool, const QColor &color)
{
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    // Logical canvas is 28 x 28.
    switch (tool) {
    case Tool::Hand: {
        for (int i = 0; i < 4; ++i) {
            const qreal x = 9.5 + i * 3.2;
            const qreal top = (i == 0 || i == 3) ? 7.5 : 4.5;
            p.drawLine(QPointF(x, 14), QPointF(x, top + (i == 3 ? 1.5 : 0)));
        }
        QPainterPath palm;
        palm.moveTo(9.5, 14);
        palm.lineTo(9.5, 16);
        palm.cubicTo(9.5, 22, 12, 24, 15.5, 24);
        palm.cubicTo(19.5, 24, 21.5, 21, 22.5, 17);
        palm.lineTo(22.5, 14);
        p.drawPath(palm);
        p.drawLine(QPointF(9.5, 16), QPointF(6, 12.5));
        break;
    }
    case Tool::Zoom:
        p.drawEllipse(QPointF(12, 12), 7.5, 7.5);
        p.drawLine(QPointF(17.6, 17.6), QPointF(23.5, 23.5));
        p.drawLine(QPointF(8.5, 12), QPointF(15.5, 12));
        p.drawLine(QPointF(12, 8.5), QPointF(12, 15.5));
        break;
    case Tool::Crop:
        p.drawLine(QPointF(8, 3.5), QPointF(8, 20));
        p.drawLine(QPointF(8, 20), QPointF(24.5, 20));
        p.drawLine(QPointF(3.5, 8), QPointF(20, 8));
        p.drawLine(QPointF(20, 8), QPointF(20, 24.5));
        break;
    case Tool::Select: {
        QPen dash(color, 1.8, Qt::CustomDashLine, Qt::FlatCap);
        dash.setDashPattern({2.2, 1.8});
        p.setPen(dash);
        p.drawRect(QRectF(5, 6, 18, 16));
        break;
    }
    case Tool::Picker:
        p.save();
        p.translate(14, 14);
        p.rotate(45);
        p.drawRoundedRect(QRectF(-3, -11, 6, 11), 1.5, 1.5);
        p.drawLine(QPointF(-5, -11), QPointF(5, -11));
        p.drawLine(QPointF(0, 0), QPointF(0, 8));
        p.restore();
        p.setBrush(color);
        p.drawEllipse(QPointF(7.5, 22.2), 1.4, 1.4);
        break;
    case Tool::Brush: {
        p.save();
        p.translate(14, 14);
        p.rotate(45);
        p.drawLine(QPointF(0, -12), QPointF(0, 0));
        p.restore();
        QPainterPath tip;
        tip.moveTo(12, 12);
        tip.cubicTo(8, 11, 5, 14, 5.5, 18);
        tip.cubicTo(6, 22.5, 4, 23, 3.5, 24);
        tip.cubicTo(9, 25.5, 14.5, 22, 14.5, 17.5);
        tip.cubicTo(14.5, 15, 13, 13.5, 12, 12);
        p.drawPath(tip);
        break;
    }
    case Tool::Eraser: {
        p.save();
        p.translate(14, 14);
        p.rotate(-45);
        p.drawRoundedRect(QRectF(-5, -10, 10, 20), 2, 2);
        p.drawLine(QPointF(-5, 2), QPointF(5, 2));
        p.restore();
        p.drawLine(QPointF(15, 24.5), QPointF(25, 24.5));
        break;
    }
    }
}

QString makeHelperIcons()
{
    const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                            + QStringLiteral("/qtphoto_icons");
    QDir().mkpath(dirPath);

    auto save = [&](const QString &name, auto painter) {
        QPixmap pm = newPixmap(12, 2.0);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);
        painter(p);
        p.end();
        pm.save(dirPath + QLatin1Char('/') + name, "PNG");
    };
    auto chevron = [](bool down, const QColor &c) {
        return [down, c](QPainter &p) {
            p.setPen(QPen(c, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            if (down)
                p.drawPolyline(QPolygonF({QPointF(2.5, 4.2), QPointF(6, 7.8), QPointF(9.5, 4.2)}));
            else
                p.drawPolyline(QPolygonF({QPointF(2.5, 7.8), QPointF(6, 4.2), QPointF(9.5, 7.8)}));
        };
    };
    save(QStringLiteral("down.png"), chevron(true, QColor("#b8b8b8")));
    save(QStringLiteral("up.png"), chevron(false, QColor("#b8b8b8")));
    save(QStringLiteral("down_hi.png"), chevron(true, Qt::white));
    save(QStringLiteral("up_hi.png"), chevron(false, Qt::white));
    save(QStringLiteral("check.png"), [](QPainter &p) {
        p.setPen(QPen(Qt::white, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(QPolygonF({QPointF(2.5, 6.3), QPointF(5, 8.7), QPointF(9.5, 3.5)}));
    });
    return dirPath;
}

} // namespace

namespace Theme {

QIcon toolIcon(Tool tool)
{
    QIcon icon;
    auto add = [&](const QColor &c, QIcon::Mode mode, QIcon::State state) {
        QPixmap pm = newPixmap(28, 2.0);
        QPainter p(&pm);
        drawToolGlyph(p, tool, c);
        p.end();
        icon.addPixmap(pm, mode, state);
    };
    add(QColor("#a8a8a8"), QIcon::Normal, QIcon::Off);
    add(QColor("#ffffff"), QIcon::Active, QIcon::Off);
    add(QColor("#ffffff"), QIcon::Normal, QIcon::On);
    add(QColor("#ffffff"), QIcon::Active, QIcon::On);
    add(QColor("#555555"), QIcon::Disabled, QIcon::Off);
    return icon;
}

void apply(QApplication &app)
{
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette pal;
    const QColor base("#181818"), panel("#212121"), text("#d4d4d4"), accent("#007acc");
    pal.setColor(QPalette::Window, panel);
    pal.setColor(QPalette::WindowText, text);
    pal.setColor(QPalette::Base, base);
    pal.setColor(QPalette::AlternateBase, panel);
    pal.setColor(QPalette::ToolTipBase, QColor("#2a2a2a"));
    pal.setColor(QPalette::ToolTipText, text);
    pal.setColor(QPalette::Text, text);
    pal.setColor(QPalette::Button, panel);
    pal.setColor(QPalette::ButtonText, text);
    pal.setColor(QPalette::BrightText, Qt::white);
    pal.setColor(QPalette::Highlight, accent);
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::PlaceholderText, QColor("#777777"));
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor("#666666"));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#666666"));
    pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#666666"));
    app.setPalette(pal);

    QFont f = app.font();
    f.setFamily(QStringLiteral("Segoe UI"));
    f.setPointSize(9);
    app.setFont(f);

    const QString iconDir = makeHelperIcons();
    QFile file(QStringLiteral(":/style.qss"));
    if (file.open(QIODevice::ReadOnly)) {
        QString qss = QString::fromUtf8(file.readAll());
        qss.replace(QStringLiteral("@ICONS@"), iconDir);
        app.setStyleSheet(qss);
    }
}

} // namespace Theme

// ---------------------------------------------------------------- SliderRow

SliderRow::SliderRow(const QString &title, int minimum, int maximum, int defaultValue,
                     const QString &suffix, QWidget *parent)
    : QWidget(parent), m_default(defaultValue), m_suffix(suffix)
{
    m_title = new QLabel(title, this);
    m_title->setObjectName(QStringLiteral("sliderTitle"));
    m_badge = new QLabel(this);
    m_badge->setObjectName(QStringLiteral("valueBadge"));
    m_badge->setAlignment(Qt::AlignCenter);
    m_badge->setMinimumWidth(54);
    m_badge->setToolTip(tr("Double-click to reset"));
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(minimum, maximum);
    m_slider->setValue(defaultValue);

    auto *top = new QHBoxLayout;
    top->setContentsMargins(0, 0, 0, 0);
    top->addWidget(m_title);
    top->addStretch(1);
    top->addWidget(m_badge);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 2, 0, 2);
    lay->setSpacing(4);
    lay->addLayout(top);
    lay->addWidget(m_slider);

    m_badge->installEventFilter(this);
    m_title->installEventFilter(this);
    m_slider->installEventFilter(this);
    connect(m_slider, &QSlider::valueChanged, this, [this](int v) {
        updateBadge(v);
        emit valueChanged(v);
    });
    updateBadge(defaultValue);
}

void SliderRow::updateBadge(int v)
{
    const QString sign = (m_default == 0 && v > 0) ? QStringLiteral("+") : QString();
    m_badge->setText(sign + QString::number(v) + m_suffix);
}

bool SliderRow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick && watched != m_slider) {
        reset();
        return true;
    }
    if (watched == m_slider && event->type() == QEvent::Wheel && !m_slider->hasFocus()) {
        event->ignore(); // let the dock scroll area handle wheel events
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

// ---------------------------------------------------------------- SpinnerWidget

SpinnerWidget::SpinnerWidget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(18, 18);
    m_timer.setInterval(40);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_angle = (m_angle + 30) % 360;
        update();
    });
    hide();
}

void SpinnerWidget::start()
{
    show();
    m_timer.start();
}

void SpinnerWidget::stop()
{
    m_timer.stop();
    hide();
}

void SpinnerWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRectF r = QRectF(rect()).adjusted(3, 3, -3, -3);
    p.setPen(QPen(QColor("#333333"), 2));
    p.drawEllipse(r);
    p.setPen(QPen(QColor("#007acc"), 2, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(r, -m_angle * 16, 100 * 16);
}

// ---------------------------------------------------------------- ColorSwatchWidget

ColorSwatchWidget::ColorSwatchWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(64, 64);
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("Click a swatch to choose a color. Double-click the corner to swap."));
}

QRect ColorSwatchWidget::fgRect() const
{
    const int s = qMin(width(), height()) * 3 / 5;
    return QRect(2, 2, s, s);
}

QRect ColorSwatchWidget::bgRect() const
{
    const int s = qMin(width(), height()) * 3 / 5;
    return QRect(qMin(width(), height()) - s - 2, qMin(width(), height()) - s - 2, s, s);
}

void ColorSwatchWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setPen(QPen(QColor("#555555"), 1));
    p.setBrush(m_bg);
    p.drawRect(bgRect());
    p.setPen(QPen(QColor("#e0e0e0"), 1));
    p.setBrush(m_fg);
    p.drawRect(fgRect());
}

void ColorSwatchWidget::mousePressEvent(QMouseEvent *e)
{
    const QPoint pos = e->position().toPoint();
    if (fgRect().contains(pos)) {
        const QColor c = QColorDialog::getColor(m_fg, window(), tr("Foreground Color"));
        if (c.isValid())
            setForeground(c);
    } else if (bgRect().contains(pos)) {
        const QColor c = QColorDialog::getColor(m_bg, window(), tr("Background Color"));
        if (c.isValid())
            setBackground(c);
    } else {
        swap();
    }
}

void ColorSwatchWidget::setForeground(const QColor &c)
{
    if (c == m_fg || !c.isValid())
        return;
    m_fg = c;
    update();
    emit foregroundChanged(c);
}

void ColorSwatchWidget::setBackground(const QColor &c)
{
    if (c == m_bg || !c.isValid())
        return;
    m_bg = c;
    update();
    emit backgroundChanged(c);
}

void ColorSwatchWidget::swap()
{
    const QColor f = m_fg, b = m_bg;
    m_fg = b;
    m_bg = f;
    update();
    emit foregroundChanged(m_fg);
    emit backgroundChanged(m_bg);
}

void ColorSwatchWidget::resetDefaults()
{
    setForeground(Qt::black);
    setBackground(Qt::white);
}
