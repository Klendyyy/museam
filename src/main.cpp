#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QFont>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

#include "mainwindow.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ========================================
    // НАСТРОЙКИ ПОДКЛЮЧЕНИЯ К БАЗЕ ДАННЫХ
    // ========================================
    QString host     = "localhost";   // хост
    int     port     = 3306;          // порт
    QString database = "museum";      // имя базы данных
    QString user     = "root";        // логин
    QString password = "";       // <-- ВВЕДИ СВОЙ ПАРОЛЬ
    // ========================================

    app.setApplicationName("Система управления музеем");
    app.setApplicationVersion("1.0.0");
    app.setStyle(QStyleFactory::create("Fusion"));

    QFont defaultFont("Segoe UI", 10);
    defaultFont.setStyleHint(QFont::SansSerif);
    app.setFont(defaultFont);

    // Глобальные стили
    app.setStyleSheet(R"(
        QMainWindow { background-color: #ecf0f1; }
        QTableView {
            background-color: white;
            alternate-background-color: #f8f9fa;
            selection-background-color: #3498db;
            selection-color: white;
            gridline-color: #dee2e6;
            border: 1px solid #ced4da;
            border-radius: 4px;
        }
        QTableView::item { padding: 5px; }
        QTableView QHeaderView::section {
            background-color: #34495e;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            min-height: 20px;
        }
        QPushButton:hover { background-color: #2980b9; }
        QPushButton:pressed { background-color: #1c5a85; }
        QPushButton:disabled { background-color: #bdc3c7; }
        QPushButton#deleteButton { background-color: #e74c3c; }
        QPushButton#deleteButton:hover { background-color: #c0392b; }
        QLineEdit, QTextEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit {
            border: 1px solid #ced4da;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
            min-height: 20px;
        }
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus,
        QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
            border-color: #3498db;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ced4da;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #2c3e50;
        }
        QTabWidget::pane {
            border: 1px solid #ced4da;
            border-radius: 4px;
            background-color: white;
        }
        QTabBar::tab {
            background-color: #ecf0f1;
            border: 1px solid #ced4da;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom-color: white;
        }
        QTabBar::tab:hover:!selected { background-color: #d5dbdb; }
        QCheckBox { spacing: 8px; }
        QCheckBox::indicator { width: 18px; height: 18px; }
        QScrollBar:vertical {
            background-color: #f1f1f1;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #c1c1c1;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background-color: #a1a1a1; }
        QMessageBox { background-color: white; }
        QDialog { background-color: #f8f9fa; }
    )");

    // Подключение к базе данных
    if (!DatabaseManager::instance().initialize(host, port, database, user, password)) {
        QMessageBox::critical(nullptr, "Ошибка",
                              "Не удалось подключиться к базе данных.\n\n"
                              "Проверьте настройки подключения.");
        return 1;
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
