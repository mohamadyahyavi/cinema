#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include "login.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- Set up PostgreSQL connection ---
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);               // Optional: explicitly set port
    db.setDatabaseName("cinema");   // your database name
    db.setUserName("postgres");     // your DB username
    db.setPassword("muhammad99");   // your DB password

    // --- Try connecting to the database ---
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return -1;
    }

    // --- Launch the Login window ---
    Login loginWindow(db);
    loginWindow.show();

    return a.exec();
}
