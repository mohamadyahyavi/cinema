#include "signup.h"
#include "ui_signup.h"
#include "login.h"              // Include login widget for navigation
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTimer>

SignUp::SignUp(QSqlDatabase db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signUp),
    m_db(db)
{
    ui->setupUi(this);

    // Connect buttons to slots
    connect(ui->pushButtonRegister, &QPushButton::clicked, this, &SignUp::handleRegister);
    connect(ui->pushButtonGoLogin, &QPushButton::clicked, this, &SignUp::goToLogin);
}

SignUp::~SignUp() {
    delete ui;
}

// Check if the email is already in the database
bool SignUp::emailExists(const QString &email) {
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM users WHERE email = :email");
    query.bindValue(":email", email);
    if(!query.exec()) return false;

    return query.next(); // Returns true if email exists
}

// Handle user registration
void SignUp::handleRegister() {
    QString fullName = ui->lineEditFullName->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();

    if(fullName.isEmpty() || email.isEmpty() || password.isEmpty()) {
        ui->labelMessage->setText("All fields are required.");
        return;
    }

    if(emailExists(email)) {
        ui->labelMessage->setText("An account with this email already exists.");

        // Optional: automatically redirect to login after 1.5s
        QTimer::singleShot(1500, this, &SignUp::goToLogin);
        return;
    }

    // Insert new user into database
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users(full_name, email, password_hash, role) "
                  "VALUES(:name, :email, :password, 'user')");
    query.bindValue(":name", fullName);
    query.bindValue(":email", email);
    query.bindValue(":password", password); // TODO: hash password for security

    if(!query.exec()) {
        ui->labelMessage->setText("Error: " + query.lastError().text());
        return;
    }

    ui->labelMessage->setText("Registration successful! You can now log in.");

    // Clear input fields
    ui->lineEditFullName->clear();
    ui->lineEditEmail->clear();
    ui->lineEditPassword->clear();
}

// Navigate to login window
void SignUp::goToLogin() {
    Login *loginWin = new Login(m_db);
    loginWin->show();
    this->close(); // Close the SignUp window
}
