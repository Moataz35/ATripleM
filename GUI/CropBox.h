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

class CropBox : public QWidget
{
    Q_OBJECT

public:
    explicit CropBox(QWidget *parent)
        : QWidget(parent)
    {
        setMouseTracking(true);

        toGrabFrom = {
            width() - grabArea,
            height() - grabArea,
            grabArea,
            grabArea
        };

        this->setStyleSheet(R"(
            CropBox {
                background-color: rgba(255, 255, 255, 0.3);
                border: 2px dashed white;
            }
        )");

        setGeometry(parentWidget()->geometry());

    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            toGrabFrom = {
                width() - grabArea,
                height() - grabArea,
                grabArea,
                grabArea
            };

            QPoint mouseLocalPosition = event->position().toPoint(); // relative to the widget

            if (toGrabFrom.contains(mouseLocalPosition)) {
                isResizing = true;
                startPosition = event->globalPosition().toPoint();
                startGeometry = geometry();
            }
            else if (this->rect().contains(mouseLocalPosition)) {
                isMoving = true;
                startPosition = event->globalPosition().toPoint();
                fromPointToBegin = event->position().toPoint();
                startGeometry = geometry();
            }

        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QPoint mouseLocalPosition = event->position().toPoint();
        if (toGrabFrom.contains(mouseLocalPosition) || isResizing) {
            setCursor(Qt::SizeFDiagCursor);
        }
        else if (isMoving) {
            setCursor(Qt::ClosedHandCursor);
        }
        else {
            unsetCursor();
        }

        //-----------------------------------------------------------
        //----------------------Moving-------------------------------
        //-----------------------------------------------------------

        if (isMoving) {
            QPoint globalMousePos = event->globalPosition().toPoint();
            QPoint parentLocalMousePosition = parentWidget()->mapFromGlobal(globalMousePos);
            QPoint newPosition = parentLocalMousePosition - fromPointToBegin;

            // Last allowed point for the top left of the crop box is
            // the right bottom of the boundaries - current size of the crop box
            QPoint lastAllowedPoint;
            lastAllowedPoint.setX(parentWidget()->width() - size().width());
            lastAllowedPoint.setY(parentWidget()->height() - size().height());

            newPosition.setX(std::min(lastAllowedPoint.x(), newPosition.x()));
            newPosition.setY(std::min(lastAllowedPoint.y(), newPosition.y()));

            newPosition.setX(std::max(0, newPosition.x()));
            newPosition.setY(std::max(0, newPosition.y()));

            QRect newGeometry = geometry();
            newGeometry.moveTo(newPosition);
            setGeometry(newGeometry);
            return;
        }

        //-----------------------------------------------------------
        //----------------------Resizing-----------------------------
        //-----------------------------------------------------------

        if (!isResizing)
            return;

        QPoint currentPos = event->globalPosition().toPoint();
        QPoint delta = currentPos - startPosition;

        int newWidth = startGeometry.width() + delta.x();
        int newHeight = startGeometry.height() + delta.y();

        // boundaries width - the crop box x relative to the boundaries
        // pos().x() is the x relative to the parent widget
        int remainWidth = parentWidget()->width() -  pos().x();
        int remainHeight = parentWidget()->height() - pos().y();

        newWidth = std::min(newWidth, remainWidth);
        newHeight = std::min(newHeight, remainHeight);

        newWidth = std::max(newWidth, 50);
        newHeight = std::max(newHeight, 50);

        QRect newGeometry = startGeometry;
        newGeometry.setWidth(newWidth);
        newGeometry.setHeight(newHeight);

        setGeometry(newGeometry);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            isResizing = false;
            isMoving = false;
        }
    }

    void paintEvent(QPaintEvent *) override
    {
        QStyleOption opt;
        // Take the current size and active stylesheet from the widget
        opt.initFrom(this);

        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:
    bool isResizing = false;
    QPoint startPosition;
    QRect startGeometry;
    QRect toGrabFrom;
    const int grabArea = 40;

    bool isMoving = false;
    QPoint fromPointToBegin;
};

// Crop Box Relative To QRect Boundaries

// class CropBox : public QWidget
// {
//     Q_OBJECT

// public:
//     explicit CropBox(QRect boundaries, QWidget *parent = nullptr)
//         : QWidget(parent), boundaries(boundaries)
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

//         setGeometry(boundaries);

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

//         if (isMoving) {
//             QPoint currentPos = event->globalPosition().toPoint();
//             QPoint parentLocalPos = parentWidget() ? parentWidget()->mapFromGlobal(currentPos) : currentPos;
//             QPoint newPosition = parentLocalPos - fromPointToBegin;

//             // Last allowed point for the top left of the crop box is
//             // the right bottom of the boundaries - current size of the crop box
//             QPoint lastAllowedPoint;
//             lastAllowedPoint.setX(boundaries.width() - size().width() + boundaries.x());
//             lastAllowedPoint.setY(boundaries.height() - size().height() + boundaries.y());

//             newPosition.setX(std::min(lastAllowedPoint.x(), newPosition.x()));
//             newPosition.setY(std::min(lastAllowedPoint.y(), newPosition.y()));

//             newPosition.setX(std::max(boundaries.x(), newPosition.x()));
//             newPosition.setY(std::max(boundaries.y(), newPosition.y()));

//             QRect newGeometry = geometry();
//             newGeometry.moveTo(newPosition);
//             setGeometry(newGeometry);
//             return;
//         }

//         if (!isResizing)
//             return;

//         QPoint currentPos = event->globalPosition().toPoint();
//         QPoint delta = currentPos - startPosition;

//         int newWidth = startGeometry.width() + delta.x();
//         int newHeight = startGeometry.height() + delta.y();

//         // boundaries width - the crop box x relative to the boundaries
//         // pos().x() is the x relative to the mainWindow
//         int remainWidth = boundaries.width() -  (pos().x() - boundaries.x());
//         int remainHeight = boundaries.height() - (pos().y() - boundaries.y());

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
//     QRect boundaries;

//     bool isResizing = false;
//     QPoint startPosition;
//     QRect startGeometry;
//     QRect toGrabFrom;
//     const int grabArea = 40;

//     bool isMoving = false;
//     QPoint fromPointToBegin;
// };

#endif // CROPBOX_H
