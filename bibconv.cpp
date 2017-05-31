
#include "bibconv.h"
#include "ui_bibconv.h"

BibConv::BibConv(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BibConv)
{
    ui->setupUi(this);
    bConType = 0;
}

BibConv::~BibConv()
{
    delete ui;
}

void BibConv::on_comboBox_activated(int index)
{
    QString fn = "";

    bConType = index;

    switch (bConType) {
    case ZAFENIA_XML:
    case CORPUS_XML:
    case OSIS_XML:
        fn = QFileDialog::getOpenFileName(this, "Open XML file", "./xml", "*.xml");
        if(fn.isNull())
        {
            ui->lineEdit->setText("Error opening xml file. Please try again.");
            return;
        }
        else
        {
            ui->lineEdit->setText(fn);
        }
        break;
    default:
        break;
    }
}

void BibConv::on_pushButtonStart_clicked()
{
    switch (bConType) {
    case MY_SWORD:
        importMySword();
        break;
    case BIBLE_QUOTE:
        importBibleQuote();
        break;
    case BIBLE_DATABASE:
        importBibleDatabase();
        break;
    case ZAFENIA_XML:
        importXml(ui->lineEdit->text());
        break;
    case CORPUS_XML:

        break;
    case OSIS_XML:
         importOsisXml(ui->lineEdit->text());
        break;
    case CSB_XML:
        break;
    default:
        break;
    }

}

void BibConv::importBibleDatabase()
{
    QString pathToTxt = QFileDialog::getOpenFileName(this,tr("Open Bible"), "", tr("txt Files (*.txt)"));
    QString pathToHtml = QFileDialog::getOpenFileName(this,tr("Open Open Headers"), "", tr("Html Files (*.html *.htm)"));

    ui->plainTextEdit->appendPlainText(pathToTxt);
    ui->plainTextEdit->appendPlainText(pathToHtml);

    // Get bible names
    QFile file(pathToHtml);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox(QMessageBox::Warning,"Error","Error Opening File").exec();
        return;
    }

    Bible bible;
    Book book;
    QString line, convBible;
    int bkId(0);

    bool bookStart = false;

    while (!file.atEnd())
    {
        line = file.readLine();
        if(line.startsWith("<title"))
        {
            bible.name = line.trimmed();
        }

        if(line.startsWith("<block"))
        {
            bookStart = true;
        }
        if(line.startsWith("</block"))
        {
            bookStart = false;
        }

        convBible = "##spDataVersion:\t1";
        convBible += "\n##Title:\t" + bible.name;
        convBible += "\n##Abbreviation:\t" + bible.abbr;
        convBible += "\n##Information:\t" + bible.copyright;
        convBible += "\n##RightToLeft:\t0";// + bible.chapterDelim.trimmed() + "\t" + bible.verseDelim.trimmed();

        if(bookStart)
        {
            if(!line.startsWith("[<a") && line != "" && (!line.startsWith("<b")))
            {
                ++bkId;
                book.name = line.trimmed();
                book.bookId = bkId;
                bible.addBook(book);

                //                ui->plainTextEdit->appendPlainText(QString::number(book.bookId) + " " + book.name);
                convBible += QString::number(book.bookId) + "\t" + book.name;
            }
        }
    }
    file.close();

    convBible += "\n-----";

    // Get Bible text
    file.setFileName(pathToTxt);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox(QMessageBox::Warning,"Error","Error Opening File").exec();
        return;
    }
    ui->progressBar->setMaximum(66);
    while(!file.atEnd())
    {
        line = file.readLine().trimmed();
        QStringList list;
        list = line.split("\t");
        QString v = list.at(4);
        v = v.remove("&para;");
        QString bCode = "B" + get3(list.at(1).toInt()) + "C" + get3(list.at(2).toInt()) + "V" + get3(list.at(3).toInt());
        convBible += bCode + "\t" + list.at(1) + "\t" + list.at(2) + "\t" + list.at(3) + "\t" + v.trimmed() + "\n";
        ui->progressBar->setValue(list.at(1).toInt());
    }
    file.close();


    ui->plainTextEdit->appendPlainText(convBible);
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

