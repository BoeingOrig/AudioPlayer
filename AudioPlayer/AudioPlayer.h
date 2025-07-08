#pragma once

// AudioPlayer.h

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QtWidgets/QMainWindow>
#include "ui_AudioPlayer.h"     // ���������� ���� UI

#include <QMediaPlayer>         // ���������� ��� ������ � �����
#include <QAudioOutput>         // ���������� ��� ������ �����
#include <QIcon>                // ���������� ��� ������ � ��������
#include <QCloseEvent>          // ���������� ��� ��������������� closeEvent
#include <QSettings>            // ���������� ��� ����������/�������� ��������

class AudioPlayer : public QMainWindow
{
    Q_OBJECT                    // ������ Q_OBJECT ��������� ��� ������������� �������� � ������

public:
    // ����������� ������ AudioPlayer
    AudioPlayer(QWidget* parent = nullptr);
    // ���������� ������ AudioPlayer
    ~AudioPlayer();

protected: // closeEvent ������ ���� protected
    void closeEvent(QCloseEvent* event) override; // �������������� ������� �������� ����

private slots:
    // ���� ��� ��������� ������� ������� ������ "�������������/�����"
    void  on_actionPlayPause_triggered();
    // ���� ��� ��������� ������� ������� ������ "����"
    void on_actionStop_triggered();
    // ���� ��� ��������� ������� ������� ������ "������� ����"
    void on_actionOpen_File_triggered();
    // ���� ��� ������������ ��������� ��������� ��������������� ������
    void on_player_playbackStateChanged(QMediaPlayer::PlaybackState state);
    // ���� ��� ���������� ������� �������
    void on_player_positionChanged(qint64 position);
    // ���� ��� ���������� ����� ������������
    void on_player_durationChanged(qint64 duration);
    // ���� ��� ����������� �������� ������������� (������)
    void on_positionSlider_sliderPressed();
    // ���� ��� ����������� �������� ������������� (����������)
    void on_positionSlider_sliderReleased();
    // ���� ��� ����������� �������� ������������� (�� ����� ��������������)
    // �� ����� ������������ ��� ��� ���������������� ����������� �������, �� �������� ���������
    // ������� ����� ������ �� sliderReleased.
    void on_positionSlider_valueChanged(int value);

private:
    Ui::AudioPlayerClass ui;     // ������ UI, ��������������� Qt Designer
    QMediaPlayer* player;        // ��������� �� ������ QMediaPlayer ��� ���������� ����������������
    QAudioOutput* audioOutput;   // ��������� �� ������ QAudioOutput ��� ������ �����

    // �������� ��������� ������ ��� ��������
    QIcon playIcon;
    QIcon pauseIcon;

    // ����, ����� �������� ����������� ��������� �������� �� �����������������
    bool isSeeking = false;
    // ��������������� ������� ��� �������������� �������
    QString formatTime(qint64 milliseconds);
};

#endif // AUDIOPLAYER_H