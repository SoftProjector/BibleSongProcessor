#include "bible.h"

Verse::Verse()
{
    num = 0;
    text = "";
}

Chapter::Chapter()
{
    num = 0;
    path = "";
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

void Book::clear()
{
    chapters.clear();
    name = "";
    filePath = "";
    chapterCount = 0;
    bookId = 0;
}

Bible::Bible()
{
    copyright =  "Bible Copyright and Permissions Information @%@%The Holy Bible@%Copyright (c) Eternity, God@%All rights reserved.@%"
                "Copyright Options@%@%"
                "Option A: If you think the bible is:@%"
                "    1. the work of men, translators, publishers and nothing to do with Holy Spirit OR@%"
                "    2. acknowledge Holy Spirit being the original author but the original author is dead and 70+ years had passed.@%"
                "    - Then you must abide by what the translators, publishers etc say about their work.@%@%"
                "Option B: If you think the bible is:@%"
                "    1.the work of Holy Spirit through His people OR@%"
                "    2.acknowledge Holy Spirit being the original author and He is still living OR@%"
                "    3.acknowledge that you have Holy Spirit within you.@%"
                "    - Then you must abide by what the Holy Spirit within you says as He is the original author and copyright holder of all Bibles.@%@%"
                "SoftProjector assumes that you agree with Option B.";
}

void Bible::addBook(Book book)
{
    books.append(book);
}

