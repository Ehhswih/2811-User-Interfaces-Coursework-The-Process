//
//
//

#include "the_player.h"

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;

    // Clear the current playlist and add a new video
    playlist->clear();
    for (const auto &info : *infos) {
        playlist->addMedia(*info.url);
    }

    // Start playing the first video
    playlist->setCurrentIndex(0);
    play();
}

// change the image and video for one button every one second
void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);

    //Remove this comment if you need a rotating image
    // buttons -> at( updateCount++ % buttons->size() ) -> init( i );
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();

}

void ThePlayer::setSpeed(qreal speed) {
    qDebug() << "Setting playback speed to:" << speed;
    setPlaybackRate(speed);
}

void ThePlayer::nextVideo() {
    playlist->next();
}

void ThePlayer::previousVideo() {
    playlist->previous();
}


static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void ThePlayer::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            playlist->load(url);
        else
            playlist->addMedia(url);
    }
}


