#ifndef PLAYER_H
#define PLAYER_H

#include "playercontrols.h"
#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QListView>
#include <QTableView>
#include <QSlider>
#include <QLabel>
#include <QToolBar>
#include <QComboBox>
#include <QMenuBar>
#include <QVector>

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = 0);
    void addToPlaylist(const QList<QUrl> urls);
    ~Player();

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();
    void previousClicked();
    void seek(int time);
    void playbackModeChanged(int mode);
    void setPlaylist(QModelIndex index);
    void newPlaylist();
    void providePlaylistContextMenu(const QPoint &point);
    void removePlaylist();
    void provideTrackContextMenu(const QPoint &point);
    void removeTrack();
    void setTrack(QModelIndex index);
    void about();

private:
    void updateDurationInfo(qint64 currentInfo);
    void setTrackInfo();
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QStandardItemModel *playlistModel;
    QStandardItemModel *listModel;
    QListView *list;
    QTableView *playlistView;
    QWidget *cover;
    PlayerControls *playerControls;
    QSlider *slider;
    QMenuBar *menu;
    QMenu *fileMenu;
    QMenu *playbackMenu;
    QMenu *aboutMenu;
    QString filename;
    QString filepath;
    QString statusInfo;
    QString title;
    QString artist;
    QLabel *durationLabel;
    QLabel *imageLabel;
    qint64 duration;
    QVector<QMediaPlaylist*> playlistVector;
    QVector<QStandardItemModel*> modelVector;
    QModelIndex remove;
};

#endif // PLAYER_H
