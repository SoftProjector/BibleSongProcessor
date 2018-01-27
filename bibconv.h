#ifndef BIBCONV_H
#define BIBCONV_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QtSql>
#include <QDomDocument>
#include "bible.h"
#include "song.h"

namespace Ui {
class BibConv;
}
enum ConvertionType
{
    CT_BIBLE,
    CT_SONG
};

enum BibleConvType
{
    PLEASE_SELECT,
    MY_SWORD,
    BIBLE_QUOTE,
    BIBLE_DATABASE,
    ZAFENIA_XML,
    CORPUS_XML,
    OSIS_XML,
    CSB_XML,
    SQLITE
};

enum SongConvType
{
    PLEASE_SELECT_S,
    EASISLIDES_XML,
    EASISLIDES_FILES
};

class BibConv : public QMainWindow
{
    Q_OBJECT

public:
    explicit BibConv(QWidget *parent = 0);
    ~BibConv();

private slots:
    void on_pushButtonStart_clicked();
    QString processLineMySword(QString line, QString &book);
    void importMySword();
    void importSQlite(QString fileName);
    void importBibleQuote();
    void importBibleDatabase();
    QString get3(int i);
    QString trimText(QString original,QString trimTxt);
//    QString trimNRTText(QString original,QString trimTxt);
    QString processBookNRT(QString dirPath, Book book, QString cDelim, QString vDelim);
//    QString processBookRST(QString dirPath, Book book, QString cDelim, QString vDelim);
//    QString processBookRST77N(QString dirPath, Book book, QString cDelim, QString vDelim);
//    QString processBookWBTC(QString dirPath, Book book, QString cDelim, QString vDelim);
    QString processBookRBO2011(QString dirPath, Book book, QString cDelim, QString vDelim);
    void toSingleLine(QString &sline);
    void importXml(QString fileName);
    void importOsisXml(QString fileName);
    void importCorpusXml(QString fileName);
    void importEpubXML(QString directory);
    void processEbupChapter(Chapter &c);
    void processEbupVerse(QDomNode &n, QDomDocument &dd);
    void updateBookName(QString &bName, int &bNum);
    void processEasislidesXml(QString fileName);
    void processEasislidesFiles(QString directory);
    QString printBible(Bible &bible);

    void on_pushButtonSave_clicked();
    void exportBible(QString path);
    void incrementProgressBar();

    void on_comboBoxConvType_activated(int index);

    void on_comboBoxBSConvType_activated(int index);
    void processBibleConversions();
    void processSongConversions();
    void processSongText(Song &song);

private:
    Ui::BibConv *ui;
    int bConType;
    int convType;
    QString bibleTitle;
    QStringList bibleTypes;
    QStringList songTypes;

    Songbook songBook;
};

#endif // BIBCONV_H
