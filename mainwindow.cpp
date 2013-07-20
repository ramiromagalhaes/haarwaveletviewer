#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QFileDialog>
#include <QImage>
#include <QListWidgetItem>


#include "../ecrsgen/lib/haarwavelet.h"

#define PIXEL_VIEW_SIZE 15
#define WAVELET_PADDING 2
#define SAMPLE_SIZE 20

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          position(0, 0),
                                          sampleSize(SAMPLE_SIZE, SAMPLE_SIZE),
                                          white(255, 255, 255),
                                          black(128, 128, 128),
                                          cleanView(cv::Mat::zeros(
                                                        sampleSize.height * PIXEL_VIEW_SIZE  + WAVELET_PADDING,
                                                        sampleSize.width * PIXEL_VIEW_SIZE  + WAVELET_PADDING,
                                                        CV_8UC3)), //8 bits, 3 channels
                                          listLoaded(false),
                                          selectedImageIndex(-1)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    //TODO reset all states

    QString filepath = QFileDialog::getOpenFileName(
                this, tr("Choose haar wavelet file to open"));

    cv::Point position(0,0); //always like that during SRFS production
    cv::Size sampleSize(SAMPLE_SIZE, SAMPLE_SIZE); //size in pixels of the trainning images

    std::ifstream ifs;
    ifs.open(filepath.toAscii().constData(), std::ifstream::in);

    int waveletIndex = 0;
    while(!ifs.eof())
    {
        HaarWavelet * wavelet = new HaarWavelet(&sampleSize, &position, ifs);
        loadedWavelets.push_back(wavelet);//real data (is it necessary?)

        //list model
        QListWidgetItem * item = new QListWidgetItem(QString::number(waveletIndex), ui->waveletList);
        item->setData(Qt::UserRole, waveletIndex);

        waveletIndex++;
    }

    ifs.close();

    listLoaded = true;

    {
        QString message;
        message.append("Opened file ");
        message.append(filepath);
        message.append(". Loaded ");
        message.append(QString::number(waveletIndex));
        message.append(" haar wavelets.");

        ui->statusBar->showMessage(message);
    }
}

void MainWindow::closeFile()
{
    while(!loadedWavelets.empty())
    {
        delete loadedWavelets.back();
        loadedWavelets.pop_back();
    }

    loadedWavelets.clear();

    listLoaded = false;
    selectedImageIndex = -1;

    //TODO clean the list
}

void MainWindow::waveletSelected(QListWidgetItem *item, QListWidgetItem *)
{
    if (!listLoaded)
    {
        return;
    }

    selectedImageIndex = item->data(Qt::UserRole).toInt();
    drawWavelet();
}

void MainWindow::drawWavelet()
{
    HaarWavelet * wavelet = loadedWavelets[selectedImageIndex];

    cv::Mat image = cleanView.clone();

    std::vector<cv::Rect>::const_iterator itRects = wavelet->rects_begin();
    const std::vector<cv::Rect>::const_iterator endRects = wavelet->rects_end();
    std::vector<float>::const_iterator itWeights = wavelet->weights_begin();
    //const std::vector<float>::const_iterator endWeights = wavelet.weights_end();

    for (; itRects != endRects; ++itRects, ++itWeights)
    {
        cv::Rect r = *itRects;
        cv::Point p1(r.x * PIXEL_VIEW_SIZE + WAVELET_PADDING/2            , r.y * PIXEL_VIEW_SIZE + WAVELET_PADDING/2);
        cv::Point p2((r.x + r.width) * PIXEL_VIEW_SIZE + WAVELET_PADDING/2, (r.y + r.height) * PIXEL_VIEW_SIZE + WAVELET_PADDING/2);

        if (*itWeights > 0)
        {
            cv::rectangle(image, p1, p2, white, CV_FILLED);
        }
        else
        {
            cv::rectangle(image, p1, p2, black, CV_FILLED);
        }
    }

    QImage qImage = QImage((const uchar*)(image.data),
                           image.cols,
                           image.rows,
                           image.step,
                           QImage::Format_RGB888);
    ui->waveletImage->setPixmap(QPixmap::fromImage(qImage));
    //ui->waveletImage->resize(ui->waveletImage->pixmap()->size());
    ui->waveletImage->update();
}
