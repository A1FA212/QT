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
    ui -> PBMute ->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    m_player = new QMediaPlayer();
    m_player->setVolume(70);
    ui->volume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->SLvolume->setValue(m_player->volume());

    connect(ui->BTPlay,&QPushButton::clicked,this->m_player,&QMediaPlayer::play);
    connect(ui->BTStop,&QPushButton::clicked,this->m_player,&QMediaPlayer::stop);
    connect(ui->BTPause,&QPushButton::clicked,this->m_player,&QMediaPlayer::pause);
    connect(m_player,&QMediaPlayer::durationChanged,this,&Widget::on_durationChanged);
    connect(m_player,&QMediaPlayer::positionChanged,this,&Widget::on_positionChanged);

    m_playlistModel = new QStandardItemModel(this);
    ui->TBPlaylist->setModel(m_playlistModel);
    m_playlistModel->setHorizontalHeaderLabels(QStringList()<<"Audio Track " << "File ");
    ui->TBPlaylist->hideColumn(1);
    ui->TBPlaylist->horizontalHeader()->setStretchLastSection(true);
    ui->TBPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    connect(ui->BTPrev,&QPushButton::clicked,m_playlist, &QMediaPlaylist::previous);
    connect(ui->BTNext,&QPushButton::clicked,m_playlist, &QMediaPlaylist::next);

    connect(m_playlist,&QMediaPlaylist::currentIndexChanged,this,&Widget::on_current_index_changed);
}

Widget::~Widget()
{
    delete ui;
    delete m_player;
    delete m_playlistModel;
    delete m_playlist;
   }

void Widget::on_BTOpen_clicked()
{
    /*QString file = QFileDialog::getOpenFileName(this,"Open File",NULL,"Audio files (*mp3 *.flac)");
    ui->LBLFile->setText(file);
    m_player->setMedia(QUrl::fromLocalFile(file));
    m_player->play();
    m_player->media();
    this->setWindowTitle(QString("MediaPlayer - ").append(file.split('/').last()));*/

   QStringList files = QFileDialog::getOpenFileNames
   (
    this,
   "Open files",
   "S:\\ananas",
   "Audio files (*mp3 *flac);;mp3 (mp3);; FLAC (*flac)"
   );

   for(QString file:files)
   {
       QList<QStandardItem*> items;
       items.append(new QStandardItem(QDir(file).dirName()));
       items.append(new QStandardItem(file));
       m_playlistModel->appendRow(items);
       m_playlist->addMedia(QUrl(file));
   }
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

void Widget::on_PBMute_clicked()
{
    m_player->setMuted(!m_player->isMuted());
    ui->PBMute->setIcon(style()->standardIcon(m_player -> isMuted()? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
}

void Widget::on_positionChanged(qint64 position)
{
    ui->SLprogress->setValue(position);
    QTime qt_position = QTime::fromMSecsSinceStartOfDay(position);
    ui->LBProgress->setText(QString("Progress: ").append(qt_position.toString(position<3600000? "mm:ss" : "hh:mm:ss")));
}


void Widget::on_SLprogress_valueChanged(int value)
{
    if(ui->SLprogress->isSliderDown())
        m_player->setPosition(value);
}

void Widget::on_current_index_changed(int position)
{
    ui->TBPlaylist->selectRow(position);
    QStandardItem* song = m_playlistModel->item(position,0);
    this->setWindowTitle(song->text());
    QStandardItem* file = m_playlistModel->item(position,0);
    ui->LBLFile->setText(QString("Song file: ").append(file->text()));

}

