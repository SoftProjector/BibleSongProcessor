
#include "bibconv.h"
#include "ui_bibconv.h"

BibConv::BibConv(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BibConv)
{
    ui->setupUi(this);
    bibleTypes << "Select Bible Type" << "My Sword" << "Bible Quote" <<
                  "Bible Database" << "Zafenia XML" << "Corpus XML" <<
                  "OSIS XML" << "CSB SML" << "SQLite" << "Zulu Bible";
    songTypes << "Select Song Type" << "EasiSlides XML" << "EasySlides Files";
    bConType = 0;
    if(0 == ui->comboBoxConvType->currentIndex())
    {
        ui->comboBoxBSConvType->clear();
        ui->comboBoxBSConvType->addItems(bibleTypes);
    }
}

BibConv::~BibConv()
{
    delete ui;
}

void BibConv::on_comboBoxConvType_activated(int index)
{
    convType = index;
    switch (convType) {
    case CT_BIBLE:
        ui->comboBoxBSConvType->clear();
        ui->comboBoxBSConvType->addItems(bibleTypes);
        break;
    case CT_SONG:
        ui->comboBoxBSConvType->clear();
        ui->comboBoxBSConvType->addItems(songTypes);
        break;
    default:
        break;
    }
}

void BibConv::on_comboBoxBSConvType_activated(int index)
{
    bConType = index;

    switch (convType) {
    case CT_BIBLE:
        processBibleConversions();
        break;
    case CT_SONG:
        processSongConversions();
        break;
    default:
        break;
    }
}

void BibConv::processBibleConversions()
{
    qDebug()<<"processBibleConversions"<<bConType;
    QString fn = "";

    switch (bConType) {
    case ZAFENIA_XML:
    case CORPUS_XML:
    case OSIS_XML:
    case ZULU_XML:
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
    case CSB_XML:
        fn = QFileDialog::getExistingDirectory(this,"EPUB XHTML directory");
        if(fn.isNull())
        {
            ui->lineEdit->setText("Error opening directory. Please try again.");
            return;
        }
        else
        {
            ui->lineEdit->setText(fn);
        }
        break;
    case SQLITE:
        fn = QFileDialog::getOpenFileName(this, "Open SQLite database", "", "*");
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
        ui->lineEdit->setText("Please Make A Selection");
        break;
    }
}

void BibConv::processSongConversions()
{
     qDebug()<<"processSongConversions"<<bConType;
    QString fn = "";

    switch (bConType) {
    case EASISLIDES_XML:
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
    case EASISLIDES_FILES:
        fn = QFileDialog::getExistingDirectory(this,"EasiSlides song directory");
        if(fn.isNull())
        {
            ui->lineEdit->setText("Error opening directory. Please try again.");
            return;
        }
        else
        {
            ui->lineEdit->setText(fn);
        }
        break;
    default:
        ui->lineEdit->setText("Please Make A Selection");
        break;
    }
}

void BibConv::on_pushButtonStart_clicked()
{
    switch (convType) {
    case CT_BIBLE:
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
            importCorpusXml(ui->lineEdit->text());
            break;
        case OSIS_XML:
            importOsisXml(ui->lineEdit->text());
            break;
        case CSB_XML:
            importEpubXML(ui->lineEdit->text());
            break;
        case SQLITE:
            importSQlite(ui->lineEdit->text());
            break;
        case ZULU_XML:
            importZuluXml(ui->lineEdit->text());
            break;
        default:
            break;
        }
        break;
    case CT_SONG:
        switch (bConType) {
        case EASISLIDES_XML:
            processEasislidesXml(ui->lineEdit->text());
            break;
        case EASISLIDES_FILES:
            processEasislidesFiles(ui->lineEdit->text());
            break;
        default:
            break;
        }
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
        incrementProgressBar();
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

void BibConv::importSQlite(QString fileName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","tsw");
    db.setDatabaseName(fileName);
    if(!db.open())
    {
        QMessageBox mb(this);
        mb.setText("An error has ocured when database file.\n"
                   "Please try again.");
        mb.setIcon(QMessageBox::Critical);
        mb.setStandardButtons(QMessageBox::Ok);
        mb.exec();
        return;
    }
    QSqlQuery sq(db);

    int i(0);
    ui->progressBar->setMaximum(31102);
    Bible bible;
    bool ok = sq.exec("SELECT number, osis, human FROM books");

    if(!ok)
    {
        qDebug()<<sq.lastQuery()<<sq.lastError();
        return;
    }

//    int book_num(0);

    while (sq.next())
    {
        Book book;
        book.bookId = sq.value(0).toInt();
        book.filePath = sq.value(1).toString();
        book.name = sq.value(2).toString().toLower();
        QStringList words = book.name.split(" ");
        book.name.clear();
        foreach (QString w, words)
        {
            QChar c = w.at(0);
            c = c.toUpper();
            w.replace(0,1,c);
            book.name += " " + w;
        }
        book.name = book.name.trimmed();

//        book.chapterCount = sq.value(2).toInt();
        bible.addBook(book);
        ui->progressBar->setValue(++i);
    }

    sq.clear();

    for(int ib = 0;ib < bible.books.count(); ++ib)
    {
        Book bk = bible.books.at(ib);
//        book_num = bk.chapterCount;
        int chNum = 0, chNumOld(0);
        Chapter ch;
        ch.num = 0;
        ok = sq.exec(QString("SELECT verse, unformatted FROM verses WHERE book = \"%1\"").arg(bk.filePath));
        if(!ok)
        {
            qDebug()<<sq.lastQuery()<<sq.lastError();
        }
        while (sq.next())
        {
            Verse v;
            float cv = sq.value(0).toFloat();
            chNum = cv;
            cv = cv - chNum;
            cv = cv * 1000;
            qDebug()<<cv;
            if(0 != chNumOld && chNum != chNumOld)
            {
                ch.num = chNumOld;
                bk.addChapter(ch);
                ch.clear();
            }
            v.num = qRound(cv);
            v.text = sq.value(1).toString().simplified();
            ch.addVerse(v);
            ui->progressBar->setValue(++i);
            chNumOld = chNum;
        }
        ch.num = chNum;
        bk.addChapter(ch);
        bk.chapterCount = chNum;
        sq.clear();

        bible.books.replace(ib,bk);
    }
    ui->plainTextEdit->setPlainText(printBible(bible));
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

    QString info = "Imported from OSIS XML / The SWORD Poject file from http://www.crosswire.org/@%--------------------------@%";
    Bible bible;
    bible.copyright = info + bible.copyright;


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

    int ct(0);
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

                    while (!nt.isNull())
                    {
                        if("div" == nt.nodeName() &&  "book" == nt.toElement().attribute("type"))
                        {
                            Book book;
                            book.name = nt.toElement().attribute("osisID");
                            updateBookName(book.name,book.bookId);

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
}

void BibConv::importZuluXml(QString fileName)
{
    QDomDocument domDoc;
    QFile file (fileName);

    QString info = "Imported from XML https://github.com/godlytalias/Bible-Database/@%--------------------------@%";
    Bible bible;
    bible.copyright = info + bible.copyright;


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

    int ct(0);
    ui->progressBar->setMaximum(40000);

    QDomElement domElem = domDoc.documentElement();
    bible.name = domElem.attribute("name").simplified();
    QDomNode nb = domElem.firstChild();

    QStringList bookNames;
    bookNames << "Genesise" << "Eksodusi" << "Levitikusi" << "Numeri" << "Duteronomi" <<
                 "Joshuwa" << "AbAhluleli" << "Ruthe" << "1 Samuweli" << "2 Samuweli" <<
                 "1 AmaKhosi" << "2 AmaKhosi" << "1 IziKronike" << "2 IziKronike" << "Ezra" <<
                 "Nehemiya" << "Esteri" << "Jobe" << "AmaHubo" << "IzAga" << "UmShumayeli" <<
                 "IsiHlabelelo SeziHlabelelo" << "Isaya" << "Jeremiya" << "IsiLilo" <<
                 "Hezekeli" << "Daniyeli" << "Hoseya" << "Joweli" << "Amose" << "Obadiya" <<
                 "Jona" << "Mika" << "Nahume" << "Habakuki" << "Zefaniya" << "Hagayi" <<
                 "Zakariya" << "Malaki" << "Mathewu" << "Marku" << "Luka" << "Johane" << "IzEnzo" <<
                 "AmaRoma" << "1 Korinte" << "2 Korinte" << "Galathiya" << "Efesu" << "Filipi" <<
                 "Kolose" << "1 Thesalonika" << "2 Thesalonika" << "1 Thimothewu" << "2 Thimothewu" <<
                 "KuThithu" << "KuFilemoni" << "KumaHeberu" << "EkaJakobe" << "1 Petru" << "2 Petru" <<
                 "1 Johane" << "2 Johane" << "3 Johane" << "EkaJuda" << "IsAmbulo";

    while(!nb.isNull())
    {
        if("Book" == nb.nodeName())
        {
            Book book;
            int id = nb.toElement().attribute("id").toInt();
            book.name = bookNames.at(id);
            book.bookId = id + 1;


            QDomNode nc = nb.firstChild();

            while (!nc.isNull())
            {
                if("Chapter" == nc.nodeName())
                {
                    Chapter chap;
                    chap.num = nc.toElement().attribute("id").toInt();
                    QDomNode nv = nc.firstChild();

                    while (!nv.isNull())
                    {
                        if("Verse" == nv.nodeName())
                        {
                            Verse v;
                            v.num = nv.toElement().attribute("id").toInt();
                            v.text = nv.toElement().text().simplified();
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
        nb = nb.nextSibling();
    }

    ui->plainTextEdit->setPlainText(printBible(bible));
//    ui->progressBar->setValue(ui->progressBar->maximum());
}

void BibConv::importCorpusXml(QString fileName)
{
    QDomDocument domDoc;
    QFile file (fileName);

    QString info = "Imported from Corpus XML https://github.com/christos-c/bible-corpus/@%--------------------------@%";
    Bible bible;
    bible.copyright = info + bible.copyright;


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

    int ct(0);
    ui->progressBar->setMaximum(70000);

    QDomElement domElem = domDoc.documentElement();
    QDomNode n = domElem.firstChild();

    while(!n.isNull())
    {
        if("cesHeader" == n.nodeName())
        {
            QDomNode nh = n.firstChild();
            while(!nh.isNull())
            {
                if("profileDesc" == nh.nodeName())
                {
                    QDomNode np = nh.firstChild();
                    while (!np.isNull())
                    {
                        if("langUsage" == np.nodeName())
                        {
                            QDomNode nl = np.firstChild();
                            while (!nl.isNull())
                            {
                                if("language" == nl.nodeName())
                                {
                                    bible.abbr = nl.toElement().attribute("iso639").simplified();
                                    bible.name = nl.toElement().text().simplified();
                                }
                                ++ct;
                                ui->progressBar->setValue(ct);
                                nl = nl.nextSibling();
                            }

                        }
                        np = np.nextSibling();
                    }
                }
                nh = nh.nextSibling();
            }
        }
        else if("text" == n.nodeName())
        {
            QDomNode nt = n.firstChild();
            while(!nt.isNull())
            {
                if("body" == nt.nodeName())
                {
                    QDomNode nb = nt.firstChild();

                    while (!nb.isNull())
                    {
                        if("div" == nb.nodeName() &&  "book" == nb.toElement().attribute("type"))
                        {
                            Book book;
                            book.name = nb.toElement().attribute("id").split(".").last().simplified();
                            updateBookName(book.name,book.bookId);

                            QDomNode nc = nb.firstChild();

                            while (!nc.isNull())
                            {
                                if("div" == nc.nodeName() &&  "chapter" == nc.toElement().attribute("type"))
                                {
                                    Chapter chap;
                                    chap.num = nc.toElement().attribute("id").split(".").last().toInt();
                                    QDomNode nv = nc.firstChild();

                                    while (!nv.isNull())
                                    {
                                        if("seg" == nv.nodeName() &&  "verse" == nv.toElement().attribute("type"))
                                        {
                                            Verse v;
                                            v.num = nv.toElement().attribute("id").split(".").last().toInt();
                                            v.text = nv.toElement().text().simplified();
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
                        nb = nb.nextSibling();
                    }
                }
                ++ct;
                ui->progressBar->setValue(ct);
                nt = nt.nextSibling();
            }
        }
        ++ct;
        ui->progressBar->setValue(ct);
        n = n.nextSibling();
    }

    ui->plainTextEdit->setPlainText(printBible(bible));
    ui->progressBar->setValue(ui->progressBar->maximum());
}

void BibConv::importEpubXML(QString directory)
{
    QDir dir(directory);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Unsorted );
    QFileInfoList files = dir.entryInfoList();

    Bible bible;
    Book b;

    int bookId = 0;
    ui->progressBar->setMaximum(files.size() * 2);

    int chapNum = 0;
    int bStartIndex = 0;

    for (int i = 0; i < files.size(); ++i)
    {
        QFileInfo f = files.at(i);
        QStringList l = f.fileName().split(".").first().split("-");
        QString bn = l.at(2);
        int bi = l.at(1).toInt();
        int cn = 0;
        if(3 == l.size())
        {
            cn = 1;
        }
        else
        {
            cn = l.at(3).toInt();
        }

        if(bookId != bi)
        {
            if(0 != b.bookId)
            {
                b.chapterCount = chapNum;
                bible.addBook(b);
            }
            // new book
            b.clear();
            b.bookId = bi;
            b.name = bn;
            bookId = bi;
            chapNum = 0;
            bStartIndex = i;
        }

        ++chapNum;
        Chapter c;

        if(cn != chapNum)
        {
            for (int k = bStartIndex;k<files.size();++k)
            {
                QFileInfo fx = files.at(k);
                QStringList lx = fx.fileName().split(".").first().split("-");
                int bix = lx.at(1).toInt();
                int cnx =0;

                if(3 == lx.size())
                {
                    cnx = 1;
                }
                else
                {
                    cnx = lx.at(3).toInt();
                }

                if(chapNum == cnx)
                {
                    c.path = fx.filePath();
                    c.num = cnx;
                    break;
                }
                if(bookId != bix)
                {
                    break;
                }
            }
        }
        else
        {
            c.path = f.filePath();
            c.num = cn;
        }
        //        c.path = f.filePath();
        //        c.num = cn;
        //        qDebug()<<cn<<chapNum<<c.num<<f.fileName()<<c.path<<f.filePath();

        processEbupChapter(c);
        b.addChapter(c);

        // add last book
        if(i == files.size()-1)
        {
            b.chapterCount = chapNum;
            bible.addBook(b);
        }
        incrementProgressBar();
    }

    ui->plainTextEdit->setPlainText(printBible(bible));
}

void BibConv::processEbupChapter(Chapter &c)
{
    Verse v;
    v.num = 0;

    QDomDocument domDoc;
    QFile file (c.path);


    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open file: " << c.path;
        ui->lineEdit->setText("Can't open file: " + c.path);
        return;
    }
    else
    {
        //        qDebug() << "Opening file: " << c.path;
    }

    if(!domDoc.setContent(&file,false))
    {
        ui->lineEdit->setText("Error XML DOM Document open");
        file.close();
        return;
    }
    file.close();

    QDomElement domElem = domDoc.documentElement();
    QDomNode n = domElem.firstChild();

    QString chapterText = "";

    while(!n.isNull())
    {
        if("body" == n.nodeName())
        {
            QDomNode nb = n.firstChild();
            while(!nb.isNull())
            {
                if("section" == nb.nodeName())
                {
                    c.num = nb.toElement().attribute("id").split(".").last().toInt();
                    QDomNode ns = nb.firstChild();
                    while (!ns.isNull())
                    {
                        if("p" == ns.nodeName() || "section" == ns.nodeName()
                                || "table" == ns.nodeName() || "ol" == ns.nodeName()
                                || "blockquote" == ns.nodeName())
                        {
                            //                            qDebug()<<"Child Node Count: "<<ns.childNodes().count()<<ns.nodeName()<<ns.toElement().attribute("class")<<ns.toElement().text();

                            if("speaker" == ns.toElement().attribute("class") || "supertitle" == ns.toElement().attribute("class"))
                            {
                                ns = ns.nextSibling();
                                continue;
                            }

                            if("speaker" != ns.toElement().attribute("class")
                                    && "supertitle" != ns.toElement().attribute("class")
                                    && "chapter-number" != ns.toElement().attribute("class")
                                    && "verse-number" != ns.toElement().attribute("class")
                                    && "speaker" != ns.toElement().attribute("class")
                                    && "dynprose" != ns.toElement().attribute("class")
                                    && "" != ns.toElement().attribute("class")
                                    && "otdynprose" != ns.toElement().attribute("class")
                                    && "poetryblock" != ns.toElement().attribute("class"))
                            {
                                qDebug()<<"Verify:"<<ns.toElement().attribute("class");
                            }

                            processEbupVerse(ns,domDoc);

                            chapterText += " " + ns.toElement().text().simplified().remove("†");
                        }
                        ns = ns.nextSibling();
                    }
                }
                nb = nb.nextSibling();
            }
        }
        n = n.nextSibling();
    }

    QStringList chapterVerses = chapterText.split("@#");
    foreach (QString vt, chapterVerses)
    {
        if("" == vt.trimmed())
        {
            continue;
        }
        QStringList vl = vt.trimmed().split(" ");
        QString vs = vl.first();
        bool ok;
        int vNum = vs.toInt(&ok,10);
        v.num++;
        if(ok)
        {
            v.num = vNum;
            if (vNum != v.num)
            {
                qDebug()<<"numMisMatch"<<c.path<<v.num<<vt;
            }
        }
        vs = "";
        for(int vp = 1;vp<vl.size();++vp)
        {
            vs += "  " + vl.at(vp);
        }
        v.text = vs.simplified();
        c.addVerse(v);
    }
}

void BibConv::processEbupVerse(QDomNode &n, QDomDocument &dd)
{
    for(int icn(0);icn<n.childNodes().count();++icn)
    {
        //        qDebug()<<"Name:"<<n.nodeName()<<"class"<<n.toElement().attribute("class")<<n.toElement().text();
        // Remove unwanted nodes
        QDomNode np = n.childNodes().at(icn);
        processEbupVerse(np,dd);
        n.childNodes().at(icn) = np;

        if("chapter-number" == np.toElement().attribute("class"))
        {
            QDomNode nx = n.childNodes().at(icn);
            n.removeChild(n.childNodes().at(icn));
            QDomText dt = dd.createTextNode("1 " );
            n.insertBefore(dt,n.childNodes().at(icn));
        }
        else if("verse-number" == np.toElement().attribute("class"))
        {
            QString vn = np.toElement().text();

            n.removeChild(n.childNodes().at(icn));
            QDomText dt = dd.createTextNode("@#" + vn + " " );
            n.insertBefore(dt,n.childNodes().at(icn));
        }

        if("h3"==np.nodeName() /*|| "a"==np.nodeName()*/)
        {
            //            qDebug()<<"node Name"<<np.nodeName()<<np.toElement().text()<<n.toElement().text();
            n.removeChild(n.childNodes().at(icn));
            //         qDebug()<<"node Name"<<np.nodeName()<<np.toElement().text()<<n.toElement().text();
        }
    }
}

void BibConv::updateBookName(QString &bName, int &bNum)
{
    if("Gen" == bName || "GEN" == bName)
    {
        bName = "Genesis";
        bNum = 1;
    }
    else if("Exod" == bName || "EXO" == bName)
    {
        bName = "Exodus";
        bNum = 2;
    }
    else if("Lev" == bName || "LEV" == bName)
    {
        bName = "Leviticus";
        bNum = 3;
    }
    else if("Num" == bName || "NUM" == bName)
    {
        bName = "Numbers";
        bNum = 4;
    }
    else if("Deut" == bName || "DEU" == bName)
    {
        bName = "Deuteronomy";
        bNum = 5;
    }
    else if("Josh" == bName || "JOS" == bName)
    {
        bName = "Joshua";
        bNum = 6;
    }
    else if("Judg" == bName || "JDG" == bName)
    {
        bName = "Judges";
        bNum = 7;
    }
    else if("Ruth" == bName || "RUT" == bName)
    {
        bName = "Ruth";
        bNum = 8;
    }
    else if("1Sam" == bName || "1SA" == bName)
    {
        bName = "1 Samuel";
        bNum = 9;
    }
    else if("2Sam" == bName || "2SA" == bName)
    {
        bName = "2 Samuel";
        bNum = 10;
    }
    else if("1Kgs" == bName || "1KI" == bName)
    {
        bName = "1 Kings";
        bNum = 11;
    }
    else if("2Kgs" == bName || "2KI" == bName)
    {
        bName = "2 Kings";
        bNum = 12;
    }
    else if("1Chr" == bName || "1CH" == bName)
    {
        bName = "1 Chronicles";
        bNum = 13;
    }
    else if("2Chr" == bName || "2CH" == bName)
    {
        bName = "2 Chronicles";
        bNum = 14;
    }
    else if("Ezra" == bName || "EZR" == bName)
    {
        bName = "Ezra";
        bNum = 15;
    }
    else if("Neh" == bName || "NEH" == bName)
    {
        bName = "Nehemiah";
        bNum = 16;
    }
    else if("Esth" == bName || "EST" == bName)
    {
        bName = "Esther";
        bNum = 17;
    }
    else if("Job" == bName || "JOB" == bName)
    {
        bName = "Job";
        bNum = 18;
    }
    else if("Ps" == bName || "PSA" == bName)
    {
        bName = "Psalms";
        bNum = 19;
    }
    else if("Prov" == bName || "PRO" == bName)
    {
        bName = "Proverbs";
        bNum = 20;
    }
    else if("Eccl" == bName || "ECC" == bName)
    {
        bName = "Ecclesiastes";
        bNum = 21;
    }
    else if("Song" == bName || "SON" == bName)
    {
        bName = "Song of Solomon";
        bNum = 22;
    }
    else if("Isa" == bName || "ISA" == bName)
    {
        bName = "Isaiah";
        bNum = 23;
    }
    else if("Jer" == bName || "JER" == bName)
    {
        bName = "Jeremiah";
        bNum =24;
    }
    else if("Lam" == bName || "LAM" == bName)
    {
        bName = "Lamentations";
        bNum = 25;
    }
    else if("Ezek" == bName || "EZE" == bName)
    {
        bName = "Ezekiel";
        bNum = 26;
    }
    else if("Dan" == bName || "DAN" == bName)
    {
        bName = "Daniel";
        bNum = 27;
    }
    else if("Hos" == bName || "HOS" == bName)
    {
        bName = "Hosea";
        bNum = 28;
    }
    else if("Joel" == bName || "JOE" == bName)
    {
        bName = "Joel";
        bNum = 29;
    }
    else if("Amos" == bName || "AMO" == bName)
    {
        bName = "Amos";
        bNum = 30;
    }
    else if("Obad" == bName || "OBA" == bName)
    {
        bName = "Obadiah";
        bNum = 31;
    }
    else if("Jonah" == bName || "JON" == bName)
    {
        bName = "Jonah";
        bNum = 32;
    }
    else if("Mic" == bName || "MIC" == bName)
    {
        bName = "Micah";
        bNum = 33;
    }
    else if("Nah" == bName || "NAH" == bName)
    {
        bName = "Nahum";
        bNum = 34;
    }
    else if("Hab" == bName || "HAB" == bName)
    {
        bName = "Habakkuk";
        bNum = 35;
    }
    else if("Zeph" == bName || "ZEP" == bName)
    {
        bName = "Zephaniah";
        bNum = 36;
    }
    else if("Hag" == bName || "HAG" == bName)
    {
        bName = "Haggai";
        bNum = 37;
    }
    else if("Zech" == bName || "ZEC" == bName)
    {
        bName = "Zechariah";
        bNum = 38;
    }
    else if("Mal" == bName || "MAL" == bName)
    {
        bName = "Malachi";
        bNum = 39;
    }
    else if("Matt" == bName || "MAT" == bName)
    {
        bName = "Matthew";
        bNum = 40;
    }
    else if("Mark" == bName || "MAR" == bName)
    {
        bName = "Mark";
        bNum = 41;
    }
    else if("Luke" == bName || "LUK" == bName)
    {
        bName = "Luke";
        bNum = 42;
    }
    else if("John" == bName || "JOH" == bName)
    {
        bName = "John";
        bNum = 43;
    }
    else if("Acts" == bName || "ACT" == bName)
    {
        bName = "Acts";
        bNum = 44;
    }
    else if("Rom" == bName || "ROM" == bName)
    {
        bName = "Romans";
        bNum = 45;
    }
    else if("1Cor" == bName || "1CO" == bName)
    {
        bName = "1 Corinthians";
        bNum = 46;
    }
    else if("2Cor" == bName || "2CO" == bName)
    {
        bName = "2 Corinthians";
        bNum = 47;
    }
    else if("Gal" == bName || "GAL" == bName)
    {
        bName = "Galatians";
        bNum = 48;
    }
    else if("Eph" == bName || "EPH" == bName)
    {
        bName = "Ephesians";
        bNum = 49;
    }
    else if("Phil" == bName || "PHI" == bName)
    {
        bName = "Philippians";
        bNum = 50;
    }
    else if("Col" == bName || "COL" == bName)
    {
        bName = "Colossians";
        bNum = 51;
    }
    else if("1Thess" == bName || "1TH" == bName)
    {
        bName = "1 Thessalonians";
        bNum = 52;
    }
    else if("2Thess" == bName || "2TH" == bName)
    {
        bName = "2 Thessalonians";
        bNum = 53;
    }
    else if("1Tim" == bName || "1TI" == bName)
    {
        bName = "1 Timothy";
        bNum = 54;
    }
    else if("2Tim" == bName || "2TI" == bName)
    {
        bName = "2 Timothy";
        bNum = 55;
    }
    else if("Titus" == bName || "TIT" == bName)
    {
        bName = "Titus";
        bNum = 56;
    }
    else if("Phlm" == bName || "PHM" == bName)
    {
        bName = "Philemon";
        bNum = 57;
    }
    else if("Heb" == bName || "HEB" == bName)
    {
        bName = "Hebrews";
        bNum = 58;
    }
    else if("Jas" == bName || "JAM" == bName)
    {
        bName = "James";
        bNum = 59;
    }
    else if("1Pet" == bName || "1PE" == bName)
    {
        bName = "1 Peter";
        bNum = 60;
    }
    else if("2Pet" == bName || "2PE" == bName)
    {
        bName = "2 Peter";
        bNum = 61;
    }
    else if("1John" == bName || "1JO" == bName)
    {
        bName = "1 John";
        bNum = 62;
    }
    else if("2John" == bName || "2JO" == bName)
    {
        bName = "2 John";
        bNum = 63;
    }
    else if("3John" == bName || "3JO" == bName)
    {
        bName = "3 John";
        bNum = 64;
    }
    else if("Jude" == bName || "JUD" == bName)
    {
        bName = "Jude";
        bNum = 65;
    }
    else if("Rev" == bName || "REV" == bName)
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
                    QString vs = "\nB" + get3(b.bookId) + "C" + get3(c.num) + "V" + get3(v.num) +
                            "\t" + QString::number(b.bookId) +
                            "\t" + QString::number(c.num) +
                            "\t" + QString::number(v.num) +
                            "\t" + v.text;
                    verses += vs;
                    //                    qDebug()<<"Verse: "<<vs;
                }
                else
                {
                    //                    qDebug()<<"Empty Verse: B" << get3(b.bookId) << "C" << get3(c.num) << "V" << get3(v.num);
                }
                ui->progressBar->setValue(ui->progressBar->value()+1);
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

void BibConv::processEasislidesXml(QString fileName)
{
    QDomDocument domDoc;
    QFile file (fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open file: " << fileName;
        ui->lineEdit->setText("Can't open file: " + fileName);
        return;
    }
    QString err = "";
    if(!domDoc.setContent(&file,false,&err))
    {
        ui->lineEdit->setText("Error XML DOM Document open:" + err);
        file.close();
        return;
    }
    file.close();

    int sNum(0);

    songBook.title = "Romanian Songbook";
    songBook.info = "Contains both Romaninan and English songs.\n\n May have duplicate songs.";
    ui->progressBar->setMaximum(9764);

    QDomElement domElem = domDoc.documentElement();
    QDomNode n = domElem.firstChild();

    while (!n.isNull())
    {
        if("Item" == n.nodeName())
        {
            QDomNode ni = n.firstChild();
            Song s;
            ++sNum;
            while(!ni.isNull())
            {
                if("Title1" == ni.nodeName())
                {
                    s.title = ni.toElement().text();
                }
                else if("SongNumber" == ni.nodeName())
                {
                    bool ok = false;
                    int n(-1);
                    n = ni.toElement().text().toInt(&ok);
                    if(ok && n > 0)
                    {
                         s.number = n;
                         qDebug()<<s.title<<s.number;
                    }
                    else
                    {
                         s.number = sNum;
                    }
                }
                else if("Contents" == ni.nodeName())
                {
                    s.text = ni.toElement().text();
                }

                ni = ni.nextSibling();
            }
            processSongText(s);
            songBook.addSong(s);
            incrementProgressBar();
        }
        n = n.nextSibling();
    }


    ui->plainTextEdit->setPlainText(songBook.printSongbook());
//    ui->progressBar->setValue(ui->progressBar->maximum());

}

void BibConv::processSongText(Song &song)
{
    QStringList songLines = song.text.split("\n");
    QString newText = "";
    bool hasStanzaTitle = false;
    foreach (QString l, songLines)
    {
        if(l.startsWith("Verse") || l.startsWith("Chorus"))
        {
            hasStanzaTitle = true;
            QStringList stl = l.split(" ");
            if(stl.size()<2)
            {
                newText += l + "\n";
                continue;
            }
            QStringList numl = stl.at(1).split(".");
            if(numl.size()<2)
            {
                newText += l + "\n";
                continue;
            }
            int i = numl.at(1).toInt();
            if(i>1)
            {
                l = "&" + l;
            }
        }
        else if(l.startsWith("CCLI") || l.startsWith("©"))
        {
            continue;
        }
        else if(l.startsWith("["))
        {
            hasStanzaTitle = true;
            l = l.remove("[");
            l = l.remove("]");
            l = l.trimmed();
            if("chorus"==l)
            {
                l = "Chorus";
            }
            else
            {
                l = "Verse " + l;
            }
        }
        newText += l + "\n";
    }

    static int x = 0;
    if(!hasStanzaTitle)
    {
        ++x;
        qDebug()<<"No Stanza Titles:"<<song.number<<song.title;
    }

    song.text = newText.trimmed();
}

void BibConv::processEasislidesFiles(QString directory)
{

}

void BibConv::on_pushButtonSave_clicked()
{
    QString filePath = "";

    switch (convType) {
    case CT_BIBLE:
        filePath = QFileDialog::getSaveFileName(this,tr("Save exported Bible as:"),
                                                bibleTitle,
                                                tr("SoftProjector Bible file ") + "(*.spb)");
        exportBible(filePath);
        break;
    case CT_SONG:
        filePath = QFileDialog::getSaveFileName(this,tr("Save exported Songbook as:"),
                                                         songBook.title,
                                                         tr("SoftProjector Songbook file ") + "(*.sps)");
        if(!filePath.endsWith(".sps"))
        {
            filePath += ".sps";
        }

        songBook.exportSongbook(filePath);
    default:
        break;
    }


}

void BibConv::exportBible(QString path)
{
    if(!path.isEmpty())
    {
        if(!path.endsWith(".spb"))
        {
            path += ".spb";
        }
        QFile ofile;
        ofile.setFileName(path);
        if (ofile.open(QIODevice::WriteOnly))
        {
            QTextStream out(&ofile);
            out.setCodec("UTF8");
            out << ui->plainTextEdit->toPlainText();
        }
        ofile.close();
    }
}

void BibConv::incrementProgressBar()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
}
