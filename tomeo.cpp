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

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn(std::string(argv[1]));

    qDebug() << "Number of videos found:" << videos.size();

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    QHBoxLayout *layout = new QHBoxLayout();

    buttonWidget->setLayout(layout);

    //Setting the Progress Bar Layout
    QWidget *sliderWidget = new QWidget();
    QHBoxLayout *sliderlayout = new QHBoxLayout();
    sliderWidget->setLayout(sliderlayout);

    //General layout of the function bar
    QWidget *functionWidget = new QWidget();
    QVBoxLayout *functionlayout = new QVBoxLayout();
    functionWidget->setLayout(functionlayout);
    functionlayout->addWidget(sliderWidget);
    functionlayout->addWidget(buttonWidget);

    // Remove this comment if you need a rotating image
    // create the four buttons
    // for ( int i = 0; i < 4; i++ ) {
    //     TheButton *button = new TheButton(buttonWidget);
    //     button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
    //     buttons.push_back(button);
    //     layout->addWidget(button);
    //     button->init(&videos.at(i));
    // }

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

    QPushButton *nextButton = new QPushButton("Next", buttonWidget);
    QPushButton *prevButton = new QPushButton("Previous", buttonWidget);

    layout->addWidget(prevButton);
    layout->addWidget(nextButton);

    QObject::connect(nextButton, &QPushButton::clicked, player, &ThePlayer::nextVideo);
    QObject::connect(prevButton, &QPushButton::clicked, player, &ThePlayer::previousVideo);


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
    QSlider* positionSlider = new QSlider(Qt::Horizontal, buttonWidget);

    // Get the total duration of the video
    qint64 totalVideoDuration = player->duration();
    positionSlider->setRange(0, totalVideoDuration);


    // Handling media state changes
    QObject::connect(player, &QMediaPlayer::stateChanged, [=](QMediaPlayer::State newState) {

       // Make sure the video is loaded
       qint64 totalVideoDuration = player->duration();
       if(totalVideoDuration != 0){
          positionSlider->setRange(0, totalVideoDuration);
       }

    });

    QObject::connect(player, &ThePlayer::positionChanged, positionSlider, &QSlider::setValue);
    QObject::connect(positionSlider, &QSlider::valueChanged, player, &ThePlayer::setPosition);

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





    // // Create the main window and layout
    // QGraphicsScene *scene = new QGraphicsScene;
    // QGraphicsView *view = new QGraphicsView(scene);

    // // Add the video widget to the scene
    // QGraphicsProxyWidget *videoProxy = scene->addWidget(videoWidget);
    // videoProxy->setZValue(0); // Video is at layer 0

    // // Add the function widget to the scene
    // QGraphicsProxyWidget *functionProxy = scene->addWidget(functionWidget);
    // functionProxy->setZValue(1); // Function bar is at layer 1, above the video




    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // Create full screen
    QPushButton fullscreenButton("Toggle Fullscreen");
    layout->addWidget(&fullscreenButton);

    // Connect the button click event to the full screen toggle slot
    QObject::connect(&fullscreenButton, &QPushButton::clicked, [&window, &fullscreenButton]() {
        if (window.isFullScreen()) {
            window.showNormal(); // Exit Fulllscreen
            fullscreenButton.setText("Toggle Fullscreen");
        } else {
            window.showFullScreen(); // Toggle Fullscreen
            fullscreenButton.setText("Exit Fullscreen");
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
    // Style example-------------------------------------------------

    //用于查看功能栏大小以及布局
    // functionWidget->setStyleSheet("background-color: rgba(0, 0, 0, 128);");

    // add the video and the buttons to the top level widget
        // Set up the layout for the main window
        // top->addWidget(view);

    top->addWidget(videoWidget);
    top->addWidget(functionWidget);

    // 在需要时，隐藏或显示部件
    // videoWidget->setVisible(true);  // 显示 videoWidget
    // sliderWidget->setVisible(false);  // 隐藏 sliderWidget
    // buttonWidget->setVisible(false);  // 隐藏 buttonWidget

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
