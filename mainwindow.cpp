#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    leftScene = new QGraphicsScene;
    rightScene = new QGraphicsScene;

    leftPixmapItem = new QGraphicsPixmapItem();
    rightPixmapItem = new QGraphicsPixmapItem();

    size = new QLabel;

    info = nullptr;

    leftScene->setBackgroundBrush(QColor::fromRgb(224,224,224));
    ui->leftGraphicsView->setScene(leftScene);
    rightScene->setBackgroundBrush(QColor::fromRgb(224,224,224));
    ui->rightGraphicsView->setScene(rightScene);

    ui->statusBar->addPermanentWidget(size);


    createAction();
    createToolBar();


    setActionStatus(false);
    setWindowTitle(WINDOW_TITLE);
}

void MainWindow::createToolBar()
{
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionClose);

    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionRestore);
    ui->toolBar->addAction(ui->actionHistogram);

    ui->toolBar->addSeparator();
}

void MainWindow::createAction()
{

}

MainWindow::~MainWindow()
{
    delete ui;

    if (leftScene)
    {
        delete leftScene;
        leftScene = nullptr;
    }

    if (size)
    {
        delete size;
        size = nullptr;
    }


    //add
    if (rightScene)
    {
        delete leftScene;
        leftScene = nullptr;
    }
}

void MainWindow::cleanImage()
{
    leftScene->clear();
    ui->leftGraphicsView->resetTransform();

    rightScene->clear();
    ui->rightGraphicsView->resetTransform();


    if (size)
    {
        delete size;
        size = new QLabel;
        ui->statusBar->addPermanentWidget(size);
    }



    this->setWindowTitle(WINDOW_TITLE);
    setActionStatus(false);
}

void MainWindow::setActionStatus(bool status)
{
    status = !status;
}

QString MainWindow::getUserName()
{
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString userName = userPath.section("/", -1, -1);
    return userName;
}

QString MainWindow::getUserPath()
{
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return userPath;
}

void MainWindow::on_actionOpen_triggered()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open image"), getUserPath() + "/Pictures",
                                             tr("All Files (*);;"
                                                "All Images (*.bpm *.gif *.jpg *.jpeg *.png *.ppm *.xbm *.xpm);;"
                                                "Image BPM (*.bpm);;"
                                                "Image GIF (*.gif);;"
                                                "Image JPG (*.jpg);;"
                                                "Image JPEG (*.jpeg);;"
                                                "Image PNG (*.png);;"
                                                "Image PPM (*.ppm);;"
                                                "Image XBM (*.xbm);;"
                                                "Image XPM (*.xpm);;"));

    if (!imagePath.isEmpty())
    {
        QFile file(imagePath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr(WINDOW_CRITICAL),
                                  tr("Unable to open image."));
            return;
        }

        // delete previous image
        cleanImage();

        // upload image
        info = new QFileInfo(imagePath);

        QPixmap leftPixmap(imagePath);
        leftPixmapItem = leftScene->addPixmap(leftPixmap);
        leftScene->setSceneRect(QRectF(leftPixmap.rect()));

        QPixmap rightPixmap(imagePath);
        rightPixmapItem = rightScene->addPixmap(rightPixmap);
        rightScene->setSceneRect(QRectF(rightPixmap.rect()));

        qDebug()<<"depth:"<<rightPixmap.depth();
        qDebug()<<"hasAlpha:"<<rightPixmap.hasAlpha();

        // settings
        this->setWindowTitle(info->fileName() + " - " + WINDOW_TITLE);

        setActionStatus(true);

        size->setText(QString::number(leftPixmapItem->pixmap().width())
                      + " x " + QString::number(leftPixmapItem->pixmap().height()));
    }
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionHistogram_triggered()
{
    QDialog * hstgrmDialog = new QDialog(this);
    QScrollArea * scrollArea = new QScrollArea(hstgrmDialog);
    Histogram * hstgrm = new Histogram(scrollArea);
    hstgrm->computeHstgrm(rightPixmapItem->pixmap().toImage());

    if (hstgrm == nullptr)
        return;


    scrollArea->setWidget(hstgrm);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(scrollArea);
    hstgrmDialog->setLayout(layout);

    hstgrm->resize(800, 780);
    hstgrmDialog->setFixedWidth(820);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->adjustSize();

    hstgrmDialog->setWindowTitle("Histogram - ImageQt");

    hstgrmDialog->show();

}

void MainWindow::on_actionAdjust_triggered()
{
    // left
    int height = leftPixmapItem->pixmap().height();
    int width = leftPixmapItem->pixmap().width();
    int max_height = ui->leftGraphicsView->height();
    int max_width = ui->leftGraphicsView->width();
    int size,max_size,fact=0;
    double val=0;


    size = qMin(width,height);
    max_size = qMin(max_width,max_height);


    if (size < max_size) {
        while ((size*val) < max_size)
            val = pow(1.2,fact++);
        val = pow(1.2,fact-2);
        ui->leftGraphicsView->setFactor(fact-2);
    }

    else {
        val = 1;
        while ((size*val) > max_size)
            val = pow(1.2,fact--);
        val = pow(1.2,fact+1);
        ui->leftGraphicsView->setFactor(fact+1);
    }

    ui->leftGraphicsView->scale(val,val);


    // right
    height = leftPixmapItem->pixmap().height();
    width = leftPixmapItem->pixmap().width();
    max_height = ui->rightGraphicsView->height();
    max_width = ui->rightGraphicsView->width();
    size = max_size = fact = 0;
    val=0;


    size = qMin(width,height);
    max_size = qMin(max_width,max_height);


    if (size < max_size) {
        while ((size*val) < max_size)
            val = pow(1.2,fact++);
        val = pow(1.2,fact-2);
        ui->rightGraphicsView->setFactor(fact-2);
    }

    else {
        val = 1;
        while ((size*val) > max_size)
            val = pow(1.2,fact--);
        val = pow(1.2,fact+1);
        ui->rightGraphicsView->setFactor(fact+1);
    }

    ui->rightGraphicsView->scale(val,val);
}
