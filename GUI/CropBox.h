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

class CropBox : public QGraphicsRectItem
{
public:
    explicit CropBox(const QRectF &bounds, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(bounds, parent), imageBounds(bounds)
    {
        setFlags(QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);

        setPen(QPen(Qt::white, 2, Qt::DashLine));
        setBrush(QBrush(QColor(255, 255, 255, 76)));
    }

    void changeBound(const QRectF& newBounds) {
        imageBounds = newBounds;
    }

protected:
    
    qreal getEffectiveGrabArea() const {
        QRectF br = rect();
        
        return std::max(5.0, std::min(15.0, std::min(br.width(), br.height()) / 3.0));
    }

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override {
        QRectF br = rect();
        QPointF pos = event->pos();
        qreal gArea = getEffectiveGrabArea();

        QRectF handle(br.right() - gArea, br.bottom() - gArea, gArea, gArea);

        if (handle.contains(pos)) {
            setCursor(Qt::SizeFDiagCursor);
        }
        else if (br.contains(pos)) {
            setCursor(Qt::OpenHandCursor);
        }
        else {
            setCursor(Qt::ArrowCursor);
        }

        QGraphicsRectItem::hoverMoveEvent(event);
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            startPos = event->scenePos();
            startRect = rect();

            QRectF br = rect();
            qreal gArea = getEffectiveGrabArea();
            QRectF handle(br.right() - gArea, br.bottom() - gArea, gArea, gArea);
            
            isResizing = handle.contains(event->pos());
            isMoving = !isResizing && br.contains(event->pos());

            if(isResizing){
                setCursor(Qt::SizeFDiagCursor);
            }
            if(isMoving){
                setCursor(Qt::ClosedHandCursor);
            }

            event->accept();
        } else {
            QGraphicsRectItem::mousePressEvent(event);
        }
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
    {
        QPointF currentPos = event->scenePos();
        QPointF delta = currentPos - startPos;

        
        qreal minAllowedSize = std::min({10.0, imageBounds.width(), imageBounds.height()});

        if (isResizing) {
            qreal newWidth = std::max(minAllowedSize, startRect.width() + delta.x());
            qreal newHeight = std::max(minAllowedSize, startRect.height() + delta.y());

            
            qreal maxWidth = imageBounds.right() - startRect.x();
            qreal maxHeight = imageBounds.bottom() - startRect.y();

            newWidth = std::min(newWidth, maxWidth);
            newHeight = std::min(newHeight, maxHeight);

            setRect(startRect.x(), startRect.y(), newWidth, newHeight);
            event->accept();
        }
        else if (isMoving) {
            QRectF newRect = startRect.translated(delta);

            
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
        if(event->button() == Qt::LeftButton){
            isResizing = false;
            isMoving = false;

            QRectF br = rect();
            qreal gArea = getEffectiveGrabArea();
            QRectF handle(br.right() - gArea, br.bottom() - gArea, gArea, gArea);

            if(handle.contains(event->pos())){
                setCursor(Qt::SizeFDiagCursor);
            }
            else if(br.contains(event->pos())){
                setCursor(Qt::OpenHandCursor);
            }
            else{
                setCursor(Qt::ArrowCursor);
            }
            event->accept();
        }
        QGraphicsRectItem::mouseReleaseEvent(event);
    }

private:
    QRectF imageBounds;
    bool isResizing = false;
    bool isMoving = false;
    QPointF startPos;
    QRectF startRect;
};

#endif // CROPBOX_H
