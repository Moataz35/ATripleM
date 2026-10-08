#include "CanvasView.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>
#include <QStyleOptionGraphicsItem>
#include <QUrl>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

namespace {
constexpr double kMinZoom = 0.02;
constexpr double kMaxZoom = 32.0;
}

// ---------------------------------------------------------------- ImageItem

class ImageItem : public QGraphicsItem
{
public:
    QImage image;
    QImage preview;

    QRectF boundingRect() const override
    {
        return QRectF(0, 0, image.width(), image.height());
    }

    void changeGeometry() { prepareGeometryChange(); }

    void paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *) override
    {
        if (image.isNull())
            return;
        const qreal lod = opt->levelOfDetailFromTransform(p->worldTransform());
        p->setRenderHint(QPainter::SmoothPixmapTransform, lod < 1.0);
        if (!preview.isNull()) {
            p->drawImage(boundingRect(), preview);
            return;
        }
        const QRect exposed = opt->exposedRect.toAlignedRect().intersected(image.rect());
        if (exposed.isEmpty())
            return;
        p->drawImage(exposed, image, exposed);
    }
};

// ---------------------------------------------------------------- CanvasView

CanvasView::CanvasView(QWidget *parent) : QGraphicsView(parent)
{
    setScene(&m_scene);
    m_item = new ImageItem;
    m_scene.addItem(m_item);

    setAcceptDrops(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::NoFrame);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setRenderHint(QPainter::Antialiasing, false);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);
    viewport()->setAttribute(Qt::WA_NoSystemBackground, true);
    updateCursor();
}

CanvasView::~CanvasView() = default;

QStringList CanvasView::imagePathsFromMime(const QMimeData *mime)
{
    QStringList result;
    if (!mime || !mime->hasUrls())
        return result;
    static const QStringList exts{"png", "jpg", "jpeg", "bmp", "tga"};
    for (const QUrl &url : mime->urls()) {
        if (!url.isLocalFile())
            continue;
        const QString path = url.toLocalFile();
        if (exts.contains(QFileInfo(path).suffix().toLower()))
            result << path;
    }
    return result;
}

// ------------------------------------------------------------ image handling

void CanvasView::setImage(const QImage &image, bool resetView)
{
    const QSize oldSize = m_item->image.size();
    m_item->changeGeometry();
    m_item->image = image;
    m_item->preview = QImage();
    m_item->update();

    if (image.size() != oldSize || resetView) {
        if (resetView || m_selection.isEmpty() == false)
            clearSelection();
    } else if (!m_selection.isEmpty()) {
        setSelection(m_selection.intersected(imageRect()));
    }

    updateSceneRect();
    if (resetView || m_fit) {
        doFit();
    } else if (image.size() != oldSize) {
        centerOn(QPointF(image.width() / 2.0, image.height() / 2.0));
    }
    viewport()->update();
}

void CanvasView::clearImage()
{
    m_item->changeGeometry();
    m_item->image = QImage();
    m_item->preview = QImage();
    clearSelection();
    m_scene.setSceneRect(QRectF());
    viewport()->update();
}

QImage CanvasView::image() const { return m_item->image; }
bool CanvasView::hasImage() const { return !m_item->image.isNull(); }

void CanvasView::setPreviewImage(const QImage &preview)
{
    m_item->preview = preview;
    m_item->update();
}

void CanvasView::clearPreview()
{
    if (m_item->preview.isNull())
        return;
    m_item->preview = QImage();
    m_item->update();
}

QRect CanvasView::imageRect() const { return m_item->image.rect(); }

// ------------------------------------------------------------ tools & state

void CanvasView::setTool(Tool tool)
{
    if (m_stroking)
        endStroke();
    m_tool = tool;
    updateCursor();
    viewport()->update();
}

void CanvasView::setBrushSize(int px)
{
    m_brushSize = std::clamp(px, 1, 500);
    updateCursor();
}

void CanvasView::setBrushOpacity(int percent) { m_brushOpacity = std::clamp(percent, 1, 100); }
void CanvasView::setForegroundColor(const QColor &c) { m_fg = c; }
void CanvasView::setBackgroundColor(const QColor &c) { m_bg = c; }

