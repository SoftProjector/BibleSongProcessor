#ifndef BIBCONV_H
#define BIBCONV_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QtSql>
#include <QDomDocument>
#include "bible.h"

namespace Ui {
class BibConv;
}

enum BibleConvType
{
    MY_SWORD,
    BIBLE_QUOTE,
    BIBLE_DATABASE,
    ZAFENIA_XML,
    CORPUS_XML,
    OSIS_XML,
    CSB_XML
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
    void updateOsisBibleName(QString &bName, int &bNum);
    QString printBible(Bible &bible);

    void on_comboBox_activated(int index);

    void on_pushButtonSave_clicked();

private:
    Ui::BibConv *ui;
    int bConType;
    QString bibleTitle;
};

#endif // BIBCONV_H
