#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include "signup.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL"); // PostgreSQL
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("cinema");
    db.setUserName("postgres");
    db.setPassword("muhammad99");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error",
                              "Failed to connect to the database:\n" + db.lastError().text());
        return;
    }

    // Open Login window
    Login *loginWin = new Login(db);
    loginWin->show();

    this->close(); // Close the main window since Login is now shown
}

MainWindow::~MainWindow()
{
    delete ui;
}
