#ifndef CINEMAWIDGET_H
#define CINEMAWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDate>
#include "booking.h"   // Include the Booking widget header

namespace Ui {
class cinema;  // Matches your cinema.ui file
}

class CinemaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CinemaWidget(QSqlDatabase db, int userId, QWidget *parent = nullptr);
    ~CinemaWidget();

private slots:
    void loadCurrentWeek();
    void loadNextWeek();
    void showWeek(const QDate &startOfWeek);

    // Opens BookingWidget for the selected show
    void openBookingWidget(int showId);

private:
    Ui::cinema *ui;        // Auto-generated UI class
    QSqlDatabase m_db;     // Database connection
    int m_userId;          // Current logged-in user ID
    QDate m_currentWeekStart; // Monday of the currently displayed week
};

#endif // CINEMAWIDGET_H