void BibConv::importXml(QString fileName)
{
    QXmlStreamReader xmlreader;
    QFile file (fileName);
    QString oline,bb,b,c,v,vt,bO,cO,bbO,abbr,l,lo;
    bO="x";
    QString out,books,info;


    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open file: " << fileName;
        //        abort();
        return;
    }


    xmlreader.setDevice(&file);

    int xc(0), ct(0);
    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    while(!f.atEnd())
    {
        QString s = f.readLine();
        ++ct;
    }
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(ct);


    while (!xmlreader.atEnd())
    {
        QXmlStreamReader::TokenType tt = xmlreader.readNext();

        switch (tt)
        {
        case QXmlStreamReader::StartElement:
        {
            ++xc;
            ui->progressBar->setValue(xc);
            QXmlStreamAttributes attrs = xmlreader.attributes();
            foreach (QXmlStreamAttribute attr, attrs)
            {
                if(attr.name().toString()=="cnumber")
                    c = attr.value().toString();
                else if (attr.name().toString()=="vnumber")
                    v = attr.value().toString();
                else if (attr.name().toString()=="bnumber")
                    b = attr.value().toString();
                else if (attr.name().toString()=="bname")
                    bb = attr.value().toString();
                else if (attr.name().toString()=="biblename")
                    abbr = attr.value().toString();
            }
            break;
        }
        case QXmlStreamReader::Characters:
        {
            vt = xmlreader.text().toString();
            break;
        }
        case QXmlStreamReader::EndElement:
        {
            if(bO!="x" && b!=bO)
                books += QString("%1\t%2\t%3\n").arg(bO).arg(bbO).arg(cO);

            bO=b;
            bbO=bb;
            cO=c;

            vt = vt.simplified();
            if(!vt.isEmpty())
            {
                l = QString("B%1C%2V%3\t%4\t%5\t%6\t%7\n").arg(get3(b.toInt())).arg(get3(c.toInt())).arg(get3(v.toInt())).arg(b).arg(c).arg(v).arg(vt);
                if(l.length()>10 && !l.startsWith(lo))
                    oline += l;
                lo = l;
                lo.resize(12);
            }
            break;
        }
        case QXmlStreamReader::EndDocument:
        default:
            break;
        }
    }
    books += QString("%1\t%2\t%3\n").arg(b).arg(bb).arg(c);

    QFileInfo fi(f);
    QString title = fi.fileName();
    title = title.remove(".xml",Qt::CaseInsensitive);
    //    ui->labelTitle->setText(title);
    abbr = abbr.remove("ENGLISH",Qt::CaseInsensitive);

    //    if(ui->radioButtonSword->isChecked())
    //        info = "Imported from The SWORD Poject file from http://www.crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles@%--------------------------@%";
    //    else if(ui->radioButtonChurchsw->isChecked())
    //        info = "Imported from XML file from http://www.churchsw.org/bibles@%--------------------------@%";
    //    info += "Bible Copyright and Permissions Information @%@%The Holy Bible@%Copyright (c) Eternity, God@%All rights reserved.@%"
    //            "Copyright Options@%@%"
    //            "Option A: If you think the bible is:@%"
    //            "    1. the work of men, translators, publishers and nothing to do with Holy Spirit OR@%"
    //            "    2. acknowledge Holy Spirit being the original author but the original author is dead and 70+ years had passed.@%"
    //            "    - Then you must abide by what the translators, publishers etc say about their work.@%@%"
    //            "Option B: If you think the bible is:@%"
    //            "    1.the work of Holy Spirit through His people OR@%"
    //            "    2.acknowledge Holy Spirit being the original author and He is still living OR@%"
    //            "    3.acknowledge that you have Holy Spirit within you.@%"
    //            "    - Then you must abide by what the Holy Spirit within you says as He is the original author and copyright holder of all Bibles.@%@%"
    //            "SoftProjector assumes that you agree with Option B.";

    out = "##spDataVersion:\t1\n";
    out += "##Title:\t" + title +"\n";
    out += "##Abbreviation:\t" + abbr +"\n";
    //    out += "##Information:\t" + info +"\n";
    out += "##Information:\tImported from XML file from http://www.churchsw.org/bibles\n";
    out += "##RightToLeft:\t\n";
    out += books;
    out += "-----\n";
    out += oline.trimmed();
    ui->progressBar->setValue(ct*99.5/100);
    ui->plainTextEdit->setPlainText(out);
    //    btext = out;
    ui->progressBar->setValue(ct);
}

