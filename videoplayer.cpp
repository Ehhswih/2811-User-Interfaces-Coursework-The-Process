#include "videoplayer.h"
#include "ui_videoplayer.h"
//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//




// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> videoPlayer::getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
        if (f.contains(".wmv"))  { // windows
#else
        if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                QImage sprite = imageReader->read(); // read the thumbnail
                if (!sprite.isNull()) {
                    QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                    QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                    out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                }
                else
                    qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}



QString videoPlayer::formatTime(qint64 timeInMs) {
    qint64 seconds = timeInMs / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString::asprintf("%02lld:%02lld", minutes, seconds);
}



videoPlayer::videoPlayer(QWidget *parent ,std::vector<TheButtonInfo> videolist) :
    QWidget(parent),videos(videolist),
    ui(new Ui::videoPlayer)
{
    ui->setupUi(this);

    setWindowTitle("tomeo");

    //setting widget
    setWidget = new playSetting();

    QObject::connect(ui->btn_setting, &QPushButton::clicked, [&](){
        setWidget->move(this->x()+this->width(),this->y());
        setWidget->show();
        setWidget->activateWindow();
    });

    QObject::connect(ui->btn_shot, &QToolButton::clicked, [&]() {
            // Capture the current frame as a QPixmap
            QPoint widgetPos = ui->vWidget->mapToGlobal(QPoint(0, 0));
            QSize widgetSize = ui->vWidget->size();
            QScreen *widgetScreen = QGuiApplication::screenAt(widgetPos);

            if (widgetScreen)
            {

                QPixmap screenshot = widgetScreen->grabWindow(0, widgetPos.x(), widgetPos.y(), widgetSize.width(), widgetSize.height());
                screenshot.save("screenshot.png");


            }
            // Save or process the screenshot as needed
            qDebug() << "Screenshot taken!";

    });



    QObject::connect(setWidget, &playSetting::comboBox_indexChanded,
                     [&](int index){
                         switch(index) {
                         case 0: // Sequential
                             playlist->setPlaybackMode(QMediaPlaylist::Sequential);
                             break;
                         case 1: // Loop
                             playlist->setPlaybackMode(QMediaPlaylist::Sequential);
                             QObject::connect(player, &QMediaPlayer::mediaStatusChanged,
                                              [&](QMediaPlayer::MediaStatus status) {
                                                  if (status == QMediaPlayer::EndOfMedia) {
                                                      player->pause();
                                                      player->setPosition(0);
                                                      player->play();
                                                  }
                                              });
                             break;
                         case 2: // Stop at End
                             playlist->setPlaybackMode(QMediaPlaylist::Sequential);
                             QObject::connect(player, &QMediaPlayer::mediaStatusChanged,
                                              [&](QMediaPlayer::MediaStatus status) {
                                                  if (status == QMediaPlayer::EndOfMedia) {
                                                      player->pause();
                                                      ui->btn_pause->setIcon(QIcon(":/play.png"));
                                                  }
                                              });
                             break;
                         default:
                             break;
                         }
                     });



    this->setBaseSize(250,500);
    this->setWindowIcon(QIcon(":/commit (1).png"));
    videoWidget = new QVideoWidget(ui->vWidget);
    QHBoxLayout* layout = new QHBoxLayout(ui->vWidget);

    // create videoWidget
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);

    layout->addWidget(videoWidget);

    // setting layout
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->vWidget->setLayout(layout);

    player = new ThePlayer;
    player->setVideoOutput(videoWidget);
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    QObject::connect(ui->btn_pause, &QToolButton::clicked, [&]() {
        player->togglePlayPause();
        if (player->state() == QMediaPlayer::PlayingState)
            ui->btn_pause->setIcon(QIcon(":/pause.png"));
        else

            ui->btn_pause->setIcon(QIcon(":/play.png"));
    });



    playlist = player->getPlaylist();
    // setting height
    ui->playlistWidget->hide();

    // Function to update the playlist widget
    auto updatePlaylistWidget = [this]() {
        ui->playlistWidget->clear();
        for (int i = 0; i < playlist->mediaCount(); ++i) {
            QUrl mediaUrl = playlist->media(i).request().url();
            playlistWidget->addItem(mediaUrl.fileName());
        }
    };

    // Initial population of the playlist widget
    updatePlaylistWidget();

    // Connect playlist update signals to the update function
    QObject::connect(playlist, &QMediaPlaylist::mediaInserted, [this]() {
        ui->playlistWidget->clear();
        for (int i = 0; i < playlist->mediaCount(); ++i) {
            QUrl mediaUrl = playlist->media(i).request().url();
            ui->playlistWidget->addItem(mediaUrl.fileName());
        }
    });
        QObject::connect(playlist, &QMediaPlaylist::mediaRemoved, updatePlaylistWidget);
        QObject::connect(playlist, &QMediaPlaylist::mediaChanged, updatePlaylistWidget);

    // Highlight Current Playing Media
    QObject::connect(playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->playlistWidget->setCurrentRow(index);
    });

    // Connect itemClicked Signal
    QObject::connect(ui->playlistWidget, &QListWidget::itemClicked, [this](QListWidgetItem *item){
        int index = ui->playlistWidget->row(item);
        playlist->setCurrentIndex(index);
    });


    QObject::connect(ui->btn_next, &QToolButton::clicked, player, &ThePlayer::nextVideo);
    QObject::connect(ui->btn_prev, &QToolButton::clicked, player, &ThePlayer::previousVideo);

    ui->cbox_speed->setItemData(0, QVariant(0.5)); // For the item "0.5x"
    ui->cbox_speed->setItemData(1, QVariant(1.0)); // For the item "1.0x"
    ui->cbox_speed->setItemData(2, QVariant(1.5)); // For the item "1.5x"
    ui->cbox_speed->setItemData(3, QVariant(2.0)); // For the item "2.0x"

    // Set the default speed (1.0x)
    ui->cbox_speed->setCurrentIndex(1);

    QObject::connect(ui->cbox_speed, QOverload<int>::of(&QComboBox::currentIndexChanged),[this](int index) {

                        qreal speed = ui->cbox_speed->itemData(index).toDouble();
                         player->setPlaybackRate(speed);
                     });


    ui->volumeSlider->setRange(0, 100); // Set the slider range from 0 to 100
    ui->volumeSlider->setValue(player->volume()); // Set the slider value to the player's current volume
    QObject::connect(ui->volumeSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    // Get the total duration of the video
    qint64 totalVideoDuration = player->duration();
    ui->positionSlider->setRange(0, totalVideoDuration);

    // Handling media state changes
    QObject::connect(player, &QMediaPlayer::stateChanged, [=]() {

        // Make sure the video is loaded
        qint64 totalVideoDuration = player->duration();
        if(totalVideoDuration != 0){
            ui->positionSlider->setRange(0, totalVideoDuration);
        }

    });


    // Create a timer that triggers every 10 milliseconds
    QTimer *updatesliderTimer = new QTimer();
    updatesliderTimer->setInterval(10);

    // Connect the timer's timeout signal to the slot function that updates the location.
    QObject::connect(updatesliderTimer, &QTimer::timeout, [=]() {
         ui->positionSlider->setValue(player->position());
    });
    updatesliderTimer->start();

    QObject::connect(player, &ThePlayer::positionChanged,  ui->positionSlider, &QSlider::setValue);

    // Click event for the fast-forward button
    QObject::connect(ui->btn_fast, &QPushButton::clicked, [&](){
        qint64 currentPosition = player->position();
        qint64 duration = player->duration();
        if(currentPosition + 5000 < duration){
            player->setPosition(currentPosition + 5000);
        } else {
            player->setPosition(duration);
        }
    });

    //qss
    mStyleSheetEditor = new StyleSheetEditor(this);

    QObject::connect(ui->btn_qss, &QPushButton::clicked, [&](){
        mStyleSheetEditor->show();
    });



    // Click event for the rewind button
    QObject::connect(ui->btn_rewind, &QPushButton::clicked, [&](){
        qint64 currentPosition = player->position();
        if(currentPosition - 5000 > 0){
            player->setPosition(currentPosition - 5000);
        } else {
            player->setPosition(0);
        }
    });


    ui->lb_total->setText(formatTime(totalVideoDuration));
    ui->lb_start->setText("00:00");


    // Update Labels on Position Change
    QObject::connect(player, &QMediaPlayer::positionChanged, [=](qint64 position) {
        ui->lb_start->setText(formatTime(position));
    });

    QObject::connect(player, &QMediaPlayer::durationChanged, [=](qint64 duration) {
        ui->lb_total->setText(formatTime(duration));
        ui->positionSlider->setRange(0, duration);
    });

    // Update Position Slider Interaction
    QObject::connect(ui->positionSlider, &QSlider::sliderMoved, player, &ThePlayer::setPosition);

    setWidget->setBrightnessValue(videoWidget->brightness());
    QObject::connect(setWidget, &playSetting::brightness_moved, videoWidget, &QVideoWidget::setBrightness);
    QObject::connect(videoWidget, &QVideoWidget::brightnessChanged, setWidget, &playSetting::setBrightnessValue);

    setWidget->setContrastValue(videoWidget->contrast());
    QObject::connect(setWidget, &playSetting::contrast_moved, videoWidget, &QVideoWidget::setContrast);
    QObject::connect(videoWidget, &QVideoWidget::contrastChanged, setWidget, &playSetting::setContrastValue);

    setWidget->setHueValue(videoWidget->hue());
    QObject::connect(setWidget, &playSetting::hue_moved, videoWidget, &QVideoWidget::setHue);
    QObject::connect(videoWidget, &QVideoWidget::hueChanged, setWidget, &playSetting::setHueValue);

    setWidget->setSaturationValue(videoWidget->saturation());
    QObject::connect(setWidget, &playSetting::saturation_moved, videoWidget, &QVideoWidget::setSaturation);
    QObject::connect(videoWidget, &QVideoWidget::saturationChanged, setWidget, &playSetting::setSaturationValue);




//         Connect the button click event to the full screen toggle slot
    QObject::connect(ui->btn_full, &QPushButton::clicked, [&]() {
        if (isFullScreen()) {
            showNormal(); // Exit Fulllscreen
            ui->btn_full->setIcon(QIcon(":/full.png"));

        } else {
            showFullScreen(); // Toggle Fullscreen
            ui->btn_full->setIcon(QIcon(":/normal.png"));
        }
    });

    QObject::connect(ui->btn_open, &QPushButton::clicked, [=]() {
        QFileDialog *fileDialog = new QFileDialog();
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog->setFileMode(QFileDialog::ExistingFiles);
        fileDialog->setWindowTitle(QObject::tr("Open File"));

        QStringList supportedMimeTypes = player->supportedMimeTypes();
        if (!supportedMimeTypes.isEmpty()) {
            supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
            fileDialog->setMimeTypeFilters(supportedMimeTypes);
        }


        fileDialog->setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));


        if (fileDialog->exec() == QDialog::Accepted)
            player->addToPlaylist(fileDialog->selectedUrls());


        player->play();
    });

    videoWidget->raise();

}

videoPlayer::~videoPlayer()
{
    delete ui;
}




void videoPlayer::on_btn_list_clicked()
{
    if(ui->playlistWidget->isHidden())
    {

        ui->playlistWidget->show();
    }
    else{
        ui->playlistWidget->hide();
    }
}

