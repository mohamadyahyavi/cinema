#pragma once

#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
class signUp;   // lowercase s, matches your .ui file
}

class SignUp : public QWidget
{
    Q_OBJECT

public:
    explicit SignUp(QSqlDatabase db, QWidget *parent = nullptr);
    ~SignUp();

private slots:
    void handleRegister();  // Called when Register button is clicked
    void goToLogin();       // Switch to login window

private:
    Ui::signUp *ui;         // lowercase s, matches the Ui class generated from .ui
    QSqlDatabase m_db;

    bool emailExists(const QString &email); // Check if email is already registered
};
