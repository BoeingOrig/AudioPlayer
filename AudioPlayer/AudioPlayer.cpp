#include "AudioPlayer.h" // ���������� ������������ ����
#include <QFileDialog>   // ���������� ��� ������� �������� �����
#include <QUrl>          // ���������� ��� ������ � URL ������
#include <QFileInfo>     // ���������� ��� ��������� ����� �����
#include <QDebug>        // ���������� ��� ���������� ���������
#include <QSlider>       // ���������� ��� ������ �� ���������
#include <QCloseEvent>   // ��� closeEvent
#include <QSettings>     // ��� QSettings
#include <QTime>         // ���������� ��� �������������� �������

// ����������� ������ AudioPlayer
AudioPlayer::AudioPlayer(QWidget* parent)
    : QMainWindow(parent)
    , ui() // �������������� ������ ui
{
    ui.setupUi(this); // ����������� ���������������� ���������, ������������ � .ui �����

    // ������������� QAudioOutput ����� QMediaPlayer
    // ������� ������ ��� ������ �����. 'this' ���������, ��� ������������ �������� �������� AudioPlayer,
    // ��� ������������ �������������� �������� ��� �������� AudioPlayer.
    audioOutput = new QAudioOutput(this);

    // ������� ������ ������. 'this' ����� ��������� ������������ ������.
    player = new QMediaPlayer(this);

    // ��������� ����� � ������� �����. ��� ���������� ��� ����, ����� ����� ����,
    // ���� ���������� �������� ������.
    player->setAudioOutput(audioOutput);

    // �������������� ������ (��������� ���������� ���� � ����� �������!)
    playIcon = QIcon(":/res/icons/icon_play.png");
    pauseIcon = QIcon(":/res/icons/icon_pause.png");

    // ������������� ��������� ������ ��� ������ "�������������"
    // �� ������������, ��� actionPlay - ��� �� ������, ������� �� ������ ������������ ��� play/pause
    ui.actionPlayPause->setIcon(playIcon);

    // ���������� ������ ��������� ��������� ��������������� ������ � ������ �����
    connect(player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::on_player_playbackStateChanged);

    // ������� ������ QSettings.
    // ������ �������� - �������� �����������, ������ - �������� ����������.
    // ��� ����� ��� ����, ����� QSettings ����, ���� ��������� ���������.
    QSettings settings("ByteWorks", "AudioPlayer"); // ������� ������ QSettings [cite: 11]

    // ��������� ����������� ���������. ���� ��������� �� �������, ���������� 0.75 �� ���������.
    // �� �����, ����� �� ��������� ���� 0.75, ���� �� ����� ������ �� �����������.
    qreal savedVolume = settings.value("volume", 0.75).toReal(); // ������������ 0.75 �� ���������
    qDebug() << "����������� ��������� (�� ��������):" << savedVolume; // ���������� ���������

    // ������������� ����������� ��������� ��� QAudioOutput
    audioOutput->setVolume(savedVolume); // ���������� ����������� ��������!

    // ������������� �������� �������� ���������, ��������������� ����������� ���������
    ui.volumeSlider->setValue(static_cast<int>(savedVolume * 100)); // ���������� ����������� ��������!
    qDebug() << "�������� ���������� ��:" << ui.volumeSlider->value(); // ���������� ���������

    // ���������� ������ ��������� �������� �������� � ����� ��������� ��������� audioOutput
    connect(ui.volumeSlider, &QSlider::valueChanged, [this](int value) {
        qreal volume = value / 100.0;
        audioOutput->setVolume(volume);
        qDebug() << "��������� �������� ����� �������� ��:" << volume; // ���������� ���������
    });

    // �������������� ����� �������
    ui.positionLabel->setText(formatTime(0));
    ui.durationLabel->setText(formatTime(0));

    // ���������� ������� ������ � ����� ������
    connect(player, &QMediaPlayer::positionChanged, this, &AudioPlayer::on_player_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &AudioPlayer::on_player_durationChanged);

    // ���������� ������� �������� ������� � ����� ������
    connect(ui.positionSlider, &QSlider::sliderPressed, this, &AudioPlayer::on_positionSlider_sliderPressed);
    connect(ui.positionSlider, &QSlider::sliderReleased, this, &AudioPlayer::on_positionSlider_sliderReleased);
    // ��� ����������� ����� ��������� ����� ������� ��� ��������������, �� �� ����� �������� ������� ������
    connect(ui.positionSlider, &QSlider::valueChanged, this, &AudioPlayer::on_positionSlider_valueChanged);
    // ��� ����������� ����� ��������� ����� ������� ��� ��������������, �� �� ����� �������� ������� ������
    connect(ui.positionSlider, &QSlider::valueChanged, this, &AudioPlayer::on_positionSlider_valueChanged);
}

