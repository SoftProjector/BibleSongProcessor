#include "song.h"

Song::Song()
{
    number = 0;
    title = "";
    text = "";
}

QString Song::getSongExport()
{

}


Songbook::Songbook()
{
    title = "";
    info = "";
}

void Songbook::addSong(Song song)
{
    songs.append(song);
}

QString Songbook::printSongbook()
{
    QString psb;
    foreach (Song s, songs)
    {
        psb += QString::number(s.number) + " - " + s.title + "\n";
    }
    return psb.trimmed();
}

void Songbook::exportSongbook(QString path)
{
    // First Delete file if one already exists
    if(QFile::exists(path))
    {
        if(!QFile::remove(path))
        {
            qDebug()<<"An error has ocured when overwriting existing file. Please try again with different file name.";
            return;
        }
    }

    {
        // Prepare SQLite songbook database file
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","sps");
        db.setDatabaseName(path);
        if(db.open())
        {
            db.transaction();
            QSqlQuery q(db);
            q.exec("PRAGMA user_version = 2");
            q.exec("CREATE TABLE 'SongBook' ('title' TEXT, 'info' TEXT)");
            q.exec("CREATE TABLE 'Songs' ('number' INTEGER, 'title' TEXT, 'category' INTEGER DEFAULT 0, "
                   "'tune' TEXT, 'words' TEXT, 'music' TEXT, 'song_text' TEXT, 'notes' TEXT, "
                   "'use_private' BOOL, 'alignment_v' INTEGER, 'alignment_h' INTEGER, 'color' INTEGER, 'font' TEXT, "
                   "'info_color' INTEGER, 'info_font' TEXT, 'ending_color' INTEGER, 'ending_font' TEXT, "
                   "'use_background' BOOL, 'background_name' TEXT, 'background' BLOB, 'count' INTEGER DEFAULT 0, 'date' TEXT)");

            // Write SongBook information
            q.prepare("INSERT INTO SongBook (title,info) VALUES(?,?)");
            q.addBindValue(title);
            q.addBindValue(info);
            q.exec();
            q.clear();


            // Write Songs
//            q.prepare("INSERT INTO Songs (number,title,category,tune,words,music,song_text,notes,"
//                      "use_private,alignment_v,alignment_h,color,font,info_color,info_font,ending_color,ending_font,"
//                      "use_background,background_name,background,count,date) "
//                      "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
            q.prepare("INSERT INTO Songs (number,title,song_text) "
                      "VALUES(?,?,?)");
            foreach (Song s, songs)
            {
                q.addBindValue(s.number);
                q.addBindValue(s.title);
//                q.addBindValue(sq.record().value("category"));
//                q.addBindValue(sq.record().value("tune"));
//                q.addBindValue(sq.record().value("words"));
//                q.addBindValue(sq.record().value("music"));
                q.addBindValue(s.text);
//                q.addBindValue(sq.record().value("notes"));
//                q.addBindValue(sq.record().value("use_private"));
//                q.addBindValue(sq.record().value("alignment_v"));
//                q.addBindValue(sq.record().value("alignment_h"));
//                q.addBindValue(sq.record().value("color"));
//                q.addBindValue(sq.record().value("font"));
//                q.addBindValue(sq.record().value("info_color"));
//                q.addBindValue(sq.record().value("info_font"));
//                q.addBindValue(sq.record().value("ending_color"));
//                q.addBindValue(sq.record().value("ending_font"));
//                q.addBindValue(sq.record().value("use_background"));
//                q.addBindValue(sq.record().value("background_name"));
//                q.addBindValue(sq.record().value("background"));
//                q.addBindValue(sq.record().value("count"));
//                q.addBindValue(sq.record().value("date"));
                q.exec();
            }
            db.commit();
        }
    }
    QSqlDatabase::removeDatabase("sps");

    qDebug()<<"The songbook - " << title << " - Has been saved to:" << path;

}
