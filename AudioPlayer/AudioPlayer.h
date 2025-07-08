#pragma once

// AudioPlayer.h

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QtWidgets/QMainWindow>
#include "ui_AudioPlayer.h"     // Используем файл UI

#include <QMediaPlayer>         // Директория для работы с аудио
#include <QAudioOutput>         // Директория для вывода аудио
#include <QIcon>                // Директория для работы с иконками
#include <QCloseEvent>          // Директория для переопределения closeEvent
#include <QSettings>            // Директория для сохранения/загрузки настроек

class AudioPlayer : public QMainWindow
{
    Q_OBJECT                    // Макрос Q_OBJECT необходим для использования сигналов и слотов

public:
    // Конструктор класса AudioPlayer
    AudioPlayer(QWidget* parent = nullptr);
    // Деструктор класса AudioPlayer
    ~AudioPlayer();

protected: // closeEvent должен быть protected
    void closeEvent(QCloseEvent* event) override; // Переопределяем событие закрытия окна

private slots:
    // Слот для обработки события нажатия кнопки "Воспроизвести/Пауза"
    void  on_actionPlayPause_triggered();
    // Слот для обработки события нажатия кнопки "Стоп"
    void on_actionStop_triggered();
    // Слот для обработки события нажатия кнопки "Открыть файл"
    void on_actionOpen_File_triggered();
    // Спот для отслеживания изменения состояния воспроизведения плеера
    void on_player_playbackStateChanged(QMediaPlayer::PlaybackState state);
    // Слот для обновления текущей позиции
    void on_player_positionChanged(qint64 position);
    // Слот для обновления общей длительности
    void on_player_durationChanged(qint64 duration);
    // Слот для перемещения ползунка пользователем (начало)
    void on_positionSlider_sliderPressed();
    // Слот для перемещения ползунка пользователем (отпускание)
    void on_positionSlider_sliderReleased();
    // Слот для перемещения ползунка пользователем (во время перетаскивания)
    // Мы можем использовать его для предварительного отображения времени, но реальное изменение
    // позиции будем делать по sliderReleased.
    void on_positionSlider_valueChanged(int value);

private:
    Ui::AudioPlayerClass ui;     // Объект UI, сгенерированный Qt Designer
    QMediaPlayer* player;        // Указатель на объект QMediaPlayer для управления воспроизведением
    QAudioOutput* audioOutput;   // Указатель на объект QAudioOutput для вывода звука

    // Сохраним состояния иконок для удобства
    QIcon playIcon;
    QIcon pauseIcon;

    // Флаг, чтобы отличать программное изменение ползунка от пользовательского
    bool isSeeking = false;
    // Вспомогательная функция для форматирования времени
    QString formatTime(qint64 milliseconds);
};

#endif // AUDIOPLAYER_H