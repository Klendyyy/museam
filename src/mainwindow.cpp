// Файл: src/mainwindow.cpp
// Действие: ПОЛНОСТЬЮ ЗАМЕНИТЬ

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
    m_dashboardAction = new QAction(tr("📊 Главная"), this);
    m_dashboardAction->setShortcut(QKeySequence("Ctrl+1"));
    m_dashboardAction->setStatusTip(tr("Панель управления и статистика"));
    m_dashboardAction->setCheckable(true);
    connect(m_dashboardAction, &QAction::triggered, this, &MainWindow::showDashboard);

    m_exhibitsAction = new QAction(tr("🖼️ Экспонаты"), this);
    m_exhibitsAction->setShortcut(QKeySequence("Ctrl+2"));
    m_exhibitsAction->setStatusTip(tr("Каталог экспонатов музея"));
    m_exhibitsAction->setCheckable(true);
    connect(m_exhibitsAction, &QAction::triggered, this, &MainWindow::showExhibits);

    m_collectionsAction = new QAction(tr("📁 Коллекции"), this);
    m_collectionsAction->setShortcut(QKeySequence("Ctrl+3"));
    m_collectionsAction->setStatusTip(tr("Управление коллекциями"));
    m_collectionsAction->setCheckable(true);
    connect(m_collectionsAction, &QAction::triggered, this, &MainWindow::showCollections);

    m_exhibitionsAction = new QAction(tr("🎭 Выставки"), this);
    m_exhibitionsAction->setShortcut(QKeySequence("Ctrl+4"));
    m_exhibitionsAction->setStatusTip(tr("Планирование выставок"));
    m_exhibitionsAction->setCheckable(true);
    connect(m_exhibitionsAction, &QAction::triggered, this, &MainWindow::showExhibitions);

    m_employeesAction = new QAction(tr("👥 Сотрудники"), this);
    m_employeesAction->setShortcut(QKeySequence("Ctrl+5"));
    m_employeesAction->setStatusTip(tr("Управление персоналом"));
    m_employeesAction->setCheckable(true);
    connect(m_employeesAction, &QAction::triggered, this, &MainWindow::showEmployees);

    m_storagesAction = new QAction(tr("📦 Хранилища"), this);
    m_storagesAction->setShortcut(QKeySequence("Ctrl+6"));
    m_storagesAction->setStatusTip(tr("Места хранения экспонатов"));
    m_storagesAction->setCheckable(true);
    connect(m_storagesAction, &QAction::triggered, this, &MainWindow::showStorages);

    m_reportsAction = new QAction(tr("📈 Отчёты"), this);
    m_reportsAction->setShortcut(QKeySequence("Ctrl+7"));
    m_reportsAction->setStatusTip(tr("Формирование отчётов"));
    m_reportsAction->setCheckable(true);
    connect(m_reportsAction, &QAction::triggered, this, &MainWindow::showReports);

    QActionGroup *navGroup = new QActionGroup(this);
    navGroup->addAction(m_dashboardAction);
    navGroup->addAction(m_exhibitsAction);
    navGroup->addAction(m_collectionsAction);
    navGroup->addAction(m_exhibitionsAction);
    navGroup->addAction(m_employeesAction);
    navGroup->addAction(m_storagesAction);
    navGroup->addAction(m_reportsAction);

    m_exitAction = new QAction(tr("Выход"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Выйти из приложения"));
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);

    m_aboutAction = new QAction(tr("О программе"), this);
    m_aboutAction->setStatusTip(tr("Информация о программе"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);

    m_aboutQtAction = new QAction(tr("О Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Информация о Qt"));
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
    m_userLabel = new QLabel(tr("👤 Администратор"));

    QLabel *timeLabel = new QLabel();
    timeLabel->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(timeLabel);
    statusBar()->addPermanentWidget(m_userLabel);
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
    m_statusLabel->setText(tr("📊 Панель управления"));
}

void MainWindow::showExhibits()
{
    m_stackedWidget->setCurrentWidget(m_exhibitWidget);
    m_exhibitsAction->setChecked(true);
    m_exhibitWidget->refresh();
    m_statusLabel->setText(tr("🖼️ Каталог экспонатов"));
}

void MainWindow::showCollections()
{
    m_stackedWidget->setCurrentWidget(m_collectionWidget);
    m_collectionsAction->setChecked(true);
    m_collectionWidget->refresh();
    m_statusLabel->setText(tr("📁 Управление коллекциями"));
}

void MainWindow::showExhibitions()
{
    m_stackedWidget->setCurrentWidget(m_exhibitionWidget);
    m_exhibitionsAction->setChecked(true);
    m_exhibitionWidget->refresh();
    m_statusLabel->setText(tr("🎭 Управление выставками"));
}

void MainWindow::showEmployees()
{
    m_stackedWidget->setCurrentWidget(m_employeeWidget);
    m_employeesAction->setChecked(true);
    m_employeeWidget->refresh();
    m_statusLabel->setText(tr("👥 Управление сотрудниками"));
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
    m_statusLabel->setText(tr("📈 Формирование отчётов"));
}

void MainWindow::about()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("О программе"));
    aboutBox.setIconPixmap(QPixmap());
    aboutBox.setText(
        tr("<div style='text-align: center;'>"
           "<h2 style='color: #1a202c; margin-bottom: 8px;'>🏛️ Система управления музеем</h2>"
           "<p style='color: #718096; font-size: 14px;'>Версия 1.0.0</p>"
           "</div>"
           "<hr style='border: 1px solid #e2e8f0; margin: 16px 0;'>"
           "<p style='color: #4a5568;'>Комплексное решение для автоматизации учёта и управления музейными фондами.</p>"
           "<h3 style='color: #2d3748; margin-top: 16px;'>Возможности:</h3>"
           "<ul style='color: #4a5568;'>"
           "<li>📦 Учёт и каталогизация экспонатов</li>"
           "<li>📁 Формирование и управление коллекциями</li>"
           "<li>🎭 Планирование и проведение выставок</li>"
           "<li>🔧 Контроль реставрационных работ</li>"
           "<li>📍 Управление местами хранения</li>"
           "<li>👥 Учёт персонала</li>"
           "<li>📊 Аналитика и отчётность</li>"
           "</ul>"
           "<hr style='border: 1px solid #e2e8f0; margin: 16px 0;'>"
           "<p style='color: #718096; font-size: 12px; text-align: center;'>© 2024 Курсовой проект</p>"));
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("О Qt"));
}

void MainWindow::updateStatusBar()
{
    DatabaseManager& db = DatabaseManager::instance();
    QString info = QString(tr("📦 Экспонатов: %1  |  📁 Коллекций: %2  |  🎭 Выставок: %3  |  👥 Сотрудников: %4"))
                       .arg(db.getExhibitCount())
                       .arg(db.getCollectionCount())
                       .arg(db.getExhibitionCount())
                       .arg(db.getEmployeeCount());
    m_statusLabel->setText(info);
}
