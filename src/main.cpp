// Файл: src/main.cpp
// ПОЛНОСТЬЮ ЗАМЕНИТЬ существующий файл main.cpp

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

    // Установка шрифта
    QFont defaultFont("Segoe UI", 10);
    defaultFont.setStyleStrategy(QFont::PreferAntialias);
    app.setFont(defaultFont);

    // ========================================
    // ПРОФЕССИОНАЛЬНЫЕ СТИЛИ ДЛЯ КУРСОВОЙ
    // ========================================
    app.setStyleSheet(R"(
        /* ===== ОСНОВНОЕ ОКНО ===== */
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #f5f7fa, stop:1 #e4e8ec);
        }

        QWidget {
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        /* ===== ТАБЛИЦЫ ===== */
        QTableView {
            background-color: #ffffff;
            alternate-background-color: #f8fafc;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            gridline-color: #e2e8f0;
            font-size: 10pt;
            outline: none;
            color: #1e293b;
        }

        QTableView::item {
            padding: 10px 8px;
            border-bottom: 1px solid #f1f5f9;
            color: #1e293b;
        }

        QTableView::item:hover {
            background-color: #eff6ff;
            color: #1e293b;
        }

        QTableView::item:selected {
            background-color: #3b82f6;
            color: #ffffff;
        }

        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1e3a5f, stop:1 #0f2744);
            color: #ffffff;
            padding: 12px 10px;
            border: none;
            border-right: 1px solid #2d4a6f;
            font-weight: 600;
            font-size: 10pt;
        }

        QHeaderView::section:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d4a6f, stop:1 #1e3a5f);
        }

        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }

        QHeaderView::section:last {
            border-top-right-radius: 8px;
            border-right: none;
        }

        /* ===== КНОПКИ ===== */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3b82f6, stop:1 #2563eb);
            color: #ffffff;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: 600;
            font-size: 10pt;
            min-width: 100px;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #60a5fa, stop:1 #3b82f6);
            color: #ffffff;
        }

        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1d4ed8, stop:1 #1e40af);
            color: #ffffff;
        }

        QPushButton:disabled {
            background: #94a3b8;
            color: #e2e8f0;
        }

        /* ===== ПОЛЯ ВВОДА ===== */
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            color: #1e293b;
        }

        QLineEdit:focus {
            border: 2px solid #3b82f6;
            background-color: #ffffff;
            color: #1e293b;
        }

        QLineEdit:hover {
            border: 2px solid #94a3b8;
        }

        QTextEdit, QPlainTextEdit {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            color: #1e293b;
        }

        QTextEdit:focus, QPlainTextEdit:focus {
            border: 2px solid #3b82f6;
            background-color: #ffffff;
            color: #1e293b;
        }

        /* ===== ВЫПАДАЮЩИЕ СПИСКИ ===== */
        QComboBox {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
            min-width: 150px;
            color: #1e293b;
        }

        QComboBox:hover {
            border: 2px solid #94a3b8;
        }

        QComboBox:focus {
            border: 2px solid #3b82f6;
        }

        QComboBox::drop-down {
            border: none;
            width: 30px;
        }

        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #64748b;
            margin-right: 10px;
        }

        QComboBox QAbstractItemView {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            padding: 4px;
            color: #1e293b;
        }

        QComboBox QAbstractItemView::item {
            padding: 8px 12px;
            border-radius: 4px;
            color: #1e293b;
        }

        QComboBox QAbstractItemView::item:hover {
            background-color: #eff6ff;
            color: #1e293b;
        }

        QComboBox QAbstractItemView::item:selected {
            background-color: #3b82f6;
            color: #ffffff;
        }

        /* ===== СПИНБОКСЫ ===== */
        QSpinBox, QDoubleSpinBox {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
            color: #1e293b;
        }

        QSpinBox:focus, QDoubleSpinBox:focus {
            border: 2px solid #3b82f6;
        }

        QSpinBox::up-button, QDoubleSpinBox::up-button,
        QSpinBox::down-button, QDoubleSpinBox::down-button {
            background-color: #f1f5f9;
            border: none;
            width: 20px;
        }

        QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover,
        QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
            background-color: #e2e8f0;
        }

        /* ===== КАЛЕНДАРЬ ===== */
        QDateEdit {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
            color: #1e293b;
        }

        QDateEdit:focus {
            border: 2px solid #3b82f6;
        }

        QDateEdit::drop-down {
            border: none;
            width: 30px;
        }

        QCalendarWidget {
            background-color: #ffffff;
            border-radius: 8px;
        }

        QCalendarWidget QToolButton {
            background-color: #3b82f6;
            color: #ffffff;
            border-radius: 4px;
            padding: 6px;
            margin: 2px;
        }

        QCalendarWidget QToolButton:hover {
            background-color: #60a5fa;
        }

        QCalendarWidget QAbstractItemView {
            color: #1e293b;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
        }

        /* ===== МЕНЮ ===== */
        QMenuBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1e3a5f, stop:1 #0f2744);
            color: #ffffff;
            padding: 4px;
            font-size: 10pt;
        }

        QMenuBar::item {
            background: transparent;
            padding: 8px 16px;
            border-radius: 4px;
            color: #ffffff;
        }

        QMenuBar::item:selected {
            background-color: rgba(255, 255, 255, 0.15);
            color: #ffffff;
        }

        QMenuBar::item:pressed {
            background-color: rgba(255, 255, 255, 0.25);
            color: #ffffff;
        }

        QMenu {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 4px;
            color: #1e293b;
        }

        QMenu::item {
            padding: 10px 40px 10px 20px;
            border-radius: 4px;
            margin: 2px 4px;
            color: #1e293b;
        }

        QMenu::item:selected {
            background-color: #3b82f6;
            color: #ffffff;
        }

        QMenu::separator {
            height: 1px;
            background-color: #e2e8f0;
            margin: 6px 10px;
        }

        /* ===== ПАНЕЛЬ ИНСТРУМЕНТОВ ===== */
        QToolBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff, stop:1 #f8fafc);
            border-bottom: 1px solid #e2e8f0;
            padding: 8px;
            spacing: 8px;
        }

        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 10pt;
            color: #1e293b;
        }

        QToolButton:hover {
            background-color: #eff6ff;
            color: #1e293b;
        }

        QToolButton:pressed {
            background-color: #dbeafe;
            color: #1e293b;
        }

        QToolButton:checked {
            background-color: #3b82f6;
            color: #ffffff;
        }

        /* ===== СТАТУСНАЯ СТРОКА ===== */
        QStatusBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1e3a5f, stop:1 #0f2744);
            color: #ffffff;
            padding: 8px;
            font-size: 9pt;
        }

        QStatusBar::item {
            border: none;
        }

        QStatusBar QLabel {
            color: #ffffff;
        }

        /* ===== ВКЛАДКИ ===== */
        QTabWidget::pane {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 10px;
        }

        QTabBar::tab {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #f8fafc, stop:1 #e2e8f0);
            border: 1px solid #e2e8f0;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            padding: 10px 20px;
            margin-right: 2px;
            font-size: 10pt;
            color: #1e293b;
        }

        QTabBar::tab:selected {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3b82f6, stop:1 #2563eb);
            color: #ffffff;
            font-weight: 600;
        }

        QTabBar::tab:hover:!selected {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #eff6ff, stop:1 #dbeafe);
            color: #1e293b;
        }

        /* ===== ГРУППИРОВКА ===== */
        QGroupBox {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            margin-top: 16px;
            padding: 20px 15px 15px 15px;
            font-weight: 600;
            font-size: 10pt;
            color: #1e293b;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 15px;
            padding: 0 8px;
            background-color: #ffffff;
            color: #1e3a5f;
        }

        /* ===== МЕТКИ ===== */
        QLabel {
            color: #334155;
            font-size: 10pt;
        }

        /* ===== ДИАЛОГИ ===== */
        QDialog {
            background-color: #f8fafc;
            color: #1e293b;
        }

        QDialog QLabel {
            color: #1e293b;
        }

        QMessageBox {
            background-color: #ffffff;
            color: #1e293b;
        }

        QMessageBox QLabel {
            color: #1e293b;
        }

        QMessageBox QPushButton {
            min-width: 80px;
        }

        /* ===== ПОЛОСЫ ПРОКРУТКИ ===== */
        QScrollBar:vertical {
            background-color: #f1f5f9;
            width: 12px;
            border-radius: 6px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background-color: #94a3b8;
            border-radius: 6px;
            min-height: 40px;
            margin: 2px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #64748b;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background-color: #f1f5f9;
            height: 12px;
            border-radius: 6px;
            margin: 0;
        }

        QScrollBar::handle:horizontal {
            background-color: #94a3b8;
            border-radius: 6px;
            min-width: 40px;
            margin: 2px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #64748b;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* ===== ЧЕКБОКСЫ ===== */
        QCheckBox {
            spacing: 8px;
            font-size: 10pt;
            color: #1e293b;
        }

        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border: 2px solid #cbd5e1;
            border-radius: 4px;
            background-color: #ffffff;
        }

        QCheckBox::indicator:hover {
            border-color: #3b82f6;
        }

        QCheckBox::indicator:checked {
            background-color: #3b82f6;
            border-color: #3b82f6;
        }

        /* ===== РАДИОКНОПКИ ===== */
        QRadioButton {
            spacing: 8px;
            font-size: 10pt;
            color: #1e293b;
        }

        QRadioButton::indicator {
            width: 20px;
            height: 20px;
            border: 2px solid #cbd5e1;
            border-radius: 10px;
            background-color: #ffffff;
        }

        QRadioButton::indicator:hover {
            border-color: #3b82f6;
        }

        QRadioButton::indicator:checked {
            background-color: #3b82f6;
            border-color: #3b82f6;
        }

        /* ===== ПРОГРЕСС БАР ===== */
        QProgressBar {
            background-color: #e2e8f0;
            border: none;
            border-radius: 8px;
            height: 16px;
            text-align: center;
            font-size: 9pt;
            color: #1e293b;
        }

        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3b82f6, stop:1 #60a5fa);
            border-radius: 8px;
        }

        /* ===== РАЗДЕЛИТЕЛЬ ===== */
        QSplitter::handle {
            background-color: #e2e8f0;
        }

        QSplitter::handle:horizontal {
            width: 2px;
        }

        QSplitter::handle:vertical {
            height: 2px;
        }

        /* ===== ДЕРЕВО И СПИСКИ ===== */
        QTreeView, QListView, QListWidget {
            background-color: #ffffff;
            alternate-background-color: #f8fafc;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            outline: none;
            color: #1e293b;
        }

        QTreeView::item, QListView::item, QListWidget::item {
            padding: 8px;
            border-radius: 4px;
            color: #1e293b;
        }

        QTreeView::item:hover, QListView::item:hover, QListWidget::item:hover {
            background-color: #eff6ff;
            color: #1e293b;
        }

        QTreeView::item:selected, QListView::item:selected, QListWidget::item:selected {
            background-color: #3b82f6;
            color: #ffffff;
        }

        /* ===== ПОДСКАЗКИ ===== */
        QToolTip {
            background-color: #1e3a5f;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px 12px;
            font-size: 9pt;
        }

        /* ===== FRAME ===== */
        QFrame {
            color: #1e293b;
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
