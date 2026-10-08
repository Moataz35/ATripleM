#ifndef WIDGETS_H
#define WIDGETS_H

#include <QApplication>
#include <QColor>
#include <QIcon>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QWidget>

enum class Tool { Hand, Zoom, Crop, Select, Picker, Brush, Eraser };

namespace Theme {
/// Generates helper icons, loads :/style.qss and applies the Fusion + dark palette.
void apply(class QApplication &app);
/// Vector drawn tool icons (normal / hover / checked states).
QIcon toolIcon(Tool tool);
}

/// Label + slider + value badge. Double-click the badge or label to reset.
class SliderRow : public QWidget
{
    Q_OBJECT
public:
    SliderRow(const QString &title, int minimum, int maximum, int defaultValue = 0,
              const QString &suffix = QString(), QWidget *parent = nullptr);
    int value() const { return m_slider->value(); }
    void setValue(int v) { m_slider->setValue(v); }
    void reset() { m_slider->setValue(m_default); }
    bool isDefault() const { return m_slider->value() == m_default; }

signals:
    void valueChanged(int value);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateBadge(int v);
    QLabel *m_title;
    QLabel *m_badge;
    QSlider *m_slider;
    int m_default;
    QString m_suffix;
};

/// Small rotating arc used as a busy indicator.
class SpinnerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpinnerWidget(QWidget *parent = nullptr);
    void start();
    void stop();
    bool isSpinning() const { return m_timer.isActive(); }
    QSize sizeHint() const override { return {18, 18}; }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QTimer m_timer;
    int m_angle = 0;
};

/// Photoshop style overlapping foreground/background swatch.
class ColorSwatchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorSwatchWidget(QWidget *parent = nullptr);
    QColor foreground() const { return m_fg; }
    QColor background() const { return m_bg; }
    QSize sizeHint() const override { return {64, 64}; }

public slots:
    void setForeground(const QColor &c);
    void setBackground(const QColor &c);
    void swap();
    void resetDefaults();

signals:
    void foregroundChanged(const QColor &c);
    void backgroundChanged(const QColor &c);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QRect fgRect() const;
    QRect bgRect() const;
    QColor m_fg{0, 0, 0};
    QColor m_bg{255, 255, 255};
};

#endif // WIDGETS_H