// ���������� ������ AudioPlayer
// ������������ ������ ��� ��������, ��������� � 'new',
// �� � ������ ������ QMediaPlayer � QAudioOutput �������� �������������,
// ��� ��� � ��� ������ ������������ ������ 'this' � ������������.
AudioPlayer::~AudioPlayer()
{
    // ui.AudioPlayerClass ui - ��� ������, �� ��������� �������������
    // ���� �� ui ��� ���������� (Ui::AudioPlayerClass *ui), �� ����� ��� �� 'delete ui;'
}

// ��������������� closeEvent ��� ���������� ��������
void AudioPlayer::closeEvent(QCloseEvent* event)
{
    QSettings settings("ByteWorks", "AudioPlayer"); // ��������� �� �� �������� ����������� � ����������!
    qreal currentVolume = audioOutput->volume();
    settings.setValue("volume", currentVolume);
    qDebug() << "���������� ��������� ��� ��������:" << currentVolume; // ���������� ���������

    // ����������� �������������� ���������, ����� ��� ���� ���������� �������� �� ����
    // ������ ��� ��������, ���� ���������� ����������� ����� ������.
    settings.sync();
    qDebug() << "��������� ����������������.";

    QMainWindow::closeEvent(event);
}

//  ���� ��� �������� ����������
void AudioPlayer::on_actionOpen_File_triggered()
{
    // ��������� ����������� ������ ������ �����.
    // ������ ��������: ������������ ����.
    // ������ ��������: ��������� �������.
    // ������ ��������: ��������� ������� (������ ������ �������� ������� ��� ��������� ��������������).
    // ��������� ��������: ������ ������, ����� ���������� ������ ����������.
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open audio file"), "", tr("Audio files (*.mp3 *.wav *.flac *.ogg);;All files (*.*)"));

    if (!filePath.isEmpty()) { // ���� ������������ ������ ���� � �� ������� ������
        // ������������� ��������� ���� ��� �������� ��� ������.
        // QUrl::fromLocalFile() ����������� ��������� ���� � ����� � ������ URL,
        // ������� QMediaPlayer ����� ������������.
        player->setSource(QUrl::fromLocalFile(filePath));

        // ���������� ��� ���������� ����� � ������ ��������� ����������.
        // QFileInfo(filePath).fileName() ��������� ������ ��� ����� �� ������� ����.
        ui.statusBar->showMessage(tr("File selected: %1").arg(QFileInfo(filePath).fileName()));
    }
}

// ���� ��� ������������ ��������� ��������� ��������������� ������
void AudioPlayer::on_actionPlayPause_triggered() // <--- ������ ���� ���� ����� ��������� ���� Play/Pause
{
    if (player->source().isValid()) {
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause(); // ���� ������, ������ �� �����
        }
        else if (player->playbackState() == QMediaPlayer::PausedState || player->playbackState() == QMediaPlayer::StoppedState) {
            player->play();  // ���� �� ����� ��� �����������, �������������
        }
    }
    else {
        // ���� ���� �� ������, �������� ������� ������ ������ �����
        ui.statusBar->showMessage(tr("No file selected. Please open a file first."), 2000);
    }
}

