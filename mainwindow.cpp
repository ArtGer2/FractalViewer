#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <complex>
#include <chrono>
#include <iostream>
using namespace std::chrono;

extern "C"
    double* CudaExec(int width, int height, double minX, double minY, double scaleX, double scaleY,
                         int offsetX, int offsetY, int maxIteration);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    isDragging(false),
    offsetX(0),
    offsetY(0),
    minX(-2.5),
    maxX(1.5),
    minY(-1.5),
    maxY(1.5),
    zoomFactor(1.0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}
void mandelbrotKernel(int width, int height, double minX, double minY, double maxX, double maxY,
                      int maxIteration, int offsetX, int offsetY, double* output);



void MainWindow::CudaCompute(int width, int height, double minX, double minY, double scaleX, double scaleY,
                             int offsetX, int offsetY, int maxIteration) {

    double* h_output = CudaExec(width, height, minX, minY, scaleX, scaleY,offsetX, offsetY, maxIteration);

    QImage image(width, height, QImage::Format_RGB32);

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            int iteration = h_output[y * width + x];
            QColor color = getColor(iteration, maxIteration);
            image.setPixel(x, y, color.rgb());
        }
    }

    QPainter painter(this);
    painter.drawImage(0, 0, image);

    delete[] h_output;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int width = this->width();
    int height = this->height();
    int maxIteration = 1000; // maxIterations

    double scaleX = (maxX - minX) / width;
    double scaleY = (maxY - minY) / height;
    CudaCompute(width,  height,  minX,  minY,  scaleX,  scaleY,  offsetX,  offsetY,  maxIteration);
}






QColor MainWindow::getColor(int iteration, int maxIteration)
{
    if (iteration == maxIteration) {
        return QColor(0, 0, 0);
    }

    int r = (iteration * 9) % 256;
    int g = (iteration * 7) % 256;
    int b = (iteration * 5) % 256;

    return QColor(r, g, b);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        lastMousePosition = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging) {
        QPoint currentPos = event->pos();
        int dx = currentPos.x() - lastMousePosition.x();
        int dy = currentPos.y() - lastMousePosition.y();

        offsetX -= dx;
        offsetY -= dy;

        lastMousePosition = currentPos;

        update();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    double zoomStep = 0.01;

    if (event->angleDelta().y() > 0) {
        zoomFactor += zoomStep;
        if(zoomFactor<1) zoomFactor=1;
    } else {
        zoomFactor -= zoomStep;
        if(zoomFactor>1) zoomFactor=1;
    }


    QPointF cursorPos = event->position();


    double scaleX = (maxX - minX) / width();
    double scaleY = (maxY - minY) / height();

    double imageX = minX + (cursorPos.x() + offsetX) * scaleX;
    double imageY = minY + (cursorPos.y() + offsetY) * scaleY;

    double newMinX = imageX - (cursorPos.x() / width()) * (maxX - minX) / zoomFactor;
    double newMaxX = imageX + (width() - cursorPos.x()) / width() * (maxX - minX) / zoomFactor;
    double newMinY = imageY - (cursorPos.y() / height()) * (maxY - minY) / zoomFactor;
    double newMaxY = imageY + (height() - cursorPos.y()) / height() * (maxY - minY) / zoomFactor;

    minX = newMinX;
    maxX = newMaxX;
    minY = newMinY;
    maxY = newMaxY;

    update();
}
