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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    createWidgets();
    applyStyles();
    
    showDashboard();
    updateStatusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Система управления музеем"));
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // Центральный виджет
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
}

void MainWindow::createActions()
{
    // Навигация
    m_dashboardAction = new QAction(tr("Панель управления"), this);
    m_dashboardAction->setShortcut(QKeySequence("Ctrl+1"));
    m_dashboardAction->setStatusTip(tr("Перейти на панель управления"));
    m_dashboardAction->setCheckable(true);
    connect(m_dashboardAction, &QAction::triggered, this, &MainWindow::showDashboard);
    
    m_exhibitsAction = new QAction(tr("Экспонаты"), this);
    m_exhibitsAction->setShortcut(QKeySequence("Ctrl+2"));
    m_exhibitsAction->setStatusTip(tr("Управление экспонатами"));
    m_exhibitsAction->setCheckable(true);
    connect(m_exhibitsAction, &QAction::triggered, this, &MainWindow::showExhibits);
    
    m_collectionsAction = new QAction(tr("Коллекции"), this);
    m_collectionsAction->setShortcut(QKeySequence("Ctrl+3"));
    m_collectionsAction->setStatusTip(tr("Управление коллекциями"));
    m_collectionsAction->setCheckable(true);
    connect(m_collectionsAction, &QAction::triggered, this, &MainWindow::showCollections);
    
    m_exhibitionsAction = new QAction(tr("Выставки"), this);
    m_exhibitionsAction->setShortcut(QKeySequence("Ctrl+4"));
    m_exhibitionsAction->setStatusTip(tr("Управление выставками"));
    m_exhibitionsAction->setCheckable(true);
    connect(m_exhibitionsAction, &QAction::triggered, this, &MainWindow::showExhibitions);
    
    m_employeesAction = new QAction(tr("Сотрудники"), this);
    m_employeesAction->setShortcut(QKeySequence("Ctrl+5"));
    m_employeesAction->setStatusTip(tr("Управление сотрудниками"));
    m_employeesAction->setCheckable(true);
    connect(m_employeesAction, &QAction::triggered, this, &MainWindow::showEmployees);
    
    m_storagesAction = new QAction(tr("Хранилища"), this);
    m_storagesAction->setShortcut(QKeySequence("Ctrl+6"));
    m_storagesAction->setStatusTip(tr("Управление хранилищами"));
    m_storagesAction->setCheckable(true);
    connect(m_storagesAction, &QAction::triggered, this, &MainWindow::showStorages);
    
    m_reportsAction = new QAction(tr("Отчёты"), this);
    m_reportsAction->setShortcut(QKeySequence("Ctrl+7"));
    m_reportsAction->setStatusTip(tr("Формирование отчётов"));
    m_reportsAction->setCheckable(true);
    connect(m_reportsAction, &QAction::triggered, this, &MainWindow::showReports);
    
    // Группа для взаимоисключающего выбора
    QActionGroup *navGroup = new QActionGroup(this);
    navGroup->addAction(m_dashboardAction);
    navGroup->addAction(m_exhibitsAction);
    navGroup->addAction(m_collectionsAction);
    navGroup->addAction(m_exhibitionsAction);
    navGroup->addAction(m_employeesAction);
    navGroup->addAction(m_storagesAction);
    navGroup->addAction(m_reportsAction);
    
    // Действия меню
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
    // Меню Файл
    QMenu *fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(m_exitAction);
    
    // Меню Навигация
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
    
    // Меню Справка
    QMenu *helpMenu = menuBar()->addMenu(tr("&Справка"));
    helpMenu->addAction(m_aboutAction);
    helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::createToolBar()
{
    m_mainToolBar = addToolBar(tr("Навигация"));
    m_mainToolBar->setMovable(false);
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_mainToolBar->setIconSize(QSize(24, 24));
    
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
    m_statusLabel = new QLabel(tr("Готово"));
    m_userLabel = new QLabel(tr("Пользователь: Администратор"));
    
    statusBar()->addWidget(m_statusLabel, 1);
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
    
    // Подключаем сигналы для обновления статуса
    connect(m_dashboardWidget, &DashboardWidget::statusMessage, 
            m_statusLabel, &QLabel::setText);
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        
        QToolBar {
            background-color: #2c3e50;
            border: none;
            spacing: 5px;
            padding: 5px;
        }
        
        QToolBar QToolButton {
            background-color: transparent;
            color: #ecf0f1;
            border: none;
            padding: 8px 12px;
            border-radius: 4px;
            font-size: 13px;
        }
        
        QToolBar QToolButton:hover {
            background-color: #34495e;
        }
        
        QToolBar QToolButton:checked {
            background-color: #3498db;
        }
        
        QMenuBar {
            background-color: #2c3e50;
            color: #ecf0f1;
        }
        
        QMenuBar::item:selected {
            background-color: #34495e;
        }
        
        QMenu {
            background-color: #fff;
            border: 1px solid #bdc3c7;
        }
        
        QMenu::item:selected {
            background-color: #3498db;
            color: #fff;
        }
        
        QStatusBar {
            background-color: #2c3e50;
            color: #ecf0f1;
        }
        
        QTableView {
            gridline-color: #bdc3c7;
            selection-background-color: #3498db;
            selection-color: #fff;
            alternate-background-color: #f9f9f9;
        }
        
        QTableView::item:hover {
            background-color: #e8f4fc;
        }
        
        QHeaderView::section {
            background-color: #34495e;
            color: #fff;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        QPushButton {
            background-color: #3498db;
            color: #fff;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-size: 13px;
        }
        
        QPushButton:hover {
            background-color: #2980b9;
        }
        
        QPushButton:pressed {
            background-color: #2472a4;
        }
        
        QPushButton:disabled {
            background-color: #bdc3c7;
        }
        
        QPushButton#deleteButton {
            background-color: #e74c3c;
        }
        
        QPushButton#deleteButton:hover {
            background-color: #c0392b;
        }
        
        QLineEdit, QTextEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            padding: 6px;
            background-color: #fff;
        }
        
        QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, 
        QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {
            border-color: #3498db;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            margin-top: 10px;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        
        QTabWidget::pane {
            border: 1px solid #bdc3c7;
            border-radius: 4px;
        }
        
        QTabBar::tab {
            background-color: #ecf0f1;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: #3498db;
            color: #fff;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: #bdc3c7;
        }
    )";
    
    setStyleSheet(styleSheet);
}

