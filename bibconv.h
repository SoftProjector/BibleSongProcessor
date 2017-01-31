#ifndef BIBCONV_H
#define BIBCONV_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QtSql>
#include "bible.h"

namespace Ui {
class BibConv;
}

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


private:
    Ui::BibConv *ui;
};

#endif // BIBCONV_H
