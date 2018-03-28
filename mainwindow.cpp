#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>
#include <vector>
#include <opencv2/features2d.hpp>
using namespace std;
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

    if(path.isEmpty())
        path = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),path,tr("Image Files [ *.jpg , *.jpeg , *.bmp , *.png , *.gif]"));
    QDir dir;
    path = dir.absoluteFilePath(fileName);
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
    (*p_stream1)>>img;

    processFrame(img);
    displayMat(img);

}
using namespace cv;
cv::Mat MainWindow::segmentation(cv::Mat src, cv::Mat color)
{

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( src, contours, hierarchy,
                  CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    if(contours.size()==0) return color;


    int idx = 0;
    cv::Mat res;

    cv::cvtColor(src,res, CV_GRAY2BGR);

    for( ; idx >= 0; idx = hierarchy[idx][0] )
    {

        Scalar color( 127+rand()%127, 127+rand()%127, 127+rand()%127 );

        drawContours( res, contours, idx, color, CV_FILLED, 8, hierarchy );
    }


    cv::Mat mask;
    cv::cvtColor(res,mask,CV_BGR2GRAY);
    cv::threshold(mask,mask,1,255,CV_THRESH_BINARY_INV);
    cv::cvtColor(mask,mask,CV_GRAY2BGR);
    cv::bitwise_and(mask,color,color);
    cv::bitwise_or(res,color,res);

    return res;

}

void MainWindow::processFrame(cv::Mat &processFrame)
{
    if(processFrame.empty())
        return;
    if(ui->checkBox_showOriginal->isChecked()) return;
    int L1 = ui->horizontalSlider_L1->value();
    int L1max = ui->horizontalSlider_L1max->value();
    int L2 = ui->horizontalSlider_L2->value();
    int L2max = ui->horizontalSlider_L2max->value();
    int L3 = ui->horizontalSlider_L3->value();
    int L3max = ui->horizontalSlider_L3_max->value();
    int kernelSize = ui->spinBox_kernelSize->value();
    int blur_size = ui->spinBox_blur->value();

    cv::Mat in = processFrame, out=processFrame;



    cv::Mat res;
    cv::inRange(in,cv::Scalar(L1,L2,L3), cv::Scalar(L1max,L2max,L3max),res);

    //tworzenie kernela
    cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size( 2*kernelSize + 1, 2*kernelSize+1 ), cv::Point( kernelSize, kernelSize ) );

    //erozja
    cv::morphologyEx(res, res,cv::MORPH_OPEN,element);
    cv::morphologyEx(res, res,cv::MORPH_CLOSE,element);

    if(ui->checkBox_detector->isChecked())
    {
        detect(res);
    }

    if(ui->checkBox_segmentationOn->isChecked())
        out = segmentation(res,in);
    else
        cv::cvtColor(res, out, CV_GRAY2BGR);

    if(ui->checkBox_detector->isChecked())
    {
        drawKeypoints( out, keypoints, out, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    }
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

void MainWindow::detect(Mat image)
{
    // Setup SimpleBlobDetector parameters.
    cv::SimpleBlobDetector::Params params;
    bitwise_not ( image, image );
    // Change thresholds
    params.minThreshold = 50;
    params.maxThreshold = 255;

    // Filter by Area.
    params.filterByArea = ui->checkBox_Area->isChecked();
    params.minArea = ui->horizontalSlider_area->value()*100;

    // Filter by Circularity
    params.filterByCircularity = false;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = false;
    params.minConvexity = 0.0;

    // Filter by Inertia
    params.filterByInertia = ui->checkBox_inertia->isChecked();
    params.minInertiaRatio = ui->horizontalSlider_inertia->value()/10.0;



    // Set up detector with params
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

    // SimpleBlobDetector::create creates a smart pointer.
    // So you need to use arrow ( ->) instead of dot ( . )
    //std::vector<KeyPoint> keypoints;
    detector->detect( image, keypoints);
    bitwise_not ( image, image );

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