// ���� ��� ������������ ��������� ��������� ��������������� ������
void AudioPlayer::on_player_playbackStateChanged(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        ui.actionPlayPause->setIcon(pauseIcon); // ���� ������, ���������� ������ "�����"
        ui.statusBar->showMessage(tr("Playing: %1").arg(QFileInfo(player->source().toLocalFile()).fileName()));
        break;
    case QMediaPlayer::PausedState:
        ui.actionPlayPause->setIcon(playIcon);  // ���� �� �����, ���������� ������ "�������������"
        ui.statusBar->showMessage(tr("Paused."));
        break;
    case QMediaPlayer::StoppedState:
        ui.actionPlayPause->setIcon(playIcon);  // ���� �����������, ���������� ������ "�������������"
        // ������� ������ ���������, ���� ����� ��������� ����������
        ui.statusBar->showMessage(tr("Ready."));
        break;
    default:
        // ����� �������� ��������� ������ ���������, ���� ����������
        break;
    }
}

// ���� ��� ���������
void AudioPlayer::on_actionStop_triggered()
{
    if (player->source().isValid()) {
        player->stop(); // ������������� ���������������
        ui.statusBar->showMessage(tr("Stopped."));
    }
    else ui.statusBar->showMessage(tr("First select the file."), 2000);
}

// ��������������� ������� ��� �������������� ������� (��������, 00:00:00)
QString AudioPlayer::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    QTime time(0, 0, 0); // ������� ������ �������, �������������� ������
    time = time.addSecs(static_cast<int>(seconds)); // ��������� �������

    if (seconds < 3600) { // ���� ������ ����, ������ MM:SS
        return time.toString("mm:ss");
    }
    else { // ���� ��� � �����, ������ HH:MM:SS
        return time.toString("hh:mm:ss");
    }
}

// ����, ���������� ��� ��������� ������� ������� ��������������� ������
void AudioPlayer::on_player_positionChanged(qint64 position)
{
    // ��������� �������� ������ ���� ������������ ��� ������ �� �������������
    // ��� ������������� "������" ��������, ����� ����� �������� �������� ��� �� ����� �������������� �������������
    if (!isSeeking) {
        ui.positionSlider->setValue(static_cast<int>(position));
        ui.positionLabel->setText(formatTime(position)); // ��������� ����� �������� ������� ������ �����
    }
    // ���� isSeeking = true, ����� ������� ����� ����������� � on_positionSlider_valueChanged/sliderMoved
}

// ����, ���������� ��� ��������� ����� ������������ ����������
void AudioPlayer::on_player_durationChanged(qint64 duration)
{
    ui.positionSlider->setMaximum(static_cast<int>(duration)); // ������������� ������������ �������� ��������
    ui.durationLabel->setText(formatTime(duration)); // ��������� ����� ����� ������������
}

// ����, ����������, ����� ������������ �������� ������������� ��������
void AudioPlayer::on_positionSlider_sliderPressed()
{
    isSeeking = true; // ������������� ����, ����� ����� �� �������� ��������
}

// ����, ����������, ����� ������������ ��������� ��������
void AudioPlayer::on_positionSlider_sliderReleased()
{
    // ������������� ������� ������ �� �������� ��������
    player->setPosition(ui.positionSlider->value());
    isSeeking = false; // ���������� ����

    // ����� ���������� ��������, ��������� ����� �������� �������,
    // ����� ��� ��������������� ����� ������� ������ (������� ����� ������������ positionChanged)
    ui.positionLabel->setText(formatTime(player->position()));
}

// ����, ���������� ��� ������ ��������� �������� �������� (� ��� ����� ��� ��������������)
// ������ ���� ���� ����� ������ ��������� ����� ������� ��� ��������������
void AudioPlayer::on_positionSlider_valueChanged(int value)
{
    // ���� ������������ ������������� �������� (isSeeking == true),
    // �� ����� ���������� �������� �����, ��������������� ��������� ��������.
    // ��� ������ ����������� ��������� ����� �������.
    if (isSeeking) {
        ui.positionLabel->setText(formatTime(value));
    }
    // � ��������� ������, ���� !isSeeking, ��� ��������� �������� ��������� ���������� (�� player->positionChanged),
    // � ����� ��� ��������� � on_player_positionChanged.
}