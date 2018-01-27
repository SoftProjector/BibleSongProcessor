#ifndef SONG_H
#define SONG_H

#include <QList>
#include <QString>
#include <QtSql>
#include <QMessageBox>


class Song
{
public:
    Song();
    QString getSongExport();

    QString title;
    int number;
    QString text;
};

class Songbook
{
public:
    Songbook();
    void addSong(Song song);
    QString printSongbook();
    void exportSongbook(QString path);

    QString title;
    QString info;
    QList<Song> songs;
};

#endif // SONG_H
