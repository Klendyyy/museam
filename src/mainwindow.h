#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

// Forward declarations
class DashboardWidget;
class ExhibitWidget;
class CollectionWidget;
class ExhibitionWidget;
class EmployeeWidget;
class StorageWidget;
class ReportsWidget;

/**
 * @brief Главное окно приложения "Система управления музеем"
 *
 * Реализует навигацию между разделами:
 * - Панель управления (Dashboard)
 * - Экспонаты
 * - Коллекции
 * - Выставки
 * - Сотрудники
 * - Хранилища
 * - Отчёты
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showDashboard();
    void showExhibits();
    void showCollections();
    void showExhibitions();
    void showEmployees();
    void showStorages();
    void showReports();

    void about();
    void aboutQt();

    void updateStatusBar();

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createWidgets();
    void applyStyles();

    // Центральный виджет со стеком страниц
    QStackedWidget *m_stackedWidget;

    // Виджеты разделов
    DashboardWidget *m_dashboardWidget;
    ExhibitWidget *m_exhibitWidget;
    CollectionWidget *m_collectionWidget;
    ExhibitionWidget *m_exhibitionWidget;
    EmployeeWidget *m_employeeWidget;
    StorageWidget *m_storageWidget;
    ReportsWidget *m_reportsWidget;

    // Действия для навигации
    QAction *m_dashboardAction;
    QAction *m_exhibitsAction;
    QAction *m_collectionsAction;
    QAction *m_exhibitionsAction;
    QAction *m_employeesAction;
    QAction *m_storagesAction;
    QAction *m_reportsAction;

    // Действия меню
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_aboutQtAction;

    // Панель инструментов
    QToolBar *m_mainToolBar;

    // Статусная строка
    QLabel *m_statusLabel;
    QLabel *m_timeLabel;
    QLabel *m_userLabel;
};

#endif // MAINWINDOW_H
