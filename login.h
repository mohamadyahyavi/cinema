#pragma once
#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
class login;   // lowercase matches .ui file
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QSqlDatabase db, QWidget *parent = nullptr);
    ~Login();

private slots:
    void handleLogin();  // Login button clicked
    void goToSignUp();   // Switch to SignUp window

private:
    Ui::login *ui;       // lowercase l
    QSqlDatabase m_db;
};
