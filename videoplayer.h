#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QObject>
#include <QFileDialog>
#include <QAudioDeviceInfo>
#include <QVideoWidget>
#include <QSlider>
#include <QFileDialog>
#include <QPushButton>
#include <QPixmap>
#include <QTimer>

#include "the_player.h"
#include "the_button.h"
#include "stylesheeteditor.h"
#include "playsetting.h"

namespace Ui {
class videoPlayer;
}

class videoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit videoPlayer(QWidget *parent = nullptr,std::vector<TheButtonInfo> videolist= std::vector<TheButtonInfo>());
    ~videoPlayer();

     std::vector<TheButtonInfo> videos;

     QString qmPath;
     QTranslator translator;

     QString formatTime(qint64 timeInMs);
     std::vector<TheButtonInfo> getInfoIn (std::string loc);

     StyleSheetEditor *mStyleSheetEditor; //ui



 private slots:
     void on_btn_list_clicked();

 private:
    Ui::videoPlayer *ui;

    QVideoWidget *videoWidget;
    std::vector<TheButton*> buttons;
    QMediaPlaylist *playlist = nullptr;
    ThePlayer *player;
    playSetting *setWidget;
    QListWidget *playlistWidget;


};

#endif // VIDEOPLAYER_H
