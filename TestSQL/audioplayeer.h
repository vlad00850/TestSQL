#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include "QMediaPlayer"
#include "QMediaPlaylist"

#include "QStandardItemModel"
#include "QStandardItem"

class audioplayeer : public QObject
{
    Q_OBJECT
public:
    explicit audioplayeer(QObject *parent = 0);
    ~audioplayeer();
    QStandardItemModel *m_playListModel;
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;

private:

signals:

public slots:
    void setPosition(int time); //При передвижении ползунка передвигается время
    void setCurrentIndex(QModelIndex index);//Для двойного щелчка и вывода записи
};

#endif // AUDIO_H
