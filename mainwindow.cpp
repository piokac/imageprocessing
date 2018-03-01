#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>
//using namespace cv;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOtworz,SIGNAL(triggered(bool)),this,SLOT(open()));
    connect(ui->actionStart,SIGNAL(triggered(bool)),this,SLOT(startWebcam()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(stopWebcam()));
    connect(ui->actionOdswiez,SIGNAL(triggered(bool)),this,SLOT(refresh()));
    connect(ui->actionZapisz_obraz,SIGNAL(triggered(bool)),this,SLOT(save()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{

    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),QDir::currentPath(),tr("Image Files [ *.jpg , *.jpeg , *.bmp , *.png , *.gif]"));

    filename = fileName.toLocal8Bit().data();

    refresh();

}

void MainWindow::save()
{
    if(img.empty()) return;
    QString filename = "screenshot"+QDateTime::currentDateTime().toString("dd-MM-yyyThh-mm-ss")+".jpg";
    imwrite( filename.toLatin1().data(), img );
}

void MainWindow::startWebcam()
{
    p_stream1 = new cv::VideoCapture (0); //0 - pierwsza kamera, gdy jest wiecej kamer mozna uzywac kolejnych numerow

    timer_ = startTimer(100);
}

void MainWindow::stopWebcam()
{
    killTimer(timer_);
    delete p_stream1;
}

void MainWindow::timerEvent(QTimerEvent *event)
{

    cv::Mat cameraFrame;
    if(!p_stream1->isOpened()) return;
    (*p_stream1)>>img;

    processFrame(img);
    displayMat(img);

}

void MainWindow::processFrame(cv::Mat &processFrame)
{
    if(processFrame.empty())
        return;
    if(ui->checkBox_showOriginal->isChecked()) return;
    int L1 = ui->horizontalSlider_L1->value();
    cv::Mat in = processFrame, out=processFrame;

    // thresholding on grey
    //    cv::Mat greyMat;
    //    cv::cvtColor(in, greyMat, CV_BGR2GRAY);
    //    cv::Mat res;
    //    cv::threshold( greyMat, res, L1, 255,cv::THRESH_BINARY );

    //    cv::cvtColor(res, out, CV_GRAY2BGR);

    processFrame = out; // musi być BGR

}

void MainWindow::refresh()
{
    if(!filename.empty())
    {
        img = imread(filename);
        if(img.empty()) return;
        processFrame(img);
        displayMat(img);
    }
}

void MainWindow::displayMat(cv::Mat& image)
{
    if(image.empty())
        return;
    QImage qimg(image.cols, image.rows, QImage::Format_RGB888);
    memcpy(qimg.bits(),image.data,image.cols*image.rows*3);
    qimg = qimg.rgbSwapped();

    QPixmap item = QPixmap::fromImage(qimg);
    ui->image_area->setPixmap(item);
}
