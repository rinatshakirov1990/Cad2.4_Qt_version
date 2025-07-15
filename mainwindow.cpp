#include "mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygonF>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMouseTracking(true);
    contours.append(QVector<QPointF>());
    closedFlags.append(false);
}

bool MainWindow::isCurrentContourClosed() const
{
    return !closedFlags.isEmpty() && closedFlags.last();
}

QVector<QPointF>& MainWindow::currentContour()
{
    return contours.last();
}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int gridStep = 40;
    const int slatHeight = 10;
    const int maxSlatLength = 3000;

    for (int idx = 0; idx < contours.size(); ++idx) {
        const QVector<QPointF> &points = contours[idx];
        bool closed = closedFlags[idx];

        // Рисуем линии полигона
        if (points.size() > 1) {
            painter.setPen(Qt::black);
            for (int i = 0; i < points.size() - 1; ++i) {
                painter.drawLine(points[i], points[i + 1]);
            }
            if (closed) {
                painter.drawLine(points.last(), points.first());
            }
        }

        // Отрисовка реек, если контур замкнут
        if (closed && points.size() > 2) {
            QPolygonF polygon(points);
            QRectF boundingRect = polygon.boundingRect();

            painter.save();
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(200, 200, 255));

            for (qreal y = boundingRect.top(); y <= boundingRect.bottom(); y += gridStep) {
                QVector<QPointF> insidePoints;
                for (qreal x = boundingRect.left(); x <= boundingRect.right(); x += 1) {
                    QPointF pt(x, y);
                    if (polygon.containsPoint(pt, Qt::OddEvenFill)) {
                        insidePoints.append(pt);
                    }
                }

                QVector<QLineF> horizontalSegments;
                bool inside = false;
                QPointF start;
                for (int i = 0; i < insidePoints.size(); ++i) {
                    if (!inside) {
                        start = insidePoints[i];
                        inside = true;
                    }
                    bool isEnd = (i == insidePoints.size() - 1) ||
                                 (insidePoints[i + 1].x() - insidePoints[i].x() > 1);
                    if (inside && isEnd) {
                        QPointF end = insidePoints[i];
                        horizontalSegments.append(QLineF(start, end));
                        inside = false;
                    }
                }

                for (const QLineF &seg : horizontalSegments) {
                    qreal xStart = seg.p1().x();
                    qreal xEnd = seg.p2().x();

                    for (qreal x = xStart; x < xEnd; x += maxSlatLength) {
                        qreal length = std::min(static_cast<qreal>(maxSlatLength), xEnd - x);

                        QRectF slatRect(x, y, length, slatHeight);
                        painter.drawRect(slatRect);
                    }
                }
            }

            painter.restore();
        }

        // Точки контура
        painter.setBrush(Qt::red);
        painter.setPen(Qt::black);
        for (const QPointF &pt : points) {
            painter.drawEllipse(pt, 4, 4);
        }
    }

    // Текущая позиция мыши — синий крестик
    painter.setPen(Qt::blue);
    int size = 5;
    painter.drawLine(mousePos.x() - size, mousePos.y(), mousePos.x() + size, mousePos.y());
    painter.drawLine(mousePos.x(), mousePos.y() - size, mousePos.x(), mousePos.y() + size);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isCurrentContourClosed()) {
            contours.append(QVector<QPointF>());
            closedFlags.append(false);
        }

        QVector<QPointF> &points = currentContour();
        QPointF clickedPoint = event->pos();

        if (!points.isEmpty()) {
            QPointF first = points.first();
            if (QLineF(first, clickedPoint).length() < 10) {
                closedFlags[closedFlags.size() - 1] = true;
                update();
                return;
            }
        }

        points.append(clickedPoint);
        update();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = event->pos();
    update();
}
