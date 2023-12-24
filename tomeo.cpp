//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

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
#include <QComboBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QAudioDeviceInfo>
#include <QVideoWidget>
#include <QSlider>
#include <QFileDialog>
#include <QPushButton>
#include <QPixmap>
#include <QTimer>
#include <QStackedLayout>
#include <QLabel>


#include "the_player.h"
#include "the_button.h"

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

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

QString formatTime(qint64 timeInMs) {
    qint64 seconds = timeInMs / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString::asprintf("%02lld:%02lld", minutes, seconds);
}

int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // create the translator
    QTranslator translator;

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc >= 2)
        videos = getInfoIn(std::string(argv[1]));

    qDebug() << "Number of videos found:" << videos.size();

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    if (argc == 3)
    {
        QString qmPath = argv[2];
        if (translator.load(qmPath)) {
            app.installTranslator(&translator);
        } else {
            qDebug() << "Failed to load translation file:" << qmPath;
        }
    }

    // create the main window and layout
    QWidget window;
    // QMainWindow window;
    QVBoxLayout *top = new QVBoxLayout(&window);
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;
    QVBoxLayout *videolayout = new QVBoxLayout();
    videoWidget->setLayout(videolayout);

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    //General layout of the function bar
    QWidget *functionWidget = new QWidget();
    QVBoxLayout *functionlayout = new QVBoxLayout();
    functionWidget->setLayout(functionlayout);

    // functionWidget->setFixedHeight(500);

    // videolayout->addWidget(functionWidget);

    // a row of buttons
    QWidget *buttonWidget = new QWidget(functionWidget);
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    QHBoxLayout *layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    //Setting the Progress Bar Layout
    QWidget *sliderWidget = new QWidget(functionWidget);
    QHBoxLayout *sliderlayout = new QHBoxLayout(sliderWidget);
    sliderWidget->setLayout(sliderlayout);

    functionlayout->addWidget(sliderWidget);
    functionlayout->addWidget(buttonWidget);

    // functionWidget->setVisible(true);

    // create the play and pause buttons
    QPushButton *togglePlayPauseButton = new QPushButton("Pause", buttonWidget);
    // add the toggle button to the layout
    layout->addWidget(togglePlayPauseButton);

    // Connect the signals and slots
    QObject::connect(togglePlayPauseButton, &QPushButton::clicked, [player, togglePlayPauseButton]() {
        player->togglePlayPause();
        if (player->state() == QMediaPlayer::PlayingState)
            togglePlayPauseButton->setText("Pause");
        else
            togglePlayPauseButton->setText("Play");
    });


    QListWidget *playlistWidget = new QListWidget(buttonWidget);
    layout->addWidget(playlistWidget);

    QMediaPlaylist *playlist = player->getPlaylist();
    // 设置固定高度
    playlistWidget->setFixedHeight(75);

    // Function to update the playlist widget
    auto updatePlaylistWidget = [&]() {
        playlistWidget->clear();
        for (int i = 0; i < playlist->mediaCount(); ++i) {
            QUrl mediaUrl = playlist->media(i).request().url();
            playlistWidget->addItem(mediaUrl.fileName());
        }
    };

    // Initial population of the playlist widget
    updatePlaylistWidget();

    // Connect playlist update signals to the update function
    QObject::connect(playlist, &QMediaPlaylist::mediaInserted, updatePlaylistWidget);
    QObject::connect(playlist, &QMediaPlaylist::mediaRemoved, updatePlaylistWidget);
    QObject::connect(playlist, &QMediaPlaylist::mediaChanged, updatePlaylistWidget);

    // Highlight Current Playing Media
    QObject::connect(playlist, &QMediaPlaylist::currentIndexChanged, [playlistWidget](int index){
        playlistWidget->setCurrentRow(index);
    });

    // Connect itemClicked Signal
    QObject::connect(playlistWidget, &QListWidget::itemClicked, [playlist, playlistWidget](QListWidgetItem *item){
        int index = playlistWidget->row(item);
        playlist->setCurrentIndex(index);
    });


    // create the next and previous buttons
    QPushButton *nextButton = new QPushButton("Next", buttonWidget);
    QPushButton *prevButton = new QPushButton("Previous", buttonWidget);

    layout->addWidget(prevButton);
    layout->addWidget(nextButton);

    QObject::connect(nextButton, &QPushButton::clicked, player, &ThePlayer::nextVideo);
    QObject::connect(prevButton, &QPushButton::clicked, player, &ThePlayer::previousVideo);

    // Set the Corresponding Playback Modes
    QString tr_seq = QObject::tr("Sequential");
    QString tr_loop = QObject::tr("Loop");
    QString tr_stop = QObject::tr("Stop at End");
    QComboBox *playbackModeComboBox = new QComboBox;
    playbackModeComboBox->addItem(tr_seq);
    playbackModeComboBox->addItem(tr_loop);
    playbackModeComboBox->addItem(tr_stop);
    layout->addWidget(playbackModeComboBox);

    QObject::connect(playbackModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [playlist, player, togglePlayPauseButton](int index){
            switch(index) {
                case 0: // Sequential
                    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
                    break;
                case 1: // Loop
                    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
                    QObject::connect(player, &QMediaPlayer::mediaStatusChanged,
                            [playlist, player, togglePlayPauseButton](QMediaPlayer::MediaStatus status) {
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
                        [playlist, player, togglePlayPauseButton](QMediaPlayer::MediaStatus status) {
                            if (status == QMediaPlayer::EndOfMedia) {
                                player->pause();
                                togglePlayPauseButton->setText("Play");
                            }
                        });
                    break;
                default:
                    break;
            }
        });


    // Creating drop-down   menus for multiplier playback
    QComboBox *speedComboBox = new QComboBox(buttonWidget);
    speedComboBox->addItem("0.5x", QVariant(0.5));
    speedComboBox->addItem("1.0x", QVariant(1.0));
    speedComboBox->addItem("1.5x", QVariant(1.5));
    speedComboBox->addItem("2.0x", QVariant(2.0));

    layout->addWidget(speedComboBox);

    QObject::connect(speedComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [player, speedComboBox](int index) {
                         qreal speed = speedComboBox->itemData(index).toDouble();
                         player->setPlaybackRate(speed);
                     });


    // Create a volume control slider
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, buttonWidget);
    volumeSlider->setRange(0, 100); // Set the slider range from 0 to 100
    volumeSlider->setValue(player->volume()); // Set the slider value to the player's current volume

    // Add the slider to the layout
    layout->addWidget(volumeSlider);

    // Connect the slider's signal to ThePlayer's setVolume slot
    QObject::connect(volumeSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);


    // Create an event loop that waits for the media to finish first loading.
    QEventLoop loop;
    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, &loop, &QEventLoop::quit);

    // Start the event loop
    loop.exec();

    // Progress bar settings
    QSlider* positionSlider = new QSlider(Qt::Horizontal, sliderWidget);

    // Get the total duration of the video
    qint64 totalVideoDuration = player->duration();
    positionSlider->setRange(0, totalVideoDuration);

    // Handling media state changes
    QObject::connect(player, &QMediaPlayer::stateChanged, [=]() {

       // Make sure the video is loaded
       qint64 totalVideoDuration = player->duration();
       if(totalVideoDuration != 0){
          positionSlider->setRange(0, totalVideoDuration);
       }

    });


     // Create a timer that triggers every 10 milliseconds
     QTimer *updatesliderTimer = new QTimer();
     updatesliderTimer->setInterval(10);

     // Connect the timer's timeout signal to the slot function that updates the location.
     QObject::connect(updatesliderTimer, &QTimer::timeout, [=]() {
         positionSlider->setValue(player->position());
     });
     updatesliderTimer->start();

    QObject::connect(player, &ThePlayer::positionChanged, positionSlider, &QSlider::setValue);


    // create two buttons for fast forward and rewind.
    QPushButton *forwardButton = new QPushButton("Fast Forward");
    QPushButton *rewindButton = new QPushButton("Rewind");
    layout->addWidget(forwardButton);
    layout->addWidget(rewindButton);

    // Click event for the fast-forward button
    QObject::connect(forwardButton, &QPushButton::clicked, [player](){
        qint64 currentPosition = player->position();
        qint64 duration = player->duration();
        if(currentPosition + 5000 < duration){
            player->setPosition(currentPosition + 5000);
        } else {
            player->setPosition(duration);
        }
    });

    // Click event for the rewind button
    QObject::connect(rewindButton, &QPushButton::clicked, [player](){
        qint64 currentPosition = player->position();
        if(currentPosition - 5000 > 0){
            player->setPosition(currentPosition - 5000);
        } else {
            player->setPosition(0);
        }
    });


    // Add Labels for Duration Display
    QLabel *totalDurationLabel = new QLabel(buttonWidget);
    QLabel *currentPositionLabel = new QLabel("00:00", buttonWidget);
    totalDurationLabel->setText(formatTime(totalVideoDuration));

    sliderlayout->addWidget(currentPositionLabel);
    sliderlayout->addWidget(positionSlider);
    sliderlayout->addWidget(totalDurationLabel);

    // Update Labels on Position Change
    QObject::connect(player, &QMediaPlayer::positionChanged, [=](qint64 position) {
        currentPositionLabel->setText(formatTime(position));
    });

    QObject::connect(player, &QMediaPlayer::durationChanged, [=](qint64 duration) {
        totalDurationLabel->setText(formatTime(duration));
        positionSlider->setRange(0, duration);
    });

    // Update Position Slider Interaction
    QObject::connect(positionSlider, &QSlider::sliderMoved, player, &ThePlayer::setPosition);



    // Create a button for taking screenshots
    QPushButton screenshotButton("Take Screenshot");

    // Connect button click to the screenshot function
    QObject::connect(&screenshotButton, &QPushButton::clicked, [functionWidget]() {
        // Capture the current frame as a QPixmap
        QPixmap screenshot = QPixmap::grabWindow(QApplication::desktop()->winId());

        // Save or process the screenshot as needed
        screenshot.save("screenshot.png");
        qDebug() << "Screenshot taken!";

    });

    layout->addWidget(&screenshotButton);

    screenshotButton.setVisible(false); // Initially hide the screenshot button

    // Create full screen
    QPushButton fullscreenButton("Toggle Fullscreen");
    layout->addWidget(&fullscreenButton);

    // Connect the button click event to the full screen toggle slot
    QObject::connect(&fullscreenButton, &QPushButton::clicked, [&window, &fullscreenButton, &screenshotButton]() {
        if (window.isFullScreen()) {
            window.showNormal(); // Exit Fulllscreen
            fullscreenButton.setText("Toggle Fullscreen");
            screenshotButton.setVisible(false);
        } else {
            window.showFullScreen(); // Toggle Fullscreen
            fullscreenButton.setText("Exit Fullscreen");
            screenshotButton.setVisible(true);
        }
    });

    // Style example-------------------------------------------------
    QString fullscreenButtonStyle = "QPushButton {"
                                    "   background-color: #2196F3;"
                                    "   color: white;"
                                    "   border: 2px solid #2196F3;"
                                    "   border-radius: 4px;"
                                    "   padding: 4px 8px;"
                                    "}";

    fullscreenButton.setStyleSheet(fullscreenButtonStyle);


    QPushButton *openFileButton = new QPushButton("Open File");
    layout->addWidget(openFileButton);

    QObject::connect(openFileButton, &QPushButton::clicked, [=]() {
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

    // Create a brightness adjustment slider
    QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(-100, 100);
    brightnessSlider->setValue(videoWidget->brightness());
    QObject::connect(brightnessSlider, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setBrightness);
    QObject::connect(videoWidget, &QVideoWidget::brightnessChanged, brightnessSlider, &QSlider::setValue);

    // Create a contrast adjustment slider
    QSlider *contrastSlider = new QSlider(Qt::Horizontal);
    contrastSlider->setRange(-100, 100);
    contrastSlider->setValue(videoWidget->contrast());
    QObject::connect(contrastSlider, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setContrast);
    QObject::connect(videoWidget, &QVideoWidget::contrastChanged, contrastSlider, &QSlider::setValue);

    // Create a hue adjustment slider
    QSlider *hueSlider = new QSlider(Qt::Horizontal);
    hueSlider->setRange(-100, 100);
    hueSlider->setValue(videoWidget->hue());
    QObject::connect(hueSlider, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setHue);
    QObject::connect(videoWidget, &QVideoWidget::hueChanged, hueSlider, &QSlider::setValue);

    // Create a saturation adjustment slider
    QSlider *saturationSlider = new QSlider(Qt::Horizontal);
    saturationSlider->setRange(-100, 100);
    saturationSlider->setValue(videoWidget->saturation());
    QObject::connect(saturationSlider, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setSaturation);
    QObject::connect(videoWidget, &QVideoWidget::saturationChanged, saturationSlider, &QSlider::setValue);

    // Add sliders to the layout
    layout->addWidget(brightnessSlider);
    layout->addWidget(contrastSlider);
    layout->addWidget(hueSlider);
    layout->addWidget(saturationSlider);



    // Style example-------------------------------------------------

    //用于查看功能栏大小以及布局
    // functionWidget->setStyleSheet("background-color: rgba(0, 0, 0, 128);");
    // Add videoWidget and functionWidget to the top level layout
    // videoWidget->lower();
    // functionWidget->raise();
    // videolayout->addWidget(functionWidget);

    // 设置 functionWidget 为半透明
    // functionWidget->setStyleSheet("background-color: rgba(255, 255, 255, 50);");

    functionWidget->setFixedHeight(175);

    QStackedLayout * sBoxLayout = new QStackedLayout();
    sBoxLayout->setStackingMode(QStackedLayout::StackAll);

    sBoxLayout->addWidget(videoWidget);
    sBoxLayout->addWidget(functionWidget);
    functionWidget->stackUnder(videoWidget);
    videoWidget->raise();

    top->addLayout(sBoxLayout);

    window.setLayout(top);

    // top->addWidget(videoWidget);
    // top->addWidget(functionWidget);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
