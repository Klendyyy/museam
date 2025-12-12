#include "mainwindow.h"
#include "dashboardwidget.h"
#include "exhibitwidget.h"
#include "collectionwidget.h"
#include "exhibitionwidget.h"
#include "employeewidget.h"
#include "storagewidget.h"
#include "reportswidget.h"
#include "databasemanager.h"

#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <QActionGroup>
#include <QDateTime>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    createWidgets();

    showDashboard();

    // Таймер для обновления времени в статусбаре
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    timer->start(60000); // Обновление каждую минуту

    updateStatusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("🏛️ Система управления музеем"));
    setMinimumSize(1280, 800);
    resize(1440, 900);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
}

void MainWindow::createActions()
{
    // Группа действий для навигации (взаимоисключающие)
    QActionGroup *navGroup = new QActionGroup(this);

    m_dashboardAction = new QAction(tr("📊 Главная"), this);
    m_dashboardAction->setShortcut(QKeySequence("Ctrl+1"));
    m_dashboardAction->setStatusTip(tr("Панель управления и статистика"));
    m_dashboardAction->setCheckable(true);
    m_dashboardAction->setActionGroup(navGroup);
    connect(m_dashboardAction, &QAction::triggered, this, &MainWindow::showDashboard);

    m_exhibitsAction = new QAction(tr("🖼️ Экспонаты"), this);
    m_exhibitsAction->setShortcut(QKeySequence("Ctrl+2"));
    m_exhibitsAction->setStatusTip(tr("Каталог экспонатов музея"));
    m_exhibitsAction->setCheckable(true);
    m_exhibitsAction->setActionGroup(navGroup);
    connect(m_exhibitsAction, &QAction::triggered, this, &MainWindow::showExhibits);

    m_collectionsAction = new QAction(tr("📁 Коллекции"), this);
    m_collectionsAction->setShortcut(QKeySequence("Ctrl+3"));
    m_collectionsAction->setStatusTip(tr("Управление коллекциями"));
    m_collectionsAction->setCheckable(true);
    m_collectionsAction->setActionGroup(navGroup);
    connect(m_collectionsAction, &QAction::triggered, this, &MainWindow::showCollections);

    m_exhibitionsAction = new QAction(tr("🎭 Выставки"), this);
    m_exhibitionsAction->setShortcut(QKeySequence("Ctrl+4"));
    m_exhibitionsAction->setStatusTip(tr("Планирование выставок"));
    m_exhibitionsAction->setCheckable(true);
    m_exhibitionsAction->setActionGroup(navGroup);
    connect(m_exhibitionsAction, &QAction::triggered, this, &MainWindow::showExhibitions);

    m_employeesAction = new QAction(tr("👥 Сотрудники"), this);
    m_employeesAction->setShortcut(QKeySequence("Ctrl+5"));
    m_employeesAction->setStatusTip(tr("Управление персоналом"));
    m_employeesAction->setCheckable(true);
    m_employeesAction->setActionGroup(navGroup);
    connect(m_employeesAction, &QAction::triggered, this, &MainWindow::showEmployees);

    m_storagesAction = new QAction(tr("📦 Хранилища"), this);
    m_storagesAction->setShortcut(QKeySequence("Ctrl+6"));
    m_storagesAction->setStatusTip(tr("Управление хранилищами"));
    m_storagesAction->setCheckable(true);
    m_storagesAction->setActionGroup(navGroup);
    connect(m_storagesAction, &QAction::triggered, this, &MainWindow::showStorages);

    m_reportsAction = new QAction(tr("📈 Отчёты"), this);
    m_reportsAction->setShortcut(QKeySequence("Ctrl+7"));
    m_reportsAction->setStatusTip(tr("Формирование отчётов"));
    m_reportsAction->setCheckable(true);
    m_reportsAction->setActionGroup(navGroup);
    connect(m_reportsAction, &QAction::triggered, this, &MainWindow::showReports);

    // Действия меню
    m_exitAction = new QAction(tr("🚪 Выход"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Завершение работы программы"));
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    m_aboutAction = new QAction(tr("ℹ️ О программе"), this);
    m_aboutAction->setStatusTip(tr("Информация о программе"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);

    m_aboutQtAction = new QAction(tr("🔧 О Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Информация о Qt Framework"));
    connect(m_aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQt);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(m_exitAction);

    QMenu *navMenu = menuBar()->addMenu(tr("&Навигация"));
    navMenu->addAction(m_dashboardAction);
    navMenu->addSeparator();
    navMenu->addAction(m_exhibitsAction);
    navMenu->addAction(m_collectionsAction);
    navMenu->addAction(m_exhibitionsAction);
    navMenu->addSeparator();
    navMenu->addAction(m_employeesAction);
    navMenu->addAction(m_storagesAction);
    navMenu->addSeparator();
    navMenu->addAction(m_reportsAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Справка"));
    helpMenu->addAction(m_aboutAction);
    helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::createToolBar()
{
    m_mainToolBar = addToolBar(tr("Навигация"));
    m_mainToolBar->setMovable(false);
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_mainToolBar->setIconSize(QSize(20, 20));

    // Стиль для toolbar
    m_mainToolBar->setStyleSheet(R"(
        QToolBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff, stop:1 #f8fafc);
            border-bottom: 2px solid #e2e8f0;
            padding: 8px 16px;
            spacing: 8px;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 8px;
            padding: 10px 16px;
            font-size: 10pt;
            font-weight: 500;
            color: #4a5568;
            margin: 0 2px;
        }
        QToolButton:hover {
            background-color: #edf2f7;
            color: #2d3748;
        }
        QToolButton:pressed {
            background-color: #e2e8f0;
        }
        QToolButton:checked {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #667eea, stop:1 #5a67d8);
            color: #ffffff;
            font-weight: 600;
        }
        QToolBar::separator {
            background-color: #e2e8f0;
            width: 1px;
            margin: 8px 12px;
        }
    )");

    m_mainToolBar->addAction(m_dashboardAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_exhibitsAction);
    m_mainToolBar->addAction(m_collectionsAction);
    m_mainToolBar->addAction(m_exhibitionsAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_employeesAction);
    m_mainToolBar->addAction(m_storagesAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_reportsAction);
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel();
    m_statusLabel->setStyleSheet("color: white; font-weight: 500;");

    m_timeLabel = new QLabel();
    m_timeLabel->setStyleSheet("color: rgba(255,255,255,0.8);");

    m_userLabel = new QLabel(tr("👤 Администратор"));
    m_userLabel->setStyleSheet("color: white; font-weight: 500;");

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_timeLabel);
    statusBar()->addPermanentWidget(m_userLabel);

    // Стиль для статусбара
    statusBar()->setStyleSheet(R"(
        QStatusBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1e3a5f, stop:1 #0f2744);
            color: #ffffff;
            padding: 8px 16px;
            font-size: 9pt;
        }
        QStatusBar::item {
            border: none;
        }
    )");
}

void MainWindow::createWidgets()
{
    m_dashboardWidget = new DashboardWidget(this);
    m_exhibitWidget = new ExhibitWidget(this);
    m_collectionWidget = new CollectionWidget(this);
    m_exhibitionWidget = new ExhibitionWidget(this);
    m_employeeWidget = new EmployeeWidget(this);
    m_storageWidget = new StorageWidget(this);
    m_reportsWidget = new ReportsWidget(this);

    m_stackedWidget->addWidget(m_dashboardWidget);
    m_stackedWidget->addWidget(m_exhibitWidget);
    m_stackedWidget->addWidget(m_collectionWidget);
    m_stackedWidget->addWidget(m_exhibitionWidget);
    m_stackedWidget->addWidget(m_employeeWidget);
    m_stackedWidget->addWidget(m_storageWidget);
    m_stackedWidget->addWidget(m_reportsWidget);

    connect(m_dashboardWidget, &DashboardWidget::statusMessage,
            m_statusLabel, &QLabel::setText);
}

void MainWindow::applyStyles()
{
    // Стили теперь применяются глобально в main.cpp
}

void MainWindow::showDashboard()
{
    m_stackedWidget->setCurrentWidget(m_dashboardWidget);
    m_dashboardAction->setChecked(true);
    m_dashboardWidget->refresh();
    m_statusLabel->setText(tr("📊 Панель управления — обзор музейных фондов"));
}

void MainWindow::showExhibits()
{
    m_stackedWidget->setCurrentWidget(m_exhibitWidget);
    m_exhibitsAction->setChecked(true);
    m_exhibitWidget->refresh();
    m_statusLabel->setText(tr("🖼️ Каталог экспонатов — %1 записей").arg(DatabaseManager::instance().getExhibitCount()));
}

void MainWindow::showCollections()
{
    m_stackedWidget->setCurrentWidget(m_collectionWidget);
    m_collectionsAction->setChecked(true);
    m_collectionWidget->refresh();
    m_statusLabel->setText(tr("📁 Управление коллекциями — %1 активных").arg(DatabaseManager::instance().getCollectionCount()));
}

void MainWindow::showExhibitions()
{
    m_stackedWidget->setCurrentWidget(m_exhibitionWidget);
    m_exhibitionsAction->setChecked(true);
    m_exhibitionWidget->refresh();
    m_statusLabel->setText(tr("🎭 Управление выставками — %1 активных").arg(DatabaseManager::instance().getExhibitionCount()));
}

void MainWindow::showEmployees()
{
    m_stackedWidget->setCurrentWidget(m_employeeWidget);
    m_employeesAction->setChecked(true);
    m_employeeWidget->refresh();
    m_statusLabel->setText(tr("👥 Управление сотрудниками — %1 в штате").arg(DatabaseManager::instance().getEmployeeCount()));
}

void MainWindow::showStorages()
{
    m_stackedWidget->setCurrentWidget(m_storageWidget);
    m_storagesAction->setChecked(true);
    m_storageWidget->refresh();
    m_statusLabel->setText(tr("📦 Управление хранилищами"));
}

void MainWindow::showReports()
{
    m_stackedWidget->setCurrentWidget(m_reportsWidget);
    m_reportsAction->setChecked(true);
    m_statusLabel->setText(tr("📈 Формирование отчётов и аналитика"));
}

void MainWindow::about()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("О программе"));
    aboutBox.setIconPixmap(QPixmap());
    aboutBox.setText(
        tr("<div style='text-align: center;'>"
           "<h2 style='color: #1a202c; margin-bottom: 8px;'>🏛️ Система управления музеем</h2>"
           "<p style='color: #667eea; font-size: 16px; font-weight: 600;'>Версия 1.0.0</p>"
           "</div>"
           "<hr style='border: 1px solid #e2e8f0; margin: 16px 0;'>"
           "<p style='color: #4a5568; line-height: 1.6;'>"
           "Комплексное решение для автоматизации учёта и управления музейными фондами. "
           "Разработано с использованием современных технологий Qt и MySQL."
           "</p>"
           "<h3 style='color: #2d3748; margin-top: 20px;'>✨ Возможности:</h3>"
           "<ul style='color: #4a5568; line-height: 1.8;'>"
           "<li>📦 Учёт и каталогизация экспонатов</li>"
           "<li>📁 Формирование и управление коллекциями</li>"
           "<li>🎭 Планирование и проведение выставок</li>"
           "<li>🔧 Контроль реставрационных работ</li>"
           "<li>📍 Управление местами хранения</li>"
           "<li>👥 Учёт персонала музея</li>"
           "<li>📊 Аналитика и формирование отчётов</li>"
           "</ul>"
           "<hr style='border: 1px solid #e2e8f0; margin: 16px 0;'>"
           "<p style='color: #718096; font-size: 12px; text-align: center;'>"
           "© 2024 Курсовой проект<br>"
           "Qt %1 • MySQL/MariaDB"
           "</p>").arg(qVersion()));
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("О Qt Framework"));
}

void MainWindow::updateStatusBar()
{
    m_timeLabel->setText(QDateTime::currentDateTime().toString("📅 dd.MM.yyyy  🕐 HH:mm"));
}
