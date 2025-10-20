#pragma once
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QDate>

namespace Ui {
class cinema;
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
    void bookSeat(int showId);

private:
    Ui::cinema *ui;
    QSqlDatabase m_db;
    int m_userId;
    QDate m_currentWeekStart;
};
