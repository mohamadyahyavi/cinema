#include "booking.h"
#include "ui_booking.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QRadioButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>

booking::booking(QSqlDatabase db, int userId, int showId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::booking),
    m_db(db),
    m_userId(userId),
    m_showId(showId)
{
    ui->setupUi(this);

    seatGroup = new QButtonGroup(this);
    seatGroup->setExclusive(true);  // Only one seat can be selected

    // Load the seats dynamically from database
    loadSeats();

    // Connect submit button
    connect(ui->pushButtonSubmit, &QPushButton::clicked, this, &booking::submitBooking);

    // Load the show info into the label
    QSqlQuery query(m_db);
    query.prepare("SELECT m.title, s.start_time, s.end_time "
                  "FROM shows s "
                  "JOIN movies m ON s.movie_id = m.id "
                  "WHERE s.id = :showId");
    query.bindValue(":showId", m_showId);

    if(query.exec() && query.next()){
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
    // Access the grid layout directly
    QGridLayout *grid = ui->gridLayoutSeats;
    if (!grid) return; // safety check

    // Clear any existing widgets in the grid
    QLayoutItem *item;
    while ((item = grid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    // Query the seats for this show
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
    const int seatsPerRow = 10; // Adjust as needed

    bool hasSeats = false;

    while (query.next()) {
        hasSeats = true;
        int seatId = query.value("id").toInt();
        QString seatNumber = query.value("seat_number").toString();
        bool isBooked = query.value("is_booked").toBool();

        // Create the seat button
        QRadioButton *seatBtn = new QRadioButton(seatNumber, ui->gridLayoutWidget);
        seatBtn->setEnabled(!isBooked);

        // Color coding: green = available, red = booked
        seatBtn->setStyleSheet(isBooked ? "color: red;" : "color: green;");

        // Add to button group
        seatGroup->addButton(seatBtn, seatId);

        // Add to the grid layout
        grid->addWidget(seatBtn, row, col);

        col++;
        if(col >= seatsPerRow){
            col = 0;
            row++;
        }
    }

    // Show message if no seats exist for this show
    if(!hasSeats){
        QLabel *noSeats = new QLabel("No seats available for this show.", ui->gridLayoutWidget);
        grid->addWidget(noSeats, 0, 0);
    }
}

void booking::submitBooking()
{
    QAbstractButton *selectedBtn = seatGroup->checkedButton();
    if(!selectedBtn){
        QMessageBox::warning(this, "No Seat Selected", "Please select a seat.");
        return;
    }

    int seatId = seatGroup->id(selectedBtn);

    // Update the seat as booked
    QSqlQuery query(m_db);
    query.prepare("UPDATE seats SET is_booked = TRUE WHERE id = :seatId");
    query.bindValue(":seatId", seatId);

    if(!query.exec()){
        QMessageBox::critical(this, "Error", "Booking failed: " + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Booked!", "Your seat has been successfully booked.");

    // Optional: reload seats so the booked seat shows as unavailable
    loadSeats();
}
