
#include "bibconv.h"
#include "ui_bibconv.h"

BibConv::BibConv(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BibConv)
{
    ui->setupUi(this);
}

BibConv::~BibConv()
{
    delete ui;
}

void BibConv::on_pushButtonStart_clicked()
{
    if(ui->radioButtonMySword->isChecked())
    {
        importMySword();
    }
    else if(ui->radioButtonBibleQuote->isChecked())
    {
        importBibleQuote();
    }
    else if(ui->radioButtonBibleDatabase->isChecked())
    {
        importBibleDatabase();
    }
}

void BibConv::importBibleDatabase()
{

}

void BibConv::importBibleQuote()
{
    QString bibleDir = ui->lineEdit->text();

    if (bibleDir == "")
    {
        QMessageBox(QMessageBox::Information,"Error","Please enter Bible Dir Name").exec();
        return;
    }
    bibleDir += QDir::separator();
    QFile file(bibleDir + "bibleqt.ini");
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox(QMessageBox::Information,"Error","Error Opening File").exec();
        return;
    }

    QString text;
    Bible bible;
    Book book;
    bool isLast;
    while(!file.atEnd())
    {
        text = QString::fromUtf8(file.readLine());
        isLast = false;
        if(text.startsWith("BibleName"))
            bible.name = trimText(text,"BibleName");
        else if(text.startsWith("BibleShortName"))
            bible.abbr = trimText(text,"BibleShortName");
        else if(text.startsWith("Copyright"))
            bible.copyright += "\n\n" + trimText(text,"Copyright");
        else if(text.startsWith("ChapterSign"))
            bible.chapterDelim = trimText(text,"ChapterSign");
        else if(text.startsWith("VerseSign"))
            bible.verseDelim = "\n\n" + trimText(text,"VerseSign");
        else if(text.startsWith("BookQty"))
            ui->progressBar->setMaximum( trimText(text,"BookQty").toInt());
        else if(text.startsWith("PathName"))
        {
            book.filePath = trimText(text,"PathName");
            book.setBookId(trimText(text,"PathName"));
        }
        else if(text.startsWith("FullName"))
            book.name = trimText(text,"FullName");
        else if(text.startsWith("ChapterQty"))
        {
            book.chapterCount = trimText(text,"ChapterQty").toInt();
            isLast = true;
        }
        if(isLast)
            bible.addBook(book);
    }
    file.close();

    toSingleLine(bible.copyright);

    text = "##spDataVersion:\t1";
    text += "\n##Title:\t" + bible.name;
    text += "\n##Abbreviation:\t" + bible.abbr;
    text += "\n##Information:\t" + bible.copyright;
    text += "\n##RightToLeft:\t0";// + bible.chapterDelim.trimmed() + "\t" + bible.verseDelim.trimmed();

    QString vtext;
    foreach(Book bk,bible.books)
    {
        QString s = QString::number(bk.bookId) + "\t" + bk.name + "\t" + QString::number(bk.chapterCount);
        text += "\n" + s;
        vtext += processBookNRT(bibleDir,bk,bible.chapterDelim.trimmed(),bible.verseDelim.trimmed());
        ui->progressBar->setValue(ui->progressBar->value()+1);
    }
    text += "\n-----" + vtext;
        ui->plainTextEdit->setPlainText(text);

}

QString BibConv::processBookNRT(QString dirPath, Book book, QString cDelim, QString vDelim)
{
    QFile file(dirPath + book.filePath);

    if(!file.open(QIODevice::ReadOnly))
        return "FAILED TO OPEN: " + book.filePath;

    QString line, oline, cline, chapter,cd,vd,v;
    QRegExp rx;
    bool isVerse = false;
    //pre delimeters
    rx.setPattern("<(.*)>");
    int rxi = rx.indexIn(cDelim);
    if(rxi >=0)
        cd = rx.cap(1);
    rxi = rx.indexIn(vDelim);
    if(rxi >=0)
        vd = rx.cap(1);

    rx.setMinimal(true);
    int ch,vr;
    while(!file.atEnd())
    {
        line = QString::fromUtf8(file.readLine());
        line = line.trimmed();
        //chapter += "\n" + line;
        if(line.startsWith(cDelim.trimmed()))
        {
            isVerse = false;
//            rx.setPattern("<" + cd +">(.*)</" + cd + ">");
//            rxi = rx.indexIn(line);
//            if(rxi >=0)
//                ch = rx.cap(1).toInt();
            ch = line.remove(QRegExp("\\D")).toInt();

        }
        else if(line.startsWith(vDelim.trimmed()))
        {
            isVerse = true;
            oline = line;

            rx.setPattern("<" + vd +">(\\d*)\\s");
            rxi = rx.indexIn(line);
            if(rxi >=0)
                vr = rx.cap(1).toInt();

            v = line.remove(rx);
            rx.setPattern("<b.*b>");
            v = v.remove(rx);
            rx.setPattern("<.*>");
            v = v.remove(rx);
            v = v.trimmed();
            rx.setPattern("^\\d*\\s");
            v = v.remove(rx);
            cline = "\nB" + get3(book.bookId) + "C" + get3(ch) + "V" + get3(vr) + "\t" +
                                QString::number(book.bookId) + "\t" + QString::number(ch) + "\t" + QString::number(vr) + "\t" + v.trimmed();
            chapter += cline;

        }
        else
        {
            if(isVerse)
            {
                oline += " " + line;
                line = oline.simplified();
                chapter.remove(cline);


                v = line.remove(rx);
                rx.setPattern("<b.*b>");
                v = v.remove(rx);
                rx.setPattern("<.*>");
                v = v.remove(rx);
                v = v.trimmed();
                rx.setPattern("^\\d*\\s");
                v = v.remove(rx);
                cline = "\nB" + get3(book.bookId) + "C" + get3(ch) + "V" + get3(vr) + "\t" +
                                    QString::number(book.bookId) + "\t" + QString::number(ch) + "\t" + QString::number(vr) + "\t" + v.trimmed();
                chapter += cline;
            }

        }


    }
    return chapter;
}

QString BibConv::processBookRBO2011(QString dirPath, Book book, QString cDelim, QString vDelim)
{
    QFile file(dirPath + book.filePath);

    if(!file.open(QIODevice::ReadOnly))
        return "FAILED TO OPEN: " + book.filePath;

    QString line, oline, cline, chapter,cd,vd,v;
    QRegExp rx;
    bool isVerse = false;
    //pre delimeters
    rx.setPattern("<(.*)>");
    int rxi = rx.indexIn(cDelim);
    if(rxi >=0)
        cd = rx.cap(1);
    rxi = rx.indexIn(vDelim);
    if(rxi >=0)
        vd = rx.cap(1);

    rx.setMinimal(true);
    int ch,vr;
    while(!file.atEnd())
    {
        line = QString::fromUtf8(file.readLine());
        line = line.trimmed();
        //chapter += "\n" + line;
        if(line.startsWith(cDelim.trimmed()))
        {
            isVerse = false;
            rx.setPattern("<" + cd +">(.*)</" + cd + ">");
            rxi = rx.indexIn(line);
            if(rxi >=0)
                ch = rx.cap(1).toInt();

        }
        else if(line.startsWith(vDelim.trimmed()))
        {
            isVerse = true;
            oline = line;

            rx.setPattern("<" + vd +">(.*)</" + vd + ">");
            rxi = rx.indexIn(line);
            if(rxi >=0)
                vr = rx.cap(1).toInt();

            v = line.remove(rx);
            rx.setPattern("<a.*a>");
            v = v.remove(rx);
            rx.setPattern("<.*>");
            v = v.remove(rx);
            cline = "\nB" + get3(book.bookId) + "C" + get3(ch) + "V" + get3(vr) + "\t" +
                                QString::number(book.bookId) + "\t" + QString::number(ch) + "\t" + QString::number(vr) + "\t" + v.trimmed();
            chapter += cline;

        }
        else
        {
            if(isVerse)
            {
                oline += " " + line;
                line = oline.simplified();
                chapter.remove(cline);

                rx.setPattern("<" + vd +">(.*)</" + vd + ">");
                rxi = rx.indexIn(line);
                if(rxi >=0)
                    vr = rx.cap(1).toInt();

                v = line.remove(rx);
                rx.setPattern("<a.*a>");
                v = v.remove(rx);
                rx.setPattern("<.*>");
                v = v.remove(rx);
                cline = "\nB" + get3(book.bookId) + "C" + get3(ch) + "V" + get3(vr) + "\t" +
                                    QString::number(book.bookId) + "\t" + QString::number(ch) + "\t" + QString::number(vr) + "\t" + v.trimmed();
                chapter += cline;
            }

        }


    }
    return chapter;
}

void BibConv::importMySword()
{
    QSqlQuery sq;
    sq.exec("SELECT Book, Chapter, Verse, Scripture FROM Bible");
    int i(0);
    ui->progressBar->setMaximum(31102);
    QString alls,books, book, bkOld,bookOld,bible,chOld;
    bookOld = "";
    chOld = "null";
    while(sq.next())
    {
        QString st,bk, ch, vn, vt;
        int b, c, v;
        bk = sq.value(0).toString();
        vt = processLineMySword( sq.value(3).toString(),book);
        ch = sq.value(1).toString();
        vn = sq.value(2).toString();

        if(bkOld != bk)
        {
            if(chOld != "null")
                books += bkOld + "\t" + bookOld + "\t" + chOld + "\n";
             bkOld = bk;
             bookOld = book;
//             chOld = ch;
        }
//        bkOld = bk;
//        bookOld = book;
        chOld = ch;

        b = sq.value(0).toInt();
        c = sq.value(1).toInt();
        v = sq.value(2).toInt();

        st = "B" + get3(b) + "C" + get3(c) + "V" + get3(v) + "\t"+ bk + "\t" + ch + "\t" + vn + "\t" + vt;
        //bible += st + "\n";
        if(vt.trimmed() != "")
            bible += st + "\n";

        ++i;
        ui->progressBar->setValue(i);
//        if(i == 10000)
//            break;
    }
    books += bkOld + "\t" + bookOld + "\t" + chOld + "\n";
    alls = books + "-----\n" + bible;
    ui->plainTextEdit->setPlainText(alls);
}

