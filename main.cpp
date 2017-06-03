#include <QApplication>
#include <QMessageBox>
#include <QtSql>
#include "bibconv.h"


bool dbconnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("SloZh.sqlite");
    //bool database_exists = ( QFile::exists("SloZh.sqlite") );
    if (!db.open())
    {
        QMessageBox mb;
        mb.setText("spData Error"
                   "Could not connect to the database spData.sqlite!\n\n"
                   "Following Errors:\n"
                   + db.lastError().databaseText() + "\n"
                   + db.lastError().driverText() +"\n"
                   + db.databaseName()
                   + "\n\nThis is a Fatal Error. Please make sure that all QtSql libraries are inlcuded."
                   "\nThe program will terminate");
        mb.setWindowTitle("Database Connection Error");
        mb.setIcon(QMessageBox::Critical);
        mb.exec();
        return false;
    }
    else
        return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BibConv w;
    w.show();
    // Try to connect to database
//    if( !dbconnect() )
//    {
//        QMessageBox mb;
//        mb.setText("Failed to connect to database 'spData.sqlite'");
//        mb.setWindowTitle("Database File Error");
//        mb.setIcon(QMessageBox::Critical);
//        mb.exec();
//        return 1;
//    }
    return a.exec();
}
