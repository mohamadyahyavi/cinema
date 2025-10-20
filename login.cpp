#include "login.h"
#include "ui_login.h"
#include "signup.h"
#include "cinema.h"          // Include Cinema widget
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

Login::Login(QSqlDatabase db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login),  // lowercase 'login' to match your .ui
    m_db(db)
{
    ui->setupUi(this);

    // Connect Login button
    connect(ui->pushButtonLogin, &QPushButton::clicked, this, &Login::handleLogin);

    // Connect "Sign Up" button
    connect(ui->pushButtonGoSignUp, &QPushButton::clicked, this, &Login::goToSignUp);

    // Optional: make labelMessage red for errors
    ui->labelMessage->setStyleSheet("color: red;");
}

Login::~Login() {
    delete ui;
}

// Handle login logic
void Login::handleLogin() {
    QString email = ui->lineEditEmail->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();

    if (email.isEmpty() || password.isEmpty()) {
        ui->labelMessage->setText("Please enter email and password.");
        return;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT id, full_name FROM users WHERE email = :email AND password_hash = :password");
    query.bindValue(":email", email);
    query.bindValue(":password", password); // TODO: hash password securely before comparing

    if (!query.exec()) {
        ui->labelMessage->setText("Database error: " + query.lastError().text());
        return;
    }

    if(query.next()) {
        int userId = query.value("id").toInt();
        QString name = query.value("full_name").toString();
        ui->labelMessage->setStyleSheet("color: green;");
        ui->labelMessage->setText("Login successful! Welcome, " + name);

        // ✅ Open CinemaWidget window
        CinemaWidget *cinemaWin = new CinemaWidget(m_db, userId);
        cinemaWin->show();
        this->close();
    } else {
        ui->labelMessage->setText("Invalid email or password.");
    }
}

// Open SignUp window
void Login::goToSignUp() {
    SignUp *signupWin = new SignUp(m_db);
    signupWin->show();
    this->close(); // Close login window
}