void CanvasView::setPaintingEnabled(bool enabled)
{
    m_paintingEnabled = enabled;
    if (!enabled && m_stroking)
        endStroke();
}

void CanvasView::setSelection(const QRect &r)
{
    const QRect clipped = r.intersected(imageRect());
    const QRect value = (clipped.width() > 0 && clipped.height() > 0) ? clipped : QRect();
    if (value == m_selection)
        return;
    m_selection = value;
    viewport()->update();
    emit selectionChanged(m_selection);
}

void CanvasView::clearSelection() { setSelection(QRect()); }

void CanvasView::selectAll()
{
    if (hasImage())
        setSelection(imageRect());
}

void CanvasView::updateCursor()
{
    if (m_panning) {
        viewport()->setCursor(Qt::ClosedHandCursor);
        return;
    }
    if (m_space || m_tool == Tool::Hand) {
        viewport()->setCursor(Qt::OpenHandCursor);
        return;
    }
    switch (m_tool) {
    case Tool::Zoom:
        viewport()->setCursor(Qt::PointingHandCursor);
        break;
    case Tool::Crop:
    case Tool::Select:
    case Tool::Picker:
        viewport()->setCursor(Qt::CrossCursor);
        break;
    case Tool::Brush:
    case Tool::Eraser: {
        const int d = std::clamp(int(std::lround(m_brushSize * m_zoom)), 6, 160);
        const int s = d + 4;
        QPixmap pm(s, s);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(QPen(QColor(0, 0, 0, 200), 3));
        p.drawEllipse(QRectF(2, 2, d, d));
        p.setPen(QPen(Qt::white, 1.2));
        p.drawEllipse(QRectF(2, 2, d, d));
        p.end();
        viewport()->setCursor(QCursor(pm, s / 2, s / 2));
        break;
    }
    default:
        viewport()->setCursor(Qt::ArrowCursor);
    }
}

// ------------------------------------------------------------ zoom / pan

void CanvasView::updateSceneRect()
{
    if (!hasImage())
        return;
    const double w = m_item->image.width();
    const double h = m_item->image.height();
    const double mx = 4096.0 / m_zoom + w;
    const double my = 4096.0 / m_zoom + h;
    m_scene.setSceneRect(QRectF(-mx, -my, w + 2 * mx, h + 2 * my));
}

void CanvasView::applyZoom(double z, const QPoint &anchor)
{
    z = std::clamp(z, kMinZoom, kMaxZoom);
    const QPointF scenePos = mapToScene(anchor);
    m_zoom = z;
    setTransform(QTransform::fromScale(z, z));
    updateSceneRect();
    const QPoint np = mapFromScene(scenePos);
    const QPoint d = np - anchor;
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + d.x());
    verticalScrollBar()->setValue(verticalScrollBar()->value() + d.y());
    updateCursor();
    viewport()->update();
    emit zoomChanged(m_zoom);
}

void CanvasView::doFit()
{
    if (!hasImage())
        return;
    m_fit = true;
    const double pad = 48.0;
    const double vw = std::max(1, viewport()->width()) - pad;
    const double vh = std::max(1, viewport()->height()) - pad;
    double z = std::min(vw / m_item->image.width(), vh / m_item->image.height());
    z = std::clamp(z, kMinZoom, kMaxZoom);
    m_zoom = z;
    setTransform(QTransform::fromScale(z, z));
    updateSceneRect();
    centerOn(QPointF(m_item->image.width() / 2.0, m_item->image.height() / 2.0));
    updateCursor();
    viewport()->update();
    emit zoomChanged(m_zoom);
}

void CanvasView::fitToScreen() { doFit(); }

void CanvasView::actualSize()
{
    if (!hasImage())
        return;
    m_fit = false;
    m_zoom = 1.0;
    setTransform(QTransform());
    updateSceneRect();
    centerOn(QPointF(m_item->image.width() / 2.0, m_item->image.height() / 2.0));
    updateCursor();
    viewport()->update();
    emit zoomChanged(m_zoom);
}

void CanvasView::setZoom(double factor)
{
    if (!hasImage())
        return;
    m_fit = false;
    applyZoom(factor, viewport()->rect().center());
}

