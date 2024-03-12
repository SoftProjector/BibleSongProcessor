#ifndef BIBLE_H
#define BIBLE_H

#include <QString>
#include <QList>
#include <QRegularExpression>

class Verse
{
public:
    Verse();
    int num;
    QString text;
};

class Chapter
{
public:
    Chapter();

    int num;
    QString path;
    QList<Verse> verses;
    void addVerse(Verse v);
    void clear();
};

class Book
{
public:
    Book();

    QString name;
    QString filePath;
    int chapterCount;
    int bookId;
    QList<Chapter> chapters;

    void setBookId(QString fp);
    void addChapter(Chapter c);
    void clear();
};

class Bible
{
public:
    Bible();

    QString name;
    QString abbr;
    QString copyright;
    QString chapterDelim;
    QString verseDelim;

    QList<Book> books;

    void addBook(Book book);
};

#endif // BIBLE_H
