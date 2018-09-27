#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include "QMediaPlayer"
#include "QMediaPlaylist"

#include "QStandardItemModel"
#include "QStandardItem"

class audio : public QObject
{
    Q_OBJECT
public:
    explicit audio(QObject *parent = 0);
    ~audio();
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
