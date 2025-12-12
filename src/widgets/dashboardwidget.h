// Файл: src/widgets/dashboardwidget.h
// Действие: ПОЛНОСТЬЮ ЗАМЕНИТЬ

#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QFrame>

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);

    void refresh();

signals:
    void statusMessage(const QString& message);

private:
    void setupUi();
    void setupTableStyle(QTableView *table);
    void updateStatistics();
    void loadRecentExhibits();
    void loadActiveExhibitions();
    void loadRestorationInProgress();

    QFrame* createStatCard(const QString& title, const QString& value,
                           const QString& color, const QString& icon = "");
    QWidget* createTableCard(const QString& title, const QString& icon);

    QLabel *m_exhibitCountLabel;
    QLabel *m_collectionCountLabel;
    QLabel *m_exhibitionCountLabel;
    QLabel *m_employeeCountLabel;
    QLabel *m_onDisplayLabel;
    QLabel *m_inRestorationLabel;

    QTableView *m_recentExhibitsTable;
    QTableView *m_activeExhibitionsTable;
    QTableView *m_restorationTable;
};

#endif // DASHBOARDWIDGET_H
