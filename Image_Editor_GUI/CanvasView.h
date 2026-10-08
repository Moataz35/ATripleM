#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QMimeData>
#include <QRect>
#include <QStringList>

#include "Widgets.h" // Tool enum

class ImageItem;

/// QGraphicsView based canvas: pan / zoom / drag&drop / selection / brush painting.
class CanvasView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CanvasView(QWidget *parent = nullptr);
    ~CanvasView() override;

    /// Local image files (png, jpg, jpeg, bmp, tga) contained in mime data.
    static QStringList imagePathsFromMime(const QMimeData *mime);

    /// Shows `image`. If `resetView` the image is fitted to the window and the selection cleared.
    void setImage(const QImage &image, bool resetView);
    void clearImage();
    QImage image() const;
    bool hasImage() const;

    /// Temporary (e.g. low resolution) preview drawn in place of the real image.
    void setPreviewImage(const QImage &preview);
    void clearPreview();

    void setTool(Tool tool);
    Tool tool() const { return m_tool; }
    void setBrushSize(int px);
    void setBrushOpacity(int percent);
    void setForegroundColor(const QColor &c);
    void setBackgroundColor(const QColor &c);
    void setPaintingEnabled(bool enabled);

    double zoom() const { return m_zoom; }
    bool isFitMode() const { return m_fit; }

    QRect selection() const { return m_selection; }
    void setSelection(const QRect &r);
    void clearSelection();
    void selectAll();

public slots:
    void zoomIn();
    void zoomOut();
    void setZoom(double factor);
    void fitToScreen();
    void actualSize();

signals:
    void zoomChanged(double factor);
    void cursorPosChanged(const QPoint &imagePos, bool inside);
    void filesDropped(const QStringList &paths);
    void strokeFinished(const QImage &result, const QString &name);
    void colorPicked(const QColor &color, bool background);
    void selectionChanged(const QRect &rect);
    void cropRequested(const QRect &rect);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void updateSceneRect();
    void applyZoom(double z, const QPoint &anchorViewPos);
    void doFit();
    void updateCursor();
    QPointF toImage(const QPoint &viewPos) const;
    QRect imageRect() const;
    QRect rectFromDrag(const QPointF &a, const QPointF &b) const;
    void beginStroke(const QPointF &p);
    void continueStroke(const QPointF &p);
    void endStroke();
    void drawSegment(const QPointF &a, const QPointF &b);
    void pickColor(const QPoint &viewPos, bool background);

    QGraphicsScene m_scene;
    ImageItem *m_item = nullptr;

    Tool m_tool = Tool::Hand;
    double m_zoom = 1.0;
    bool m_fit = true;
    bool m_space = false;
    bool m_panning = false;
    QPoint m_lastPan;
    bool m_dropHover = false;
    bool m_paintingEnabled = true;

    QRect m_selection;
    bool m_selecting = false;
    QPointF m_selStart;

    int m_brushSize = 12;
    int m_brushOpacity = 100;
    QColor m_fg{0, 0, 0};
    QColor m_bg{255, 255, 255};
    bool m_stroking = false;
    QPointF m_lastStrokePt;
    QImage m_strokeBase;
    QImage m_strokeLayer;
};

#endif // CANVASVIEW_H
