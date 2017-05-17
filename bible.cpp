#include "bible.h"

Bible::Bible()
{
    copyright = "Imported from BibleQuote Module";
}

void Bible::addBook(Book book)
{
    books.append(book);
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