void MainWindow::showDashboard()
{
    m_stackedWidget->setCurrentWidget(m_dashboardWidget);
    m_dashboardAction->setChecked(true);
    m_dashboardWidget->refresh();
    m_statusLabel->setText(tr("Панель управления"));
}

void MainWindow::showExhibits()
{
    m_stackedWidget->setCurrentWidget(m_exhibitWidget);
    m_exhibitsAction->setChecked(true);
    m_exhibitWidget->refresh();
    m_statusLabel->setText(tr("Управление экспонатами"));
}

void MainWindow::showCollections()
{
    m_stackedWidget->setCurrentWidget(m_collectionWidget);
    m_collectionsAction->setChecked(true);
    m_collectionWidget->refresh();
    m_statusLabel->setText(tr("Управление коллекциями"));
}

void MainWindow::showExhibitions()
{
    m_stackedWidget->setCurrentWidget(m_exhibitionWidget);
    m_exhibitionsAction->setChecked(true);
    m_exhibitionWidget->refresh();
    m_statusLabel->setText(tr("Управление выставками"));
}

void MainWindow::showEmployees()
{
    m_stackedWidget->setCurrentWidget(m_employeeWidget);
    m_employeesAction->setChecked(true);
    m_employeeWidget->refresh();
    m_statusLabel->setText(tr("Управление сотрудниками"));
}

void MainWindow::showStorages()
{
    m_stackedWidget->setCurrentWidget(m_storageWidget);
    m_storagesAction->setChecked(true);
    m_storageWidget->refresh();
    m_statusLabel->setText(tr("Управление хранилищами"));
}

void MainWindow::showReports()
{
    m_stackedWidget->setCurrentWidget(m_reportsWidget);
    m_reportsAction->setChecked(true);
    m_statusLabel->setText(tr("Формирование отчётов"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе"),
        tr("<h2>Система управления музеем</h2>"
           "<p>Версия 1.0.0</p>"
           "<p>Приложение для учёта и управления музейными фондами.</p>"
           "<p><b>Возможности:</b></p>"
           "<ul>"
           "<li>Учёт экспонатов</li>"
           "<li>Управление коллекциями</li>"
           "<li>Организация выставок</li>"
           "<li>Учёт реставрационных работ</li>"
           "<li>Управление хранилищами</li>"
           "<li>Формирование отчётов</li>"
           "</ul>"
           "<p>© 2024 Курсовой проект</p>"));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("О Qt"));
}

void MainWindow::updateStatusBar()
{
    DatabaseManager& db = DatabaseManager::instance();
    QString info = QString(tr("Экспонатов: %1 | Коллекций: %2 | Выставок: %3 | Сотрудников: %4"))
                       .arg(db.getExhibitCount())
                       .arg(db.getCollectionCount())
                       .arg(db.getExhibitionCount())
                       .arg(db.getEmployeeCount());
    m_statusLabel->setText(info);
}
