#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void CudaCompute(int width, int height, double minX, double minY, double scaleX, double scaleY,
                     int offsetX, int offsetY, int maxIteration);
    QColor getColor(int iteration, int maxIteration);
    bool isDragging;
    QPoint lastMousePosition;
    double offsetX, offsetY;
    double minX, maxX, minY, maxY;

    double zoomFactor;
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
