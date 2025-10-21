#pragma once
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QButtonGroup>
#include <QDateTime>

namespace Ui {
class booking;
}

class booking : public QWidget
{
    Q_OBJECT

public:
    explicit booking(QSqlDatabase db, int userId, int showId, QWidget *parent = nullptr);
    ~booking();

private slots:
    void submitBooking();

private:
    void loadSeats();

    Ui::booking *ui;
    QSqlDatabase m_db;
    int m_userId;
    int m_showId;
    QButtonGroup *seatGroup;  // ensures only one seat can be selected
};