void BibConv::importOsisXml(QString fileName)
{
    QDomDocument domDoc;
    QFile file (fileName);
    QString oline,bb,b,c,v,vt,bO,cO,bbO,abbr,l,lo;
    bO="x";
    QString out,books,info;
    Bible bible;


    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open file: " << fileName;
        ui->lineEdit->setText("Can't open file: " + fileName);
        return;
    }


    if(!domDoc.setContent(&file,false))
    {
        ui->lineEdit->setText("Error XML DOM Document open");
        file.close();
        return;
    }
    file.close();

//    xmlreader.setDevice(&file);

    int xc(0), ct(0);
    ui->progressBar->setMaximum(272000);

    QDomElement domElem = domDoc.documentElement();
    QDomNode n = domElem.firstChild();

    while(!n.isNull())
    {
        if("osisText" == n.nodeName())
        {
            QDomNode nOT = n.firstChild();
            while(!nOT.isNull())
            {
                if("header" == nOT.nodeName())
                {
                    QDomNode nh = nOT.firstChild();
                    bool firstWork = true;
                    while (!nh.isNull())
                    {
                        if("work" == nh.nodeName() && firstWork)
                        {
                            firstWork = false;
                            QDomElement ew = nh.toElement();
                            bible.abbr = ew.attribute("osisWork");

                            QDomNode nw = nh.firstChild();
                            while (!nw.isNull())
                            {
                                if("title" == nw.nodeName())
                                {
                                    bible.name = nw.toElement().text();
                                }
                                ++ct;
                                ui->progressBar->setValue(ct);
                                nw = nw.nextSibling();
                            }
                        }

                        ++ct;
                        ui->progressBar->setValue(ct);
                        nh = nh.nextSibling();
                    }
                }
                else if("div" == nOT.nodeName() && "x-testament" == nOT.toElement().attribute("type"))
                {
                    QDomNode nt = nOT.firstChild();
                    int bkNum = 0;

                    while (!nt.isNull())
                    {
                        if("div" == nt.nodeName() &&  "book" == nt.toElement().attribute("type"))
                        {
                            Book book;
                            book.name = nt.toElement().attribute("osisID");
                            updateOsisBibleName(book.name,book.bookId);

                            QDomNode nc = nt.firstChild();

                            while (!nc.isNull())
                            {
                                if("chapter" == nc.nodeName())
                                {
                                    Chapter chap;
                                    QString cid = nc.toElement().attribute("osisID");
                                    QStringList cidlist = cid.split(".");
                                    chap.num = cidlist.at(cidlist.count()-1).toInt();

                                    QDomNode nv = nc.firstChild();

                                    while (!nv.isNull())
                                    {
                                        if("verse" == nv.nodeName())
                                        {
                                            for(int icn(0);icn<nv.childNodes().count();++icn)
                                            {
                                                QDomNode nvc = nv.childNodes().at(icn);
                                                if("title"==nvc.nodeName() ||
                                                        "milestone"==nvc.nodeName() ||
                                                        "reference"==nvc.nodeName() ||
                                                        "note"==nvc.nodeName())
                                                {
                                                  nv.removeChild(nv.childNodes().at(icn));
                                                }

                                                ++ct;
                                                ui->progressBar->setValue(ct);
                                            }

                                            // Run delete child nodes again. first loop sometimes leaves child nodes undeleted.
                                            for(int icn(0);icn<nv.childNodes().count();++icn)
                                            {
                                                QDomNode nvc = nv.childNodes().at(icn);
                                                if("title"==nvc.nodeName() ||
                                                        "milestone"==nvc.nodeName() ||
                                                        "reference"==nvc.nodeName() ||
                                                        "note"==nvc.nodeName())
                                                {
                                                  nv.removeChild(nv.childNodes().at(icn));
                                                }
                                            }

                                            Verse v;
                                            QString vid = nv.toElement().attribute("osisID");
                                            QStringList vidlist = vid.split(".");
                                            v.num = vidlist.at(vidlist.count()-1).toInt();
                                            v.text = nv.toElement().text();
                                            chap.addVerse(v);
                                        }

                                        ++ct;
                                        ui->progressBar->setValue(ct);
                                        nv = nv.nextSibling();
                                    }

                                    ++book.chapterCount;
                                    book.addChapter(chap);
                                }
                                ++ct;
                                ui->progressBar->setValue(ct);
                                nc = nc.nextSibling();
                            }

                            bible.addBook(book);
                        }
                        ++ct;
                        ui->progressBar->setValue(ct);
                        nt = nt.nextSibling();
                    }
                }
                ++ct;
                ui->progressBar->setValue(ct);
                nOT = nOT.nextSibling();
            }
        }
        ++ct;
        ui->progressBar->setValue(ct);
        n = n.nextSibling();
    }

   ui->plainTextEdit->setPlainText(printBible(bible));
   ui->progressBar->setValue(ui->progressBar->maximum());
    //    btext = out;
