#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QFont>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include "mainwindow.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ========================================
    // НАСТРОЙКИ ПОДКЛЮЧЕНИЯ К БАЗЕ ДАННЫХ
    // ========================================
    QString host     = "localhost";
    int     port     = 3306;
    QString database = "museum";
    QString user     = "root";
    QString password = "";  // <-- ВВЕДИ СВОЙ ПАРОЛЬ
    // ========================================

    app.setApplicationName("Система управления музеем");
    app.setApplicationVersion("1.0.0");
    app.setStyle(QStyleFactory::create("Fusion"));

    QFont defaultFont("Segoe UI", 10);
    app.setFont(defaultFont);

    // Простые рабочие стили
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f2f5;
        }

        QTableView {
            background-color: white;
            alternate-background-color: #f8f9fa;
            selection-background-color: #0d6efd;
            selection-color: white;
            border: 1px solid #dee2e6;
            border-radius: 4px;
            gridline-color: #e9ecef;
        }

        QTableView::item {
            padding: 8px;
            border-bottom: 1px solid #e9ecef;
        }

        QTableView::item:hover {
            background-color: #e7f1ff;
        }

        QHeaderView::section {
            background-color: #495057;
            color: white;
            padding: 10px 8px;
            border: none;
            font-weight: bold;
        }

        QPushButton {
            background-color: #0d6efd;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 6px;
            font-weight: bold;
            font-size: 13px;
        }

        QPushButton:hover {
            background-color: #0b5ed7;
        }

        QPushButton:pressed {
            background-color: #0a58ca;
        }

        QPushButton:disabled {
            background-color: #6c757d;
        }

        QPushButton#deleteButton {
            background-color: #dc3545;
        }

        QPushButton#deleteButton:hover {
            background-color: #bb2d3b;
        }

        QPushButton#successButton {
            background-color: #198754;
        }

        QPushButton#successButton:hover {
            background-color: #157347;
        }

        QLineEdit, QTextEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit {
            border: 1px solid #ced4da;
            border-radius: 4px;
            padding: 8px;
            background-color: white;
            min-height: 20px;
        }

        QLineEdit:focus, QTextEdit:focus, QComboBox:focus,
        QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
            border: 2px solid #0d6efd;
        }

        QComboBox::drop-down {
            border: none;
            width: 30px;
        }

        QComboBox::down-arrow {
            width: 12px;
            height: 12px;
        }

        QGroupBox {
            font-weight: bold;
            border: 1px solid #dee2e6;
            border-radius: 6px;
            margin-top: 14px;
            padding-top: 14px;
            background-color: white;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            color: #212529;
        }

        QTabWidget::pane {
            border: 1px solid #dee2e6;
            border-radius: 4px;
            background-color: white;
        }

        QTabBar::tab {
            background-color: #e9ecef;
            border: 1px solid #dee2e6;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }

        QTabBar::tab:selected {
            background-color: white;
            border-bottom-color: white;
        }

        QTabBar::tab:hover:!selected {
            background-color: #dee2e6;
        }

        QCheckBox {
            spacing: 8px;
        }

        QCheckBox::indicator {
            width: 18px;
            height: 18px;
        }

        QToolBar {
            background-color: #343a40;
            border: none;
            spacing: 5px;
            padding: 8px;
        }

        QToolBar QToolButton {
            background-color: transparent;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }

        QToolBar QToolButton:hover {
            background-color: #495057;
        }

        QToolBar QToolButton:checked {
            background-color: #0d6efd;
        }

        QMenuBar {
            background-color: #343a40;
            color: white;
        }

        QMenuBar::item:selected {
            background-color: #495057;
        }

        QMenu {
            background-color: white;
            border: 1px solid #dee2e6;
        }

        QMenu::item {
            padding: 8px 30px;
        }

        QMenu::item:selected {
            background-color: #0d6efd;
            color: white;
        }

        QStatusBar {
            background-color: #343a40;
            color: white;
        }

        QDialog {
            background-color: #f8f9fa;
        }

        QMessageBox {
            background-color: white;
        }

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

        QScrollBar::handle:vertical:hover {
            background-color: #a1a1a1;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");

    // Подключение к базе данных с выводом информации
    qDebug() << "Подключение к БД:" << host << port << database << user;

    if (!DatabaseManager::instance().initialize(host, port, database, user, password)) {
        QMessageBox::critical(nullptr, "Ошибка подключения",
                              "Не удалось подключиться к базе данных.\n\n"
                              "Проверьте:\n"
                              "1. Запущен ли MySQL/MariaDB сервер\n"
                              "2. Правильность пароля в main.cpp\n"
                              "3. Существует ли база данных 'museum'\n\n"
                              "Подробности в консоли.");
        return 1;
    }

    // Проверяем наличие данных
    QSqlQuery testQuery(DatabaseManager::instance().database());
    testQuery.exec("SELECT COUNT(*) FROM exhibits");
    if (testQuery.next()) {
        int count = testQuery.value(0).toInt();
        qDebug() << "Экспонатов в БД:" << count;
        if (count == 0) {
            QMessageBox::warning(nullptr, "Нет данных",
                                 "База данных пуста.\n\n"
                                 "Выполните скрипт test_data.sql в phpMyAdmin\n"
                                 "для добавления тестовых данных.");
        }
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
