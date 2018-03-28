#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <opencv2/cv.cpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    cv::Mat segmentation(cv::Mat res, cv::Mat color);
    
public slots:
    void open();
    void save();
    void startWebcam();
    void stopWebcam();
    void timerEvent(QTimerEvent *event);
    void processFrame(cv::Mat& processFrame);
    void refresh();
    void detect(cv::Mat image);
private:
    void displayMat(cv::Mat &image);
    Ui::MainWindow *ui;

    cv::Mat img;
    cv::VideoCapture* p_stream1;
    int timer_;
    cv::String filename;
    std::vector<cv::KeyPoint> keypoints;
    QString path;
};

#endif // MAINWINDOW_H
