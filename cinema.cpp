#include "cinema.h"
#include "ui_cinema.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

CinemaWidget::CinemaWidget(QSqlDatabase db, int userId, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::cinema),
    m_db(db),
    m_userId(userId)
{
    ui->setupUi(this);

    // Start from current week's Monday
    m_currentWeekStart = QDate::currentDate().addDays(-(QDate::currentDate().dayOfWeek() - 1));

    // Load current week shows by default
    showWeek(m_currentWeekStart);

    // Connect navigation buttons
    connect(ui->pushButtonNextWeek, &QPushButton::clicked, this, &CinemaWidget::loadNextWeek);
    connect(ui->pushButtonCurrentWeek, &QPushButton::clicked, this, &CinemaWidget::loadCurrentWeek);
}

CinemaWidget::~CinemaWidget()
{
    delete ui;
}

// Load current week schedule
void CinemaWidget::loadCurrentWeek()
{
    m_currentWeekStart = QDate::currentDate().addDays(-(QDate::currentDate().dayOfWeek() - 1));
    showWeek(m_currentWeekStart);
}

// Load next week schedule
void CinemaWidget::loadNextWeek()
{
    m_currentWeekStart = m_currentWeekStart.addDays(7);
    showWeek(m_currentWeekStart);
}

// Display all shows in the selected week
void CinemaWidget::showWeek(const QDate &startOfWeek)
{
    // Clear old content
    QLayout *layout = ui->layoutScheduleContainer->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        layout = new QVBoxLayout(ui->layoutScheduleContainer);
        ui->layoutScheduleContainer->setLayout(layout);
    }

    QDate endOfWeek = startOfWeek.addDays(6);

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT s.id, m.title, s.start_time, s.end_time
        FROM shows s
        JOIN movies m ON s.movie_id = m.id
        WHERE DATE(s.start_time) BETWEEN :start AND :end
        ORDER BY s.start_time;
    )");
    query.bindValue(":start", startOfWeek);
    query.bindValue(":end", endOfWeek);

    if (!query.exec()) {
        QLabel *errorLabel = new QLabel("Database error: " + query.lastError().text());
        layout->addWidget(errorLabel);
        return;
    }

    bool hasResults = false;
    while (query.next()) {
        hasResults = true;
        int showId = query.value("id").toInt();
        QString title = query.value("title").toString();
        QDateTime start = query.value("start_time").toDateTime();
        QDateTime end = query.value("end_time").toDateTime();

        QWidget *showWidget = new QWidget;
        QHBoxLayout *showLayout = new QHBoxLayout(showWidget);

        QLabel *label = new QLabel(
            QString("%1 — %2\n%3 - %4")
                .arg(title)
                .arg(start.date().toString("ddd, MMM d"))
                .arg(start.time().toString("hh:mm"))
                .arg(end.time().toString("hh:mm"))
            );

        QPushButton *bookBtn = new QPushButton("View Seats");
        connect(bookBtn, &QPushButton::clicked, [this, showId]() { bookSeat(showId); });

        showLayout->addWidget(label);
        showLayout->addStretch();
        showLayout->addWidget(bookBtn);

        layout->addWidget(showWidget);
    }

    if (!hasResults) {
        QLabel *noShows = new QLabel("No shows scheduled for this week.");
        layout->addWidget(noShows);
    }
}

// Placeholder — this will open the seat selection screen later
void CinemaWidget::bookSeat(int showId)
{
    QMessageBox::information(this, "Show Selected",
                             QString("You selected show ID: %1").arg(showId));
}
