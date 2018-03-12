#include "player.h"
#include <QtWidgets>
#include <QMediaMetaData>
#include <QtDebug>
#include <QFileDialog>
#include <QFileInfo>

Player::Player(QWidget *parent) :
    QWidget(parent)
{
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    playlistModel = new QStandardItemModel(this);
    listModel = new QStandardItemModel(this);
    playerControls = new PlayerControls(this);
    list = new QListView(this);
    playlistView = new QTableView(this);
    slider = new QSlider(Qt::Horizontal, this);
    imageLabel = new QLabel(this);
    menu = new QMenuBar(this);
    fileMenu = new QMenu("File", this);
    playbackMenu = new QMenu("Playback", this);
    aboutMenu = new QMenu("About", this);

    QBoxLayout *vlayout = new QVBoxLayout;
    QBoxLayout *controlLayout = new QHBoxLayout;
    QBoxLayout *layout = new QHBoxLayout;
    QBoxLayout *miscLayout = new QVBoxLayout;
    QBoxLayout *playlistLayout = new QHBoxLayout;
    QBoxLayout *sliderLayout = new QHBoxLayout;
    durationLabel = new QLabel(this);
    QToolButton *addButton = new QToolButton;

    addButton->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    playlistModel->setHorizontalHeaderLabels(QStringList() << tr("Title") << tr("Artist") << tr("#") << tr("Album") << tr("Bitrate") << tr("Length"));

    playlistView->setModel(playlistModel);
    playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    playlistView->horizontalHeader()->setStretchLastSection(true);
    playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    playlistView->verticalHeader()->setVisible(false);
    playlistView->horizontalHeader()->resizeSection(2,25);
    playlistView->setContextMenuPolicy(Qt::CustomContextMenu);

    list->setModel(listModel);
    list->setEditTriggers(QAbstractItemView::SelectedClicked);
    list->setContextMenuPolicy(Qt::CustomContextMenu);
    list->setMaximumWidth(350);

    QStandardItem *item = new QStandardItem(tr("Untitled playlist"));
    listModel->appendRow(item);

    imageLabel->setScaledContents(true);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMaximumSize(350,350);

    player->setPlaylist(playlist);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    menu->addMenu(fileMenu);
    menu->addMenu(playbackMenu);
    menu->addMenu(aboutMenu);
    fileMenu->addAction("Open...", this, SLOT(open()), QKeySequence(tr("Ctrl+O")));
    fileMenu->addAction("New playlist...", this, SLOT(newPlaylist()));
    playbackMenu->addAction("Stop", player, SLOT(stop()));
    playbackMenu->addAction("Pause", player, SLOT(pause()));
    playbackMenu->addAction("Play", player, SLOT(play()));
    playbackMenu->addAction("Next", playlist, SLOT(next()));
    playbackMenu->addAction("Previous", playlist, SLOT(previous()));
    aboutMenu->addAction("About", this, SLOT(about()));

    miscLayout->addWidget(list);
    miscLayout->addWidget(imageLabel);
    playlistLayout->addWidget(playlistView);
    layout->addLayout(miscLayout);
    layout->addLayout(playlistLayout);
    controlLayout->addWidget(addButton);
    controlLayout->addWidget(playerControls);
    vlayout->addWidget(menu);
    vlayout->addLayout(controlLayout);
    vlayout->addLayout(layout);
    sliderLayout->addWidget(slider);
    sliderLayout->addWidget(durationLabel);
    vlayout->addLayout(sliderLayout);
    setLayout(vlayout);

    playerControls->setState(player->state());
    playerControls->setVolume(player->volume());
    playerControls->setMuted(playerControls->isMuted());

    connect(playerControls, SIGNAL(play()), player, SLOT(play()));
    connect(playerControls, SIGNAL(pause()), player, SLOT(pause()));
    connect(playerControls, SIGNAL(stop()), player, SLOT(stop()));
    connect(playerControls, SIGNAL(next()), playlist, SLOT(next()));
    connect(playerControls, SIGNAL(previous()), this, SLOT(previousClicked()));
    connect(playerControls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));
    connect(playerControls, SIGNAL(toggleMute(bool)), player, SLOT(setMuted(bool)));
    connect(playerControls, SIGNAL(changePlaybackMode(int)), this, SLOT(playbackModeChanged(int)));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), playerControls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(volumeChanged(int)), playerControls, SLOT(setVolume(int)));
    connect(player, SIGNAL(mutedChanged(bool)), playerControls, SLOT(setMuted(bool)));
    connect(addButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(playlistView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(setTrack(QModelIndex)));
    connect(list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(setPlaylist(QModelIndex)));
    connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(providePlaylistContextMenu(const QPoint &)));
    connect(playlistView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(provideTrackContextMenu(const QPoint &)));


    playlistVector.append(playlist);
    modelVector.append(playlistModel);
    setWindowTitle(QString("Now playing nothing!"));
}

