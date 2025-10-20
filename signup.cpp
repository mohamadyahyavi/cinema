#include "signup.h"
#include "ui_signup.h"
#include "login.h"
#include "cinema.h"       // You’ll create this soon
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTimer>
#include <QCryptographicHash>

SignUp::SignUp(QSqlDatabase db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::signUp)
    , m_db(db)
{
    ui->setupUi(this);

    // Connect buttons to slots
    connect(ui->pushButtonRegister, &QPushButton::clicked, this, &SignUp::handleRegister);
    connect(ui->pushButtonGoLogin, &QPushButton::clicked, this, &SignUp::goToLogin);
}

SignUp::~SignUp()
{
    delete ui;
}

// ✅ Check if the email already exists
bool SignUp::emailExists(const QString &email)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM users WHERE email = :email");
    query.bindValue(":email", email);

    if (!query.exec()) {
        qDebug() << "Email check failed:" << query.lastError().text();
        return false;
    }

    return query.next(); // true if an account already exists
}

// ✅ Handle user registration
void SignUp::handleRegister()
{
    QString fullName = ui->lineEditFullName->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();

    if (fullName.isEmpty() || email.isEmpty() || password.isEmpty()) {
        ui->labelMessage->setText("⚠️ All fields are required.");
        return;
    }

    if (emailExists(email)) {
        ui->labelMessage->setText("⚠️ This email is already registered.");
        QTimer::singleShot(1500, this, &SignUp::goToLogin);
        return;
    }

    // ✅ Hash the password (SHA-256)
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Insert new user into database
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users(full_name, email, password_hash) "
                  "VALUES(:name, :email, :password) RETURNING id;");
    query.bindValue(":name", fullName);
    query.bindValue(":email", email);
    query.bindValue(":password", password); // TODO: hash password for security

    if(!query.exec()) {
        ui->labelMessage->setText("Error: " + query.lastError().text());
        return;
    }

    int userId = -1;
    if (query.next()) {
        userId = query.value("id").toInt();
    }

    ui->labelMessage->setText("Registration successful! Welcome to Cinema!");

// ✅ Open CinemaWidget window
#include "cinema.h" // make sure this is at the top of the file
    CinemaWidget *cinemaWin = new CinemaWidget(m_db, userId);
    cinemaWin->show();
    this->close();

}

// ✅ Navigate to login window
void SignUp::goToLogin()
{
    Login *loginWin = new Login(m_db);
    loginWin->show();
    this->close();
}
