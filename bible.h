#ifndef BIBLE_H
#define BIBLE_H

#include <QString>
#include <QList>
#include <QRegExp>


class Book
{
public:
    Book();

    QString name;
    QString filePath;
    int chapterCount;
    int bookId;

    void setBookId(QString fp);
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