//    ui->progressBar->setValue(ct);
}

void BibConv::updateOsisBibleName(QString &bName, int &bNum)
{
    if("Gen" == bName)
    {
        bName = "Genesis";
        bNum = 1;
    }
    else if("Exod" == bName)
    {
        bName = "Exodus";
        bNum = 2;
    }
    else if("Lev" == bName)
    {
        bName = "Leviticus";
        bNum = 3;
    }
    else if("Num" == bName)
    {
        bName = "Numbers";
        bNum = 4;
    }
    else if("Deut" == bName)
    {
        bName = "Deuteronomy";
        bNum = 5;
    }
    else if("Josh" == bName)
    {
        bName = "Joshua";
        bNum = 6;
    }
    else if("Judg" == bName)
    {
        bName = "Judges";
        bNum = 7;
    }
    else if("Ruth" == bName)
    {
        bName = "Ruth";
        bNum = 8;
    }
    else if("1Sam" == bName)
    {
        bName = "1 Samuel";
        bNum = 9;
    }
    else if("2Sam" == bName)
    {
        bName = "2 Samuel";
        bNum = 10;
    }
    else if("1Kgs" == bName)
    {
        bName = "1 Kings";
        bNum = 11;
    }
    else if("2Kgs" == bName)
    {
        bName = "2 Kings";
        bNum = 12;
    }
    else if("1Chr" == bName)
    {
        bName = "1 Chronicles";
        bNum = 13;
    }
    else if("2Chr" == bName)
    {
        bName = "2 Chronicles";
        bNum = 14;
    }
    else if("Ezra" == bName)
    {
        bName = "Ezra";
        bNum = 15;
    }
    else if("Neh" == bName)
    {
        bName = "Nehemiah";
        bNum = 16;
    }
    else if("Esth" == bName)
    {
        bName = "Esther";
        bNum = 17;
    }
    else if("Job" == bName)
    {
        bName = "Job";
        bNum = 18;
    }
    else if("Ps" == bName)
    {
        bName = "Psalms";
        bNum = 19;
    }
    else if("Prov" == bName)
    {
        bName = "Proverbs";
        bNum = 20;
    }
    else if("Eccl" == bName)
    {
        bName = "Ecclesiastes";
        bNum = 21;
    }
    else if("Song" == bName)
    {
        bName = "Song of Solomon";
        bNum = 22;
    }
    else if("Isa" == bName)
    {
        bName = "Isaiah";
        bNum = 23;
    }
    else if("Jer" == bName)
    {
        bName = "Jeremiah";
        bNum =24;
    }
    else if("Lam" == bName)
    {
        bName = "Lamentations";
        bNum = 25;
    }
    else if("Ezek" == bName)
    {
        bName = "Ezekiel";
        bNum = 26;
    }
    else if("Dan" == bName)
    {
        bName = "Daniel";
        bNum = 27;
    }
    else if("Hos" == bName)
    {
        bName = "Hosea";
        bNum = 28;
    }
    else if("Joel" == bName)
    {
        bName = "Joel";
        bNum = 29;
    }
    else if("Amos" == bName)
    {
        bName = "Amos";
        bNum = 30;
    }
    else if("Obad" == bName)
    {
        bName = "Obadiah";
        bNum = 31;
    }
    else if("Jonah" == bName)
    {
        bName = "Jonah";
        bNum = 32;
    }
    else if("Mic" == bName)
    {
        bName = "Micah";
        bNum = 33;
    }
    else if("Nah" == bName)
    {
        bName = "Nahum";
        bNum = 34;
    }
    else if("Hab" == bName)
    {
        bName = "Habakkuk";
        bNum = 35;
    }
    else if("Zeph" == bName)
    {
        bName = "Zephaniah";
        bNum = 36;
    }
    else if("Hag" == bName)
    {
        bName = "Haggai";
        bNum = 37;
    }
    else if("Zech" == bName)
    {
        bName = "Zechariah";
        bNum = 38;
    }
    else if("Mal" == bName)
    {
        bName = "Malachi";
        bNum = 39;
    }
    else if("Matt" == bName)
    {
        bName = "Matthew";
        bNum = 40;
    }
    else if("Mark" == bName)
    {
        bName = "Mark";
        bNum = 41;
    }
    else if("Luke" == bName)
    {
        bName = "Luke";
        bNum = 42;
    }
    else if("John" == bName)
    {
        bName = "John";
        bNum = 43;
    }
    else if("Acts" == bName)
    {
        bName = "Acts";
        bNum = 44;
    }
    else if("Rom" == bName)
    {
        bName = "Romans";
        bNum = 45;
    }
    else if("1Cor" == bName)
    {
        bName = "1 Corinthians";
        bNum = 46;
    }
    else if("2Cor" == bName)
    {
        bName = "2 Corinthians";
        bNum = 47;
    }
    else if("Gal" == bName)
    {
        bName = "Galatians";
        bNum = 48;
    }
    else if("Eph" == bName)
    {
        bName = "Ephesians";
        bNum = 49;
    }
    else if("Phil" == bName)
    {
        bName = "Philippians";
        bNum = 50;
    }
    else if("Col" == bName)
    {
        bName = "Colossians";
        bNum = 51;
    }
    else if("1Thess" == bName)
    {
        bName = "1 Thessalonians";
        bNum = 52;
    }
    else if("2Thess" == bName)
    {
        bName = "2 Thessalonians";
        bNum = 53;
    }
    else if("1Tim" == bName)
    {
        bName = "1 Timothy";
        bNum = 54;
    }
    else if("2Tim" == bName)
    {
        bName = "2 Timothy";
        bNum = 55;
    }
    else if("Titus" == bName)
    {
        bName = "Titus";
        bNum = 56;
    }
    else if("Phlm" == bName)
    {
        bName = "Philemon";
        bNum = 57;
    }
    else if("Heb" == bName)
    {
        bName = "Hebrews";
        bNum = 58;
    }
    else if("Jas" == bName)
    {
        bName = "James";
        bNum = 59;
    }
    else if("1Pet" == bName)
    {
        bName = "1 Peter";
        bNum = 60;
    }
    else if("2Pet" == bName)
    {
        bName = "2 Peter";
        bNum = 61;
    }
    else if("1John" == bName)
    {
        bName = "1 John";
        bNum = 62;
    }
    else if("2John" == bName)
    {
        bName = "2 John";
        bNum = 63;
    }
    else if("3John" == bName)
    {
        bName = "3 John";
        bNum = 64;
    }
    else if("Jude" == bName)
    {
        bName = "Jude";
        bNum = 65;
    }
    else if("Rev" == bName)
    {
        bName = "Revelation";
        bNum = 66;
    }
    else if("Tob" == bName)
    {
        bName = "Tobit";
        bNum = 67;
    }
    else if("Jdt" == bName)
    {
        bName = "Judith";
        bNum = 68;
    }
    else if("Estg" == bName)
    {
        bName = "Esther-Greek";
        bNum = 69;
    }
    else if("Wis" == bName)
    {
        bName = "Wisdom";
        bNum = 70;
    }
    else if("Sir" == bName)
    {
        bName = "Sirach";
        bNum = 71;
    }
    else if("Bar" == bName)
    {
        bName = "Baruch";
        bNum = 72;
    }
    else if("EpJer" == bName)
    {
        bName = "Epistle of Jeremiah";
        bNum = 73;
    }
    else if("PrAza" == bName)
    {
        bName = "Prayer of Azariah";
        bNum = 74;
    }
    else if("Sus" == bName)
    {
        bName = "Susanna";
        bNum = 75;
    }
    else if("BelDr" == bName)
    {
        bName = "Bel and the Dragon";
        bNum = 76;
    }
    else if("1Macc" == bName)
    {
        bName = "1 Maccabees";
        bNum = 77;
    }
    else if("2Macc" == bName)
    {
        bName = "2 Maccabees";
        bNum = 78;
    }
    else if("3Macc" == bName)
    {
        bName = "3 Maccabees";
        bNum = 79;
    }
    else if("na" == bName)
    {
        bName = "na";
        bNum = 80;
    }
    else if("1Esd" == bName)
    {
        bName = "1 Esdras";
        bNum = 81;
    }
    else if("2Esd" == bName)
    {
        bName = "2 Esdras";
        bNum = 82;
    }
    else if("PrMan" == bName)
    {
        bName = "Prayer of Manasses";
        bNum = 83;
    }
}

