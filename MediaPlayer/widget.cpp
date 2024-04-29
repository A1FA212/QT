#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QStyle>
#include <QTime>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui -> BTPlay ->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui -> BTPause ->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui -> BTStop ->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui -> BTPrev ->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui -> BTNext ->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));


    m_player = new QMediaPlayer();
    m_player->setVolume(70);
    ui->volume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->SLvolume->setValue(m_player->volume());

    connect(ui->BTPlay,&QPushButton::clicked,this->m_player,&QMediaPlayer::play);
    connect(ui->BTStop,&QPushButton::clicked,this->m_player,&QMediaPlayer::stop);
    connect(ui->BTPause,&QPushButton::clicked,this->m_player,&QMediaPlayer::pause);
    connect(m_player,&QMediaPlayer::durationChanged,this,&Widget::on_durationChanged);
}

Widget::~Widget()
{
    delete ui;
    delete m_player;
}

void Widget::on_BTOpen_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,"Open File",NULL,"Audio files (*mp3 *.flac)");
    ui->LBLFile->setText(file);
    m_player->setMedia(QUrl::fromLocalFile(file));
    m_player->play();
}

void Widget::on_SLvolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->volume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
}

void Widget::on_durationChanged(qint64 duration)
{
    ui->SLprogress->setMaximum(duration);
    QTime qt_duration = QTime::fromMSecsSinceStartOfDay(duration);
    ui->LBdur->setText(QString("Duration: ").append(qt_duration.toString(duration < 3600000? "mm:ss" : "hh:mm:ss")));
}