Player::~Player()
{
    delete player;
    delete playlist;
    delete playlistModel;
    delete listModel;
    delete playerControls;
    delete list;
    delete playlistView;
    delete slider;
    delete imageLabel;
    delete menu;
    delete fileMenu;
    delete playbackMenu;
    delete aboutMenu;
    delete durationLabel;
}

void Player::open()
{
    QFileDialog fileDialog(this);
    QList<QStandardItem *> items;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setNameFilter(tr("Audio files (*.mp3 *.flac *.m3u)"));
    fileDialog.setWindowTitle(tr("Open Files"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    if(!filepath.isEmpty())
        fileDialog.setDirectory(filepath);
    if (fileDialog.exec() == QDialog::Accepted)
    {
        addToPlaylist(fileDialog.selectedUrls());
        playlist->setCurrentIndex(playlist->mediaCount()-1);
        QFileInfo fileInfo(fileDialog.selectedFiles().last());
        filename =  fileInfo.completeBaseName();
        filepath = fileInfo.absolutePath();
    }
    player->play();
}

void Player::durationChanged(qint64 duration)
{
    this->duration = duration/1000;
    slider->setMaximum(duration / 1000);
}

void Player::positionChanged(qint64 progress)
{
    if (!slider->isSliderDown()) {
        slider->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void Player::metaDataChanged()
{
    QString albumTitle, length, coverpath, number, bitrate;
    title = filename;
    artist = "Unknown artist";
    if(player->isMetaDataAvailable())
    {
        if(player->metaData(QMediaMetaData::Title).toString() != "")
            title = player->metaData(QMediaMetaData::Title).toString();
        if(player->metaData(QMediaMetaData::AlbumArtist).toString() != "")
            artist = player->metaData(QMediaMetaData::AlbumArtist).toString();
        albumTitle = player->metaData(QMediaMetaData::AlbumTitle).toString();
        QTime totalDuration((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QString format = duration > 3600 ? "hh:mm:ss" : "mm:ss";
        length = totalDuration.toString(format);
        number = player->metaData(QMediaMetaData::TrackNumber).toString();
        bitrate = player->metaData(QMediaMetaData::AudioBitRate).toString();
        if(playlistModel->findItems(player->metaData(QMediaMetaData::Title).toString()).isEmpty() && playlistModel->findItems(filename).isEmpty())
        {
            QList<QStandardItem *> items;
            items.append(new QStandardItem(title));
            items.append(new QStandardItem(artist));
            items.append(new QStandardItem(number));
            items.append(new QStandardItem(albumTitle));
            items.append(new QStandardItem(bitrate));
            items.append(new QStandardItem(length));
            playlistModel->appendRow(items);
            playlistView->setRowHeight(playlistModel->rowCount()-1, 15);
        }
        coverpath = filepath + "/cover.jpg";
        QPixmap pixmap(coverpath);
        QSize size = pixmap.size();
        pixmap.scaled(size, Qt::KeepAspectRatio);
        imageLabel->setPixmap(pixmap);
    }
    setTrackInfo();
}

void Player::addToPlaylist(const QList<QUrl> urls)
{
    foreach (const QUrl &url, urls) {
        playlist->addMedia(url);
    }
}

void Player::previousClicked()
{
    if(player->position() <= 5000)
        playlist->previous();
    else
        player->setPosition(0);
}

void Player::seek(int time)
{
    player->setPosition(time * 1000);
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    durationLabel->setText(tStr);
}

void Player::playbackModeChanged(int mode)
{
    switch (mode)
    {
    case 0:
        playlist->setPlaybackMode(QMediaPlaylist::Sequential);
        break;
    case 1:
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        break;
    case 2:
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        break;
    case 3:
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        break;
    }
}

void Player::setPlaylist(QModelIndex index)
{
    playlist = playlistVector[index.row()];
    playlistModel = modelVector[index.row()];
    player->setPlaylist(playlistVector[index.row()]);
    playlistView->setModel(modelVector[index.row()]);
    playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    playlistView->horizontalHeader()->setStretchLastSection(true);
    playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    playlistView->verticalHeader()->setVisible(false);
    playlistView->horizontalHeader()->resizeSection(2,25);
    playlistView->setContextMenuPolicy(Qt::CustomContextMenu);
    playlist->setCurrentIndex(0);
    player->play();
}

void Player::newPlaylist()
{
    QStandardItem *item = new QStandardItem(tr("Untitled playlist"));
    listModel->appendRow(item);
    playlist = new QMediaPlaylist(this);
    playlistModel = new QStandardItemModel(this);
    connect(playerControls, SIGNAL(next()), playlist, SLOT(next()));
    playlistModel->setHorizontalHeaderLabels(QStringList() << tr("Title") << tr("Artist") << tr("#") << tr("Album") << tr("Bitrate") << tr("Length"));
    playlistVector.append(playlist);
    modelVector.append(playlistModel);
}

void Player::providePlaylistContextMenu(const QPoint &point)
{
    qDebug() << "context";
    QAction *addAction = new QAction("Add new playlist",list);
    QAction *removeAction = new QAction("Remove playlist",list);
    QMenu *contextMenu = new QMenu(this);
    remove = list->indexAt(point);
    qDebug() << remove.isValid();
    contextMenu->addAction(addAction);
    contextMenu->addAction(removeAction);
    connect(addAction, SIGNAL(triggered()), this, SLOT(newPlaylist()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removePlaylist()));
    contextMenu->exec(QCursor::pos());
    delete contextMenu;
}

void Player::removePlaylist()
{
    if(remove.isValid() && remove.row() != 0)
    {
        listModel->removeRow(remove.row());
        delete playlistVector[remove.row()];
        delete modelVector[remove.row()];
        playlistVector.remove(remove.row());
        modelVector.remove(remove.row());
    }
}

void Player::setTrackInfo()
{
    setWindowTitle(QString("Now playing: %1 by %2").arg(title).arg(artist));
}

void Player::provideTrackContextMenu(const QPoint &point)
{
    QAction *removeAction = new QAction("Remove track",playlistView);
    QMenu *contextMenu = new QMenu(this);
    remove = playlistView->indexAt(point);
    qDebug() << remove.isValid();
    contextMenu->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeTrack()));
    contextMenu->exec(QCursor::pos());
    delete contextMenu;
}

void Player::removeTrack()
{
    if(remove.isValid())
    {
        playlistModel->removeRow(remove.row());
        playlist->removeMedia(remove.row());
        setWindowTitle(QString("Now playing nothing!"));
    }
}

void Player::setTrack(QModelIndex index)
{
    playlist->setCurrentIndex(index.row());
}

void Player::about()
{
    QMessageBox::information(this, tr("About"), tr("Made by mm 2017/18"));
}