QString BibConv::printBible(Bible &bible)
{
    bibleTitle = bible.name;
    QString bText = "";
    QString books = "";
    QString verses = "";
    QString info = "##spDataVersion:\t1";
    info += "\n##Title:\t" + bible.name;
    info += "\n##Abbreviation:\t" + bible.abbr;
    info += "\n##Information:\t" + bible.copyright;
    info += "\n##RightToLeft:\t";
    for(int ib(0);ib<bible.books.count();++ib)
    {
        Book b = bible.books.at(ib);
        bool hasVerses = false;
        for(int ic(0);ic<b.chapters.count();++ic)
        {
            Chapter c = b.chapters.at(ic);
            for(int iv(0);iv<c.verses.count();++iv)
            {
                Verse v = c.verses.at(iv);
                if(!v.text.isEmpty())
                {
                    hasVerses = true;
                    verses += "\nB" + get3(b.bookId) + "C" + get3(c.num) + "V" + get3(v.num) +
                            "\t" + QString::number(b.bookId) +
                            "\t" + QString::number(c.num) +
                            "\t" + QString::number(v.num) +
                            "\t" + v.text;
                }
                else
                {
//                    qDebug()<<"Empty Verse: B" << get3(b.bookId) << "C" << get3(c.num) << "V" << get3(v.num);
                }

            }
        }
        if(hasVerses)
        {
            books += "\n" + QString::number(b.bookId) + "\t" + b.name + "\t" + QString::number(b.chapterCount);
        }
        else
        {
            qDebug()<<b.name<<" - Does not have any text";
        }
    }
    bText = info + books + "\n-----" + verses;
    return bText;
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

void BibConv::toSingleLine(QString &sline)
{
    QStringList line_list = sline.split("\n");
    sline = line_list[0];
    for(int i(1); i<line_list.size();++i)
        sline += "@%" + line_list[i];

    sline = sline.trimmed();
}

void BibConv::on_pushButtonSave_clicked()
{
    QString file_path = QFileDialog::getSaveFileName(this,tr("Save exported Bible as:"),
                                                     bibleTitle,
                                                     tr("SoftProjector Bible file ") + "(*.spb)");
    if(!file_path.isEmpty())
    {
        if(!file_path.endsWith(".spb"))
        {
            file_path = file_path + ".spb";
        }
        QFile ofile;
        ofile.setFileName(file_path);
        if (ofile.open(QIODevice::WriteOnly))
        {
            QTextStream out(&ofile);
            out.setCodec("UTF8");
            out << ui->plainTextEdit->toPlainText();
        }
        ofile.close();
    }
}
