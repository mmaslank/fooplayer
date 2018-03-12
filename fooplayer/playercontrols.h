#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAbstractButton>
#include <QAbstractSlider>
#include <QComboBox>

class PlayerControls : public QWidget
{
    Q_OBJECT
public:
    PlayerControls(QWidget *parent = nullptr);
    ~PlayerControls();
    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void toggleMute(bool mute);
    void changePlaybackMode(int mode);

private slots:
    void playClicked();
    void muteClicked();
    void onVolumeSliderValueChanged();
    void playbackModeChanged();

private:
    QMediaPlayer::State playerState;
    bool playerMuted;
    QAbstractButton *playButton;
    QAbstractButton *stopButton;
    QAbstractButton *nextButton;
    QAbstractButton *previousButton;
    QAbstractButton *muteButton;
    QAbstractSlider *volumeSlider;
    QComboBox *playbackMode;
};

#endif // PLAYERCONTROLS_H
