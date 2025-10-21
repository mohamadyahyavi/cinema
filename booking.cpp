#include "booking.h"
#include "ui_booking.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QRadioButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>

booking::booking(QSqlDatabase db, int userId, int showId, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::booking),
    m_db(db),
    m_userId(userId),
    m_showId(showId)
{
    ui->setupUi(this);

    seatGroup = new QButtonGroup(this);
    seatGroup->setExclusive(true);  // Only one seat selectable at a time

    // Dynamically create a grid layout inside scrollAreaWidgetContents
    QGridLayout *grid = new QGridLayout(ui->scrollAreaWidgetContents);
    grid->setContentsMargins(5,5,5,5);
    grid->setSpacing(10);
    ui->scrollAreaWidgetContents->setLayout(grid);

    // Load seats from database
    loadSeats();

    // Connect the submit button
    connect(ui->pushButtonSubmit, &QPushButton::clicked, this, &booking::submitBooking);

    // Load show info into label
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT m.title, s.start_time, s.end_time
        FROM shows s
        JOIN movies m ON s.movie_id = m.id
        WHERE s.id = :showId
    )");
    query.bindValue(":showId", m_showId);

    if (query.exec() && query.next()) {
        QString title = query.value("title").toString();
        QDateTime start = query.value("start_time").toDateTime();
        QDateTime end = query.value("end_time").toDateTime();
        ui->labelShowInfo->setText(
            QString("%1\n%2 - %3")
                .arg(title)
                .arg(start.toString("ddd, MMM d hh:mm"))
                .arg(end.toString("hh:mm"))
            );
    }
}

booking::~booking()
{
    delete ui;
}

void booking::loadSeats()
{
    QGridLayout *grid = qobject_cast<QGridLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!grid) return;

    // Clear existing widgets
    QLayoutItem *item;
    while ((item = grid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT id, seat_number, is_booked FROM seats "
                  "WHERE show_id = :showId ORDER BY seat_number");
    query.bindValue(":showId", m_showId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Cannot load seats: " + query.lastError().text());
        return;
    }

    int row = 0;
    int col = 0;
    const int seatsPerRow = 10;
    bool hasSeats = false;

    while (query.next()) {
        hasSeats = true;

        int seatId = query.value("id").toInt();
        QString seatNumber = query.value("seat_number").toString();
        bool isBooked = query.value("is_booked").toBool();

        QRadioButton *seatBtn = new QRadioButton(seatNumber);
        seatBtn->setEnabled(!isBooked);
        seatBtn->setStyleSheet(isBooked ? "color:red;" : "color:green;");

        seatGroup->addButton(seatBtn, seatId);
        grid->addWidget(seatBtn, row, col);

        col++;
        if (col >= seatsPerRow) {
            col = 0;
            row++;
        }
    }

    // If no seats found, show a label
    if (!hasSeats) {
        QLabel *noSeats = new QLabel("No seats available for this show.");
        grid->addWidget(noSeats, 0, 0);
    }
}

void booking::submitBooking()
{
    QAbstractButton *selectedBtn = seatGroup->checkedButton();
    if (!selectedBtn) {
        QMessageBox::warning(this, "No Seat Selected", "Please select a seat.");
        return;
    }

    int seatId = seatGroup->id(selectedBtn);

    QSqlQuery query(m_db);
    query.prepare("UPDATE seats SET is_booked = TRUE WHERE id = :seatId");
    query.bindValue(":seatId", seatId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Booking failed: " + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Booked!", "Your seat has been successfully booked.");

    // Reload seats to update availability immediately
    loadSeats();
}