void CanvasView::zoomIn() { setZoom(m_zoom * 1.25); }
void CanvasView::zoomOut() { setZoom(m_zoom / 1.25); }

void CanvasView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (m_fit && hasImage())
        doFit();
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) && hasImage()) {
        const double delta = event->angleDelta().y() != 0 ? event->angleDelta().y()
                                                          : event->pixelDelta().y();
        if (delta != 0) {
            m_fit = false;
            applyZoom(m_zoom * std::pow(1.0015, delta), event->position().toPoint());
        }
        event->accept();
        return;
    }
    QGraphicsView::wheelEvent(event);
}

// ------------------------------------------------------------ mouse

QPointF CanvasView::toImage(const QPoint &viewPos) const { return mapToScene(viewPos); }

QRect CanvasView::rectFromDrag(const QPointF &a, const QPointF &b) const
{
    const QPoint p1(int(std::lround(a.x())), int(std::lround(a.y())));
    const QPoint p2(int(std::lround(b.x())), int(std::lround(b.y())));
    return QRect(p1, p2).normalized().intersected(imageRect());
}

void CanvasView::pickColor(const QPoint &viewPos, bool background)
{
    const QPointF p = toImage(viewPos);
    const int x = int(std::floor(p.x())), y = int(std::floor(p.y()));
    if (x < 0 || y < 0 || x >= m_item->image.width() || y >= m_item->image.height())
        return;
    emit colorPicked(QColor(m_item->image.pixel(x, y)), background);
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
    setFocus(Qt::MouseFocusReason);
    if (!hasImage()) {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    const bool panGesture = event->button() == Qt::MiddleButton ||
                            (event->button() == Qt::LeftButton && (m_space || m_tool == Tool::Hand));
    if (panGesture) {
        m_panning = true;
        m_lastPan = event->pos();
        updateCursor();
        event->accept();
        return;
    }

    if (event->button() == Qt::RightButton) {
        if (m_tool == Tool::Picker)
            pickColor(event->pos(), true);
        else if (m_tool == Tool::Zoom) {
            m_fit = false;
            applyZoom(m_zoom / 1.5, event->pos());
        }
        event->accept();
        return;
    }
    if (event->button() != Qt::LeftButton)
        return;

    switch (m_tool) {
    case Tool::Zoom:
        m_fit = false;
        applyZoom((event->modifiers() & Qt::AltModifier) ? m_zoom / 1.5 : m_zoom * 1.5, event->pos());
        break;
    case Tool::Crop:
    case Tool::Select:
        m_selecting = true;
        m_selStart = toImage(event->pos());
        break;
    case Tool::Picker:
        pickColor(event->pos(), (event->modifiers() & Qt::AltModifier) != 0);
        break;
    case Tool::Brush:
    case Tool::Eraser:
        if (m_paintingEnabled)
            beginStroke(toImage(event->pos()));
        break;
    default:
        break;
    }
    event->accept();
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
    if (hasImage()) {
        const QPointF p = toImage(event->pos());
        const QPoint ip(int(std::floor(p.x())), int(std::floor(p.y())));
        emit cursorPosChanged(ip, imageRect().contains(ip));
    }

    if (m_panning) {
        const QPoint d = event->pos() - m_lastPan;
        m_lastPan = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - d.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - d.y());
        m_fit = false;
        event->accept();
        return;
    }
    if (m_selecting) {
        const QRect r = rectFromDrag(m_selStart, toImage(event->pos()));
        if (r != m_selection) {
            m_selection = (r.width() > 0 && r.height() > 0) ? r : QRect();
            viewport()->update();
            emit selectionChanged(m_selection);
        }
        event->accept();
        return;
    }
    if (m_stroking) {
        continueStroke(toImage(event->pos()));
        event->accept();
        return;
    }
    if (m_tool == Tool::Picker && (event->buttons() & Qt::LeftButton) && hasImage())
        pickColor(event->pos(), (event->modifiers() & Qt::AltModifier) != 0);
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_panning && (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)) {
        m_panning = false;
        updateCursor();
        event->accept();
        return;
    }
    if (m_selecting && event->button() == Qt::LeftButton) {
        m_selecting = false;
        const QPointF end = toImage(event->pos());
        if (std::fabs(end.x() - m_selStart.x()) < 1.0 && std::fabs(end.y() - m_selStart.y()) < 1.0)
            clearSelection(); // plain click deselects
        event->accept();
        return;
    }
    if (m_stroking && event->button() == Qt::LeftButton) {
        endStroke();
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_tool == Tool::Crop && !m_selection.isEmpty() && event->button() == Qt::LeftButton) {
        emit cropRequested(m_selection);
        event->accept();
        return;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void CanvasView::enterEvent(QEnterEvent *event)
{
    QGraphicsView::enterEvent(event);
    if (window() && window()->isActiveWindow())
        setFocus(Qt::MouseFocusReason);
}

void CanvasView::leaveEvent(QEvent *event)
{
    QGraphicsView::leaveEvent(event);
    emit cursorPosChanged(QPoint(), false);
}

// ------------------------------------------------------------ keyboard

void CanvasView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (!event->isAutoRepeat()) {
            m_space = true;
            updateCursor();
        }
        event->accept();
        return;
    }
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) &&
        m_tool == Tool::Crop && !m_selection.isEmpty()) {
        emit cropRequested(m_selection);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape && !m_selection.isEmpty()) {
        clearSelection();
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_space = false;
        m_panning = m_panning && (QGuiApplication::mouseButtons() & (Qt::LeftButton | Qt::MiddleButton));
        updateCursor();
        event->accept();
        return;
    }
    QGraphicsView::keyReleaseEvent(event);
}