QString BibConv::processLineMySword(QString line, QString &book)
{
    QRegExp rx;
    rx.setMinimal(true);
   // rx.setPatternSyntax(QRegExp::RegExp2);
    QString rxs = ui->lineEdit->text();
    rx.setPattern("(<TS2>(.*)\\s\\d<Ts>)");//    (<TS2>(.*)<Ts>)
    int rxi = rx.indexIn(line);
    if(rxi >=0)
        book = rx.cap(2);


    QString vr,v ;
    rx.setPattern("<RF.*Rf>|<TS.*Ts>|<v1.*v1>");
//    vr = line.remove(rx);
    vr = line.replace(rx," ");
    rx.setPattern("<.*>");
//    vr = line.remove(rx);
    vr = line.replace(rx," ");
    vr = vr.simplified();

    int p(0);

//    rx.setPattern("((\\.)(\\S))"); // Do Dot
//    while ((p = rx.indexIn(vr, p)) != -1)
//    {
//        QString a,b,c;
//        a = rx.cap(1);
//        b = rx.cap(2);
//        c = rx.cap(3);
//        vr = vr.replace(a,b + " " + c);
//        v += a;
//        p += rx.matchedLength();
//    }

        p = 0;
        rx.setPattern("((\\,)(\\S))"); // Do Comma
        while ((p = rx.indexIn(vr, p)) != -1)
        {
            QString a,b,c;
            a = rx.cap(1);
            b = rx.cap(2);
            c = rx.cap(3);
            vr = vr.replace(a,b + " " + c);
//            v += a;
            p += rx.matchedLength();
        }

//        p = 0;
//        rx.setPattern("((!)(\\S))"); // Do Exlamation
//        while ((p = rx.indexIn(vr, p)) != -1)
//        {
//            QString a,b,c;
//            a = rx.cap(1);
//            b = rx.cap(2);
//            c = rx.cap(3);
//            vr = vr.replace(a,b + " " + c);
//            v += a;
//            p += rx.matchedLength();
//        }

//        p = 0;
//        rx.setPattern("((\\?)(\\S))"); // Do Question
//        while ((p = rx.indexIn(vr, p)) != -1)
//        {
//            QString a,b,c;
//            a = rx.cap(1);
//            b = rx.cap(2);
//            c = rx.cap(3);
//            vr = vr.replace(a,b + " " + c);
//            v += a;
//            p += rx.matchedLength();
//        }

//        p = 0;
//        rx.setPattern("((:)(\\S))"); // Do colon
//        while ((p = rx.indexIn(vr, p)) != -1)
//        {
//            QString a,b,c;
//            a = rx.cap(1);
//            b = rx.cap(2);
//            c = rx.cap(3);
//            vr = vr.replace(a,b + " " + c);
//            v += a;
//            p += rx.matchedLength();
//        }

//        p = 0;
//        rx.setPattern("((;)(\\S))"); // Do semi-colon
//        while ((p = rx.indexIn(vr, p)) != -1)
//        {
//            QString a,b,c;
//            a = rx.cap(1);
//            b = rx.cap(2);
//            c = rx.cap(3);
//            vr = vr.replace(a,b + " " + c);
//            v += a;
//            p += rx.matchedLength();
//        }


//    p = 0;
//    rx.setPattern("((;)(\\S))"); // Do ------
//    while ((p = rx.indexIn(vr, p)) != -1)
//    {
//        QString a,b,c;
//        a = rx.cap(1);
//        b = rx.cap(2);
//        c = rx.cap(3);

//        vr = vr.replace(a,b + " " + c);

//        //        if(!vr.contains(QRegExp("\\S"+a)))
//        v += a;
//        p += rx.matchedLength();
//    }

//    if(v.trimmed() != "")
//        v = v + " == " + vr;

    return vr;
}

QString BibConv::get3(int i)
{
    QString st;
    if (i>=100)
    {
        st = st.number(i);
    }
    else if (i>=10)
    {
        st = "0" + st.number(i);
    }
    else
    {
        st = "00" + st.number(i);
    }
    return st;
}

QString BibConv::trimText(QString original, QString trimTxt)
{
    original = original.remove(trimTxt.trimmed() + " =");
    return original.trimmed();
}

//QString BibConv::trimNRTText(QString original, QString trimTxt)
//{
//    original = original.remove(trimTxt.trimmed() + " =");
//    return original.trimmed();
//}

void BibConv::toSingleLine(QString &sline)
{
    QStringList line_list = sline.split("\n");
    sline = line_list[0];
    for(int i(1); i<line_list.size();++i)
        sline += "@%" + line_list[i];

    sline = sline.trimmed();
}
