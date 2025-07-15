#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QVector<QVector<QPointF>> contours;  // все контуры
    QVector<bool> closedFlags;           // флаги замкнутости контуров
    QPointF mousePos;                    // текущая позиция мыши

    bool isCurrentContourClosed() const;
    QVector<QPointF>& currentContour();
};

#endif // MAINWINDOW_H
