#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QModelIndex>
#include <QModelIndexList>

class HaarWavelet;
class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFile();
    void closeFile();
    void waveletSelected(QListWidgetItem *item, QListWidgetItem *);

private:
    void drawWavelet();

    Ui::MainWindow *ui;

    const cv::Point position; //always like that during SRFS production
    const cv::Size sampleSize; //size in pixels of the trainning images
    const cv::Scalar white;
    const cv::Scalar black;
    const cv::Mat cleanView;

    bool listLoaded;
    int selectedImageIndex;
    std::vector<HaarWavelet *> loadedWavelets;
};

#endif // MAINWINDOW_H
