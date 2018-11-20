#include "audioplayeer.h"

audioplayeer::audioplayeer(QObject *parent) : QObject(parent)
{
    m_playListModel = new QStandardItemModel(this);

    m_player = new QMediaPlayer(this);// Инициализация плеера
    m_playlist = new QMediaPlaylist(m_player);// Инициализация плейлиста

    m_player->setPlaylist(m_playlist);// Установка плейлиста в плеер
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop); // Циклический режим проигрывания плеера

}

audioplayeer::~audioplayeer()
{

}

//При передвижении ползунка передвигается время
void audioplayeer::setPosition(int time)
{
    m_player->setPosition(time);
}

//Для двойного щелчка и вывода записи
void audioplayeer::setCurrentIndex(QModelIndex index)
{
    m_playlist->setCurrentIndex(index.row());
    m_player->play();
}
