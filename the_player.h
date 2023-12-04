//
//
//

#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H


#include <QApplication>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSlider>
#include <QListWidget>

#include "the_button.h"
#include <vector>
#include <QTimer>

class ThePlayer : public QMediaPlayer {

Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    long updateCount = 0;
    QMediaPlaylist* playlist;

public:
    ThePlayer() : QMediaPlayer(NULL) {
        setVolume(50); // be slightly less annoying
        connect (this, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (playStateChanged(QMediaPlayer::State)) );

        mTimer = new QTimer(NULL);
        mTimer->setInterval(1000); // 1000ms is one second between ...
        mTimer->start();
        connect( mTimer, SIGNAL (timeout()), SLOT ( shuffle() ) ); // ...running shuffle method

        playlist = new QMediaPlaylist(this);
        setPlaylist(playlist);
    }

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);


private slots:

    // change the image and video for one button every one second
    void shuffle();

    void playStateChanged (QMediaPlayer::State ms);

public slots:

    // start playing this ButtonInfo
    void jumpTo (TheButtonInfo* button);

    void togglePlayPause() {
            if (state() == QMediaPlayer::PlayingState)
                pause();
            else
                play();
        }

    // control the video speed
    void setSpeed(qreal speed);

    void nextVideo();
    void previousVideo();
};

#endif //CW2_THE_PLAYER_H
