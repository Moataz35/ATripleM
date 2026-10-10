#ifndef CROPBOX_H
#define CROPBOX_H

#include <QPainter>
#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QRect>
#include <QStyle>
#include <QStyleOption>
#include <algorithm>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QCursor>

// class CropBox : public QWidget
// {
//     Q_OBJECT

// public:
//     explicit CropBox(QWidget *parent)
//         : QWidget(parent)
//     {
//         setMouseTracking(true);

//         toGrabFrom = {
//             width() - grabArea,
//             height() - grabArea,
//             grabArea,
//             grabArea
//         };

//         this->setStyleSheet(R"(
//             CropBox {
//                 background-color: rgba(255, 255, 255, 0.3);
//                 border: 2px dashed white;
//             }
//         )");

//         setGeometry(parentWidget()->geometry());

//     }

// protected:
//     void mousePressEvent(QMouseEvent *event) override
//     {
//         if (event->button() == Qt::LeftButton) {
//             toGrabFrom = {
//                 width() - grabArea,
//                 height() - grabArea,
//                 grabArea,
//                 grabArea
//             };

//             QPoint mouseLocalPosition = event->position().toPoint(); // relative to the widget

//             if (toGrabFrom.contains(mouseLocalPosition)) {
//                 isResizing = true;
//                 startPosition = event->globalPosition().toPoint();
//                 startGeometry = geometry();
//             }
//             else if (this->rect().contains(mouseLocalPosition)) {
//                 isMoving = true;
//                 startPosition = event->globalPosition().toPoint();
//                 fromPointToBegin = event->position().toPoint();
//                 startGeometry = geometry();
//             }

//         }
//     }

//     void mouseMoveEvent(QMouseEvent *event) override
//     {
//         QPoint mouseLocalPosition = event->position().toPoint();
//         if (toGrabFrom.contains(mouseLocalPosition) || isResizing) {
//             setCursor(Qt::SizeFDiagCursor);
//         }
//         else if (isMoving) {
//             setCursor(Qt::ClosedHandCursor);
//         }
//         else {
//             unsetCursor();
//         }

//         //-----------------------------------------------------------
//         //----------------------Moving-------------------------------
//         //-----------------------------------------------------------

//         if (isMoving) {
//             QPoint globalMousePos = event->globalPosition().toPoint();
//             QPoint parentLocalMousePosition = parentWidget()->mapFromGlobal(globalMousePos);
//             QPoint newPosition = parentLocalMousePosition - fromPointToBegin;

//             // Last allowed point for the top left of the crop box is
//             // the right bottom of the boundaries - current size of the crop box
//             QPoint lastAllowedPoint;
//             lastAllowedPoint.setX(parentWidget()->width() - size().width());
//             lastAllowedPoint.setY(parentWidget()->height() - size().height());

//             newPosition.setX(std::min(lastAllowedPoint.x(), newPosition.x()));
//             newPosition.setY(std::min(lastAllowedPoint.y(), newPosition.y()));

//             newPosition.setX(std::max(0, newPosition.x()));
//             newPosition.setY(std::max(0, newPosition.y()));

//             QRect newGeometry = geometry();
//             newGeometry.moveTo(newPosition);
//             setGeometry(newGeometry);
//             return;
//         }

//         //-----------------------------------------------------------
//         //----------------------Resizing-----------------------------
//         //-----------------------------------------------------------

//         if (!isResizing)
//             return;

//         QPoint currentPos = event->globalPosition().toPoint();
//         QPoint delta = currentPos - startPosition;

//         int newWidth = startGeometry.width() + delta.x();
//         int newHeight = startGeometry.height() + delta.y();

//         // boundaries width - the crop box x relative to the boundaries
//         // pos().x() is the x relative to the parent widget
//         int remainWidth = parentWidget()->width() -  pos().x();
//         int remainHeight = parentWidget()->height() - pos().y();

//         newWidth = std::min(newWidth, remainWidth);
//         newHeight = std::min(newHeight, remainHeight);

//         newWidth = std::max(newWidth, 50);
//         newHeight = std::max(newHeight, 50);

//         QRect newGeometry = startGeometry;
//         newGeometry.setWidth(newWidth);
//         newGeometry.setHeight(newHeight);

//         setGeometry(newGeometry);
//     }

//     void mouseReleaseEvent(QMouseEvent *event) override
//     {
//         if (event->button() == Qt::LeftButton)
//         {
//             isResizing = false;
//             isMoving = false;
//         }
//     }

//     void paintEvent(QPaintEvent *) override
//     {
//         QStyleOption opt;
//         // Take the current size and active stylesheet from the widget
//         opt.initFrom(this);

//         QPainter p(this);
//         style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//     }

// private:
//     bool isResizing = false;
//     QPoint startPosition;
//     QRect startGeometry;
//     QRect toGrabFrom;
//     const int grabArea = 40;

//     bool isMoving = false;
//     QPoint fromPointToBegin;
// };

// Crop Box Relative To QRect Boundaries



class CropBox : public QGraphicsRectItem
{
public:
    explicit CropBox(const QRectF &bounds, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(bounds, parent), imageBounds(bounds)
    {
        setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);

        // Styling the crop box
        setPen(QPen(Qt::white, 2, Qt::DashLine));
        setBrush(QBrush(QColor(255, 255, 255, 76))); // rgba(255, 255, 255, 0.3)
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            startPos = event->scenePos();
            startRect = rect();

            QRectF br = rect();
            QRectF handle(br.right() - grabArea, br.bottom() - grabArea, grabArea, grabArea);
            isResizing = handle.contains(event->pos());
            isMoving = !isResizing && br.contains(event->pos());

            event->accept();
        } else {
            QGraphicsRectItem::mousePressEvent(event);
        }
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
    {
        QPointF currentPos = event->scenePos();
        QPointF delta = currentPos - startPos;

        if (isResizing) {
            qreal newWidth = std::max(50.0, startRect.width() + delta.x());
            qreal newHeight = std::max(50.0, startRect.height() + delta.y());

            // Restrict resizing from exceeding the bottom-right image boundaries
            qreal maxWidth = imageBounds.right() - startRect.x();
            qreal maxHeight = imageBounds.bottom() - startRect.y();

            newWidth = std::min(newWidth, maxWidth);
            newHeight = std::min(newHeight, maxHeight);

            setRect(startRect.x(), startRect.y(), newWidth, newHeight);
            event->accept();
        }
        else if (isMoving) {
            QRectF newRect = startRect.translated(delta);

            // Clamp movement so the box never goes outside imageBounds
            qreal dx = 0;
            qreal dy = 0;

            if (newRect.left() < imageBounds.left()) {
                dx = imageBounds.left() - newRect.left();
            } else if (newRect.right() > imageBounds.right()) {
                dx = imageBounds.right() - newRect.right();
            }

            if (newRect.top() < imageBounds.top()) {
                dy = imageBounds.top() - newRect.top();
            } else if (newRect.bottom() > imageBounds.bottom()) {
                dy = imageBounds.bottom() - newRect.bottom();
            }

            newRect.translate(dx, dy);
            setRect(newRect);
            event->accept();
        }
        else {
            QGraphicsRectItem::mouseMoveEvent(event);
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        isResizing = false;
        isMoving = false;
        QGraphicsRectItem::mouseReleaseEvent(event);
    }

private:
    QRectF imageBounds;
    bool isResizing = false;
    bool isMoving = false;
    QPointF startPos;
    QRectF startRect;
    const qreal grabArea = 20.0;
};

#endif // CROPBOX_H
