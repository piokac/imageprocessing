#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QDir>
#include <QFileDialog>
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

void MainWindow::startWebcam()
{
    p_stream1 = new cv::VideoCapture (0);

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
    (*p_stream1)>>cameraFrame;

    processFrame(cameraFrame);
    displayMat(cameraFrame);

}

void MainWindow::processFrame(cv::Mat &processFrame)
{
    if(processFrame.empty())
        return;
    int L1 = ui->horizontalSlider_L1->value();
    cv::Mat in = processFrame, out;
#if 0
    // thresholding on grey
    cv::Mat greyMat;
    cv::cvtColor(in, greyMat, CV_BGR2GRAY);
    cv::Mat res;
    cv::threshold( greyMat, res, L1, 255,cv::THRESH_BINARY );
    cv::cvtColor(res, out, CV_GRAY2BGR);
#endif
#if 0
    //thresholding on RGB
    int low_b=0,low_g=0, low_r = L1, high_b = 255, high_g = 255, high_r = 255;
    cv::Mat greyMat;
    cv::inRange(in,cv::Scalar(low_b,low_g,low_r), cv::Scalar(high_b,high_g,high_r),greyMat);

    cv::cvtColor(greyMat, out, CV_GRAY2BGR);
#endif

#if 1
    //thresholding on HSV
    cv::Mat hsvMat;
    cv::cvtColor(in, hsvMat, CV_BGR2HSV);
    int low_h=L1,low_s=50, low_v = 50, high_h = 180, high_s = 255, high_v= 255;
    cv::Mat greyMat;
    cv::inRange(hsvMat,cv::Scalar(low_h,low_s,low_v), cv::Scalar(high_h,high_s,high_v),greyMat);

    cv::cvtColor(greyMat, out, CV_GRAY2BGR);
#endif
    //imshow("Video Capture",out);
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
