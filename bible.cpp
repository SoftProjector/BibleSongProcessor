#include "bible.h"

Verse::Verse()
{
    num = 0;
    text = "";
}

Chapter::Chapter()
{
    num = 0;
}

void Chapter::addVerse(Verse v)
{
    verses.append(v);
}

Book::Book()
{
    name = "";
    filePath = "";
    chapterCount = 0;
    bookId = 0;
}

void Book::setBookId(QString fp)
{

    fp = fp.remove(QRegExp("\\D"));
    bookId = fp.toInt();
}

void Book::addChapter(Chapter c)
{
    chapters.append(c);
}

Bible::Bible()
{
    copyright = "Imported from BibleQuote Module";
}

void Bible::addBook(Book book)
{
    books.append(book);
}

