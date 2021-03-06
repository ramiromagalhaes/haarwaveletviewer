#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <sstream>

#include <QString>
#include <QFileDialog>
#include <QImage>
#include <QListWidgetItem>

#include "haarwavelet.h"
#include "haarwaveletutilities.h"

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
    QFileDialog dialog(this, tr("Choose haar wavelet file to open"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    if(!dialog.exec()) {
        return;
    }

    closeFile(); //ensures that the old file is closed prior to opening another one



    QStringList files = dialog.selectedFiles();
    QString filepath = files[0];



    int waveletIndex = 0;
    loadHaarWavelets(&sampleSize, filepath.toAscii().constData(), loadedWavelets);
    for (std::vector<HaarWavelet *>::const_iterator it = loadedWavelets.begin(); it != loadedWavelets.end(); ++it)
    {
        std::stringstream ss;
        (*it)->write(ss);

        //setting the view on the screen
        QListWidgetItem * item = new QListWidgetItem(ss.str().c_str(), ui->waveletList);
        item->setData(Qt::UserRole, waveletIndex);
    }
    listLoaded = true;


    {
        QString message;
        message.append("Opened file ");
        message.append(filepath);
        message.append(". Loaded ");
        message.append(QString::number(loadedWavelets.size()));
        message.append(" haar wavelets.");

        ui->statusBar->showMessage(message);
    }
}

void MainWindow::closeFile()
{
    listLoaded = false;
    selectedImageIndex = -1;

    std::vector<HaarWavelet *>::iterator it = loadedWavelets.begin();
    const std::vector<HaarWavelet *>::iterator end = loadedWavelets.end();
    for(; it != end; ++it) {
        HaarWavelet * h = *it;
        delete h;
    }

    loadedWavelets.clear();
    ui->waveletList->clear();
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
    ui->waveletImage->update();
}
