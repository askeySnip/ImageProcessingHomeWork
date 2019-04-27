#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QDebug>
#include <QStandardPaths>
#include <QTranslator>

#include "histogram.h"
#include "algor.h"

#define WINDOW_TITLE    "Image Processing"
#define WINDOW_CRITICAL "Error - Image Processing"
#define WINDOW_WARNING  "Notice - Image Processing"
#define WINDOW_ABOUT    "About - Image Processing"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateRightImage(QPixmap &pixmap);
    void cleanImage();

    void setActionStatus(bool);
    void createToolBar();
    void createAction();

private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionHistogram_triggered();

    void on_actionAdjust_triggered();

    void on_actionSave_triggered();

    void on_actionSave_AS_triggered();

    void on_actionRestore_triggered();

    void on_actionAddGaussNoise_triggered();

    void on_action_ArithmeticMeanFilter_triggered();

    void on_actionGeometricMeanFilter_triggered();

    void on_actionAdaptiveFilter_triggered();

    void on_actionTransGray_triggered();

    void on_actionAddSaltNoise_triggered();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *leftScene;
    QGraphicsScene *rightScene;
    QGraphicsPixmapItem *leftPixmapItem;
    QGraphicsPixmapItem *rightPixmapItem;

    QLabel *size;

    QFileInfo *info;

    QString getUserName();
    QString getUserPath();
    cv::Mat leftMat;
    cv::Mat rightMat;
};

#endif // MAINWINDOW_H