// ------------------------------------------------------------ painting tools

void CanvasView::beginStroke(const QPointF &p)
{
    if (!hasImage())
        return;
    m_strokeBase = m_item->image;
    m_strokeLayer = QImage(m_item->image.size(), QImage::Format_ARGB32_Premultiplied);
    m_strokeLayer.fill(Qt::transparent);
    m_stroking = true;
    m_lastStrokePt = p;
    drawSegment(p, p);
}

void CanvasView::continueStroke(const QPointF &p)
{
    drawSegment(m_lastStrokePt, p);
    m_lastStrokePt = p;
}

void CanvasView::drawSegment(const QPointF &a, const QPointF &b)
{
    const QColor color = (m_tool == Tool::Eraser) ? m_bg : m_fg;
    {
        QPainter lp(&m_strokeLayer);
        lp.setRenderHint(QPainter::Antialiasing, true);
        if (!m_selection.isEmpty())
            lp.setClipRect(m_selection);
        QPen pen(color, m_brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        lp.setPen(pen);
        if (a == b) {
            lp.setPen(Qt::NoPen);
            lp.setBrush(color);
            lp.drawEllipse(a, m_brushSize / 2.0, m_brushSize / 2.0);
        } else {
            lp.drawLine(a, b);
        }
    }
    const double r = m_brushSize / 2.0 + 2.0;
    const QRect dirty = QRectF(a, b).normalized().adjusted(-r, -r, r, r).toAlignedRect()
                            .intersected(m_item->image.rect());
    if (dirty.isEmpty())
        return;
    {
        QPainter p(&m_item->image);
        p.setClipRect(dirty);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawImage(dirty.topLeft(), m_strokeBase, dirty);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        p.setOpacity(m_brushOpacity / 100.0);
        p.drawImage(dirty.topLeft(), m_strokeLayer, dirty);
    }
    m_item->update(QRectF(dirty));
}

void CanvasView::endStroke()
{
    if (!m_stroking)
        return;
    m_stroking = false;
    m_strokeBase = QImage();
    m_strokeLayer = QImage();
    emit strokeFinished(m_item->image, m_tool == Tool::Eraser ? tr("Eraser") : tr("Brush Stroke"));
}

// ------------------------------------------------------------ drawing

void CanvasView::drawBackground(QPainter *painter, const QRectF &)
{
    painter->save();
    painter->resetTransform();
    const QRect vp = viewport()->rect();
    painter->fillRect(vp, QColor("#161616"));

    // Dot pattern anchored to the scene so it pans with the content.
    const int step = 24;
    const QPoint origin = hasImage() ? mapFromScene(QPointF(0, 0)) : QPoint(0, 0);
    const int ox = ((origin.x() % step) + step) % step;
    const int oy = ((origin.y() % step) + step) % step;
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#2a2a2a"));
    for (int x = ox - step; x < vp.width(); x += step)
        for (int y = oy - step; y < vp.height(); y += step)
            painter->drawRect(x, y, 2, 2);

    if (hasImage()) {
        const QRect ir = mapFromScene(QRectF(0, 0, m_item->image.width(), m_item->image.height()))
                             .boundingRect();
        painter->setRenderHint(QPainter::Antialiasing, true);
        const int layers = 16;
        for (int i = layers; i >= 1; --i) {
            const double t = 1.0 - double(i) / (layers + 1);
            painter->setBrush(QColor(0, 0, 0, int(26 * t * t + 2)));
            painter->drawRoundedRect(QRectF(ir).adjusted(-i, -i + 6, i, i + 6), i, i);
        }
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QColor("#333333"), 1));
        painter->drawRect(ir.adjusted(-1, -1, 0, 0));
    }
    painter->restore();
}

