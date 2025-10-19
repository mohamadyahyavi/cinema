#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include "login.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- Connect to PostgreSQL ---
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("cinema");  // your database name
    db.setUserName("postgres");    // your DB username
    db.setPassword("muhammad99"); // your DB password

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return -1;  // exit if DB connection fails
    }

    // --- Show Login Widget first ---
    Login loginWindow(db);
    loginWindow.show();

    return a.exec();
}