void CanvasView::drawForeground(QPainter *painter, const QRectF &)
{
    if (!hasImage() || m_selection.isEmpty())
        return;
    const QRectF sel(m_selection);
    if (m_tool == Tool::Crop) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 150));
        const QRectF img(imageRect());
        painter->drawRect(QRectF(img.left(), img.top(), img.width(), sel.top() - img.top()));
        painter->drawRect(QRectF(img.left(), sel.bottom(), img.width(), img.bottom() - sel.bottom()));
        painter->drawRect(QRectF(img.left(), sel.top(), sel.left() - img.left(), sel.height()));
        painter->drawRect(QRectF(sel.right(), sel.top(), img.right() - sel.right(), sel.height()));

        QPen thirds(QColor(255, 255, 255, 90), 1);
        thirds.setCosmetic(true);
        painter->setPen(thirds);
        for (int i = 1; i <= 2; ++i) {
            painter->drawLine(QPointF(sel.left() + sel.width() * i / 3, sel.top()),
                              QPointF(sel.left() + sel.width() * i / 3, sel.bottom()));
            painter->drawLine(QPointF(sel.left(), sel.top() + sel.height() * i / 3),
                              QPointF(sel.right(), sel.top() + sel.height() * i / 3));
        }
    }
    painter->setBrush(Qt::NoBrush);
    QPen black(Qt::black, 1);
    black.setCosmetic(true);
    painter->setPen(black);
    painter->drawRect(sel);
    QPen white(Qt::white, 1, Qt::CustomDashLine);
    white.setDashPattern({4, 4});
    white.setCosmetic(true);
    painter->setPen(white);
    painter->drawRect(sel);
}

void CanvasView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing, true);
    if (!hasImage()) {
        p.setPen(QColor("#6d6d6d"));
        QFont f = font();
        f.setPointSize(13);
        p.setFont(f);
        p.drawText(viewport()->rect(), Qt::AlignCenter,
                   tr("Drop an image here\nor press Ctrl+O to open one\n\nPNG · JPG · BMP"));
    }
    if (m_dropHover) {
        p.setPen(QPen(QColor("#007acc"), 3, Qt::DashLine));
        p.setBrush(QColor(0, 122, 204, 28));
        p.drawRoundedRect(QRectF(viewport()->rect()).adjusted(6, 6, -6, -6), 8, 8);
    }
}

// ------------------------------------------------------------ drag & drop

void CanvasView::dragEnterEvent(QDragEnterEvent *event)
{
    if (!imagePathsFromMime(event->mimeData()).isEmpty()) {
        event->acceptProposedAction();
        m_dropHover = true;
        viewport()->update();
    } else {
        event->ignore();
    }
}

void CanvasView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!imagePathsFromMime(event->mimeData()).isEmpty())
        event->acceptProposedAction();
    else
        event->ignore();
}

void CanvasView::dragLeaveEvent(QDragLeaveEvent *event)
{
    m_dropHover = false;
    viewport()->update();
    event->accept();
}

void CanvasView::dropEvent(QDropEvent *event)
{
    m_dropHover = false;
    viewport()->update();
    const QStringList paths = imagePathsFromMime(event->mimeData());
    if (paths.isEmpty()) {
        event->ignore();
        return;
    }
    event->acceptProposedAction();
    emit filesDropped(paths);
}
