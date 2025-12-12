// Файл: src/widgets/reportswidget.cpp
// Действие: ПОЛНОСТЬЮ ЗАМЕНИТЬ

#include "reportswidget.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QGraphicsDropShadowEffect>

ReportsWidget::ReportsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ReportsWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Заголовок
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("📊 Формирование отчётов"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    headerLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Создание и экспорт аналитических отчётов"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerWidget);

    // Панель параметров
    QWidget *paramsCard = new QWidget();
    paramsCard->setStyleSheet("QWidget { background-color: #ffffff; border-radius: 12px; }");
    QGraphicsDropShadowEffect *paramsShadow = new QGraphicsDropShadowEffect();
    paramsShadow->setBlurRadius(15);
    paramsShadow->setColor(QColor(0, 0, 0, 20));
    paramsShadow->setOffset(0, 2);
    paramsCard->setGraphicsEffect(paramsShadow);

    QHBoxLayout *paramsLayout = new QHBoxLayout(paramsCard);
    paramsLayout->setContentsMargins(20, 16, 20, 16);
    paramsLayout->setSpacing(20);

    QLabel *typeLabel = new QLabel(tr("📋 Тип отчёта:"));
    typeLabel->setStyleSheet("font-weight: 600; color: #4a5568;");
    paramsLayout->addWidget(typeLabel);

    m_reportTypeCombo = new QComboBox();
    m_reportTypeCombo->setMinimumWidth(200);
    m_reportTypeCombo->addItems({
        tr("Каталог экспонатов"),
        tr("Отчёт по коллекциям"),
        tr("Отчёт по выставкам"),
        tr("Отчёт о реставрации"),
        tr("Отчёт по хранилищам"),
        tr("Отчёт по сотрудникам"),
        tr("Общая статистика")
    });
    connect(m_reportTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportsWidget::onReportTypeChanged);
    paramsLayout->addWidget(m_reportTypeCombo);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setStyleSheet("background-color: #e2e8f0;");
    paramsLayout->addWidget(separator);

    QLabel *periodLabel = new QLabel(tr("📅 Период:"));
    periodLabel->setStyleSheet("font-weight: 600; color: #4a5568;");
    paramsLayout->addWidget(periodLabel);

    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDate(QDate::currentDate().addYears(-1));
    m_startDateEdit->setDisplayFormat("dd.MM.yyyy");
    paramsLayout->addWidget(m_startDateEdit);

    QLabel *toLabel = new QLabel(tr("—"));
    toLabel->setStyleSheet("color: #718096;");
    paramsLayout->addWidget(toLabel);

    m_endDateEdit = new QDateEdit();
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDate(QDate::currentDate());
    m_endDateEdit->setDisplayFormat("dd.MM.yyyy");
    paramsLayout->addWidget(m_endDateEdit);

    paramsLayout->addStretch();
    mainLayout->addWidget(paramsCard);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_generateButton = new QPushButton(tr("📄 Сформировать отчёт"));
    m_generateButton->setMinimumWidth(180);
    m_generateButton->setObjectName("successButton");
    connect(m_generateButton, &QPushButton::clicked, this, &ReportsWidget::onGenerateReport);
    buttonLayout->addWidget(m_generateButton);

    m_exportButton = new QPushButton(tr("💾 Экспорт в файл"));
    m_exportButton->setMinimumWidth(150);
    m_exportButton->setEnabled(false);
    connect(m_exportButton, &QPushButton::clicked, this, &ReportsWidget::onExportReport);
    buttonLayout->addWidget(m_exportButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Область отчёта
    QWidget *reportCard = new QWidget();
    reportCard->setStyleSheet("QWidget { background-color: #ffffff; border-radius: 12px; }");
    QGraphicsDropShadowEffect *reportShadow = new QGraphicsDropShadowEffect();
    reportShadow->setBlurRadius(20);
    reportShadow->setColor(QColor(0, 0, 0, 25));
    reportShadow->setOffset(0, 4);
    reportCard->setGraphicsEffect(reportShadow);

    QVBoxLayout *reportLayout = new QVBoxLayout(reportCard);
    reportLayout->setContentsMargins(0, 0, 0, 0);

    m_reportView = new QTextEdit();
    m_reportView->setReadOnly(true);
    m_reportView->setFrameShape(QFrame::NoFrame);
    m_reportView->setStyleSheet(
        "QTextEdit { background-color: #ffffff; font-family: 'Segoe UI', Arial; padding: 20px; }"
        );
    reportLayout->addWidget(m_reportView);

    mainLayout->addWidget(reportCard, 1);
}

void ReportsWidget::onReportTypeChanged(int index)
{
    Q_UNUSED(index)
}

void ReportsWidget::onGenerateReport()
{
    int reportType = m_reportTypeCombo->currentIndex();

    switch (reportType) {
    case 0: generateExhibitReport(); break;
    case 1: generateCollectionReport(); break;
    case 2: generateExhibitionReport(); break;
    case 3: generateRestorationReport(); break;
    case 4: generateStorageReport(); break;
    case 5: generateEmployeeReport(); break;
    case 6: generateStatisticsReport(); break;
    }

    m_exportButton->setEnabled(!m_currentReport.isEmpty());
}

void ReportsWidget::generateExhibitReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #3182ce; padding-bottom: 10px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th { background: linear-gradient(135deg, #2d3748, #4a5568); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #f7fafc; }
            .summary { background: #ebf8ff; padding: 15px; border-radius: 8px; margin-top: 20px; }
        </style>
    )";
    html += "<h2>🖼️ Каталог экспонатов</h2>";
    html += QString("<p style='color: #718096;'>📅 Дата формирования: %1</p>")
                .arg(QDate::currentDate().toString("dd.MM.yyyy"));

    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT e.inventory_number, e.name, c.name as category,
               e.author, e.creation_date, e.status
        FROM exhibits e
        LEFT JOIN categories c ON e.category_id = c.id
        ORDER BY e.inventory_number
    )");

    html += "<table>";
    html += "<tr><th>Инв. №</th><th>Название</th><th>Категория</th>"
            "<th>Автор</th><th>Дата создания</th><th>Статус</th></tr>";

    int count = 0;
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
        .arg(query.value(0).toString())
            .arg(query.value(1).toString())
            .arg(query.value(2).toString())
            .arg(query.value(3).toString())
            .arg(query.value(4).toString())
            .arg(query.value(5).toString());
        count++;
    }
    html += "</table>";
    html += QString("<div class='summary'><strong>📊 Всего экспонатов: %1</strong></div>").arg(count);

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateCollectionReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #38a169; padding-bottom: 10px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th { background: linear-gradient(135deg, #276749, #38a169); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #f0fff4; }
        </style>
    )";
    html += "<h2>📁 Отчёт по коллекциям</h2>";
    html += QString("<p style='color: #718096;'>📅 Дата формирования: %1</p>")
                .arg(QDate::currentDate().toString("dd.MM.yyyy"));

    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT c.name,
               CONCAT(e.last_name, ' ', e.first_name) as curator,
               DATE_FORMAT(c.creation_date, '%d.%m.%Y') as creation_date,
               CASE WHEN c.is_permanent = 1 THEN 'Да' ELSE 'Нет' END as permanent,
               (SELECT COUNT(*) FROM exhibits WHERE collection_id = c.id) as exhibit_count
        FROM collections c
        LEFT JOIN employees e ON c.curator_id = e.id
        WHERE c.is_active = 1
        ORDER BY c.name
    )");

    html += "<table>";
    html += "<tr><th>Название</th><th>Куратор</th><th>Дата создания</th>"
            "<th>Постоянная</th><th>Экспонатов</th></tr>";

    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td></tr>")
        .arg(query.value(0).toString())
            .arg(query.value(1).toString())
            .arg(query.value(2).toString())
            .arg(query.value(3).toString())
            .arg(query.value(4).toString());
    }
    html += "</table>";

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateExhibitionReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #805ad5; padding-bottom: 10px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th { background: linear-gradient(135deg, #553c9a, #805ad5); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #faf5ff; }
            .summary { background: #faf5ff; padding: 15px; border-radius: 8px; margin-top: 20px; }
        </style>
    )";
    html += "<h2>🎭 Отчёт по выставкам</h2>";
    html += QString("<p style='color: #718096;'>📅 Период: %1 — %2</p>")
                .arg(m_startDateEdit->date().toString("dd.MM.yyyy"))
                .arg(m_endDateEdit->date().toString("dd.MM.yyyy"));

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT name, location,
               DATE_FORMAT(start_date, '%d.%m.%Y') as start_date,
               DATE_FORMAT(end_date, '%d.%m.%Y') as end_date,
               status, visitor_count, budget
        FROM exhibitions
        WHERE start_date >= :start_date AND start_date <= :end_date
        ORDER BY start_date DESC
    )");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    query.exec();

    html += "<table>";
    html += "<tr><th>Название</th><th>Место</th><th>Начало</th><th>Окончание</th>"
            "<th>Статус</th><th>Посетителей</th><th>Бюджет</th></tr>";

    int totalVisitors = 0;
    double totalBudget = 0;
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td>"
                        "<td>%5</td><td>%6</td><td>%7 ₽</td></tr>")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toString())
                    .arg(query.value(2).toString())
                    .arg(query.value(3).toString())
                    .arg(query.value(4).toString())
                    .arg(query.value(5).toInt())
                    .arg(QString::number(query.value(6).toDouble(), 'f', 2));
        totalVisitors += query.value(5).toInt();
        totalBudget += query.value(6).toDouble();
    }
    html += "</table>";
    html += QString("<div class='summary'>"
                    "<strong>👥 Всего посетителей: %1</strong><br>"
                    "<strong>💰 Общий бюджет: %2 ₽</strong></div>")
                .arg(totalVisitors)
                .arg(QString::number(totalBudget, 'f', 2));

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateRestorationReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #e53e3e; padding-bottom: 10px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th { background: linear-gradient(135deg, #c53030, #e53e3e); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #fff5f5; }
            .summary { background: #fff5f5; padding: 15px; border-radius: 8px; margin-top: 20px; }
        </style>
    )";
    html += "<h2>🔧 Отчёт о реставрационных работах</h2>";
    html += QString("<p style='color: #718096;'>📅 Период: %1 — %2</p>")
                .arg(m_startDateEdit->date().toString("dd.MM.yyyy"))
                .arg(m_endDateEdit->date().toString("dd.MM.yyyy"));

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT e.name, rt.name as type,
               DATE_FORMAT(r.start_date, '%d.%m.%Y') as start_date,
               DATE_FORMAT(r.end_date, '%d.%m.%Y') as end_date,
               emp.last_name as restorer, r.status, r.cost
        FROM restorations r
        JOIN exhibits e ON r.exhibit_id = e.id
        LEFT JOIN restoration_types rt ON r.restoration_type_id = rt.id
        LEFT JOIN employees emp ON r.restorer_id = emp.id
        WHERE r.start_date >= :start_date AND r.start_date <= :end_date
        ORDER BY r.start_date DESC
    )");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    query.exec();

    html += "<table>";
    html += "<tr><th>Экспонат</th><th>Тип работ</th><th>Начало</th><th>Окончание</th>"
            "<th>Реставратор</th><th>Статус</th><th>Стоимость</th></tr>";

    double totalCost = 0;
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td>"
                        "<td>%5</td><td>%6</td><td>%7 ₽</td></tr>")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toString())
                    .arg(query.value(2).toString())
                    .arg(query.value(3).toString())
                    .arg(query.value(4).toString())
                    .arg(query.value(5).toString())
                    .arg(QString::number(query.value(6).toDouble(), 'f', 2));
        totalCost += query.value(6).toDouble();
    }
    html += "</table>";
    html += QString("<div class='summary'><strong>💰 Общая стоимость работ: %1 ₽</strong></div>")
                .arg(QString::number(totalCost, 'f', 2));

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateStorageReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #dd6b20; padding-bottom: 10px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th { background: linear-gradient(135deg, #c05621, #dd6b20); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #fffaf0; }
        </style>
    )";
    html += "<h2>📦 Отчёт по хранилищам</h2>";
    html += QString("<p style='color: #718096;'>📅 Дата формирования: %1</p>")
                .arg(QDate::currentDate().toString("dd.MM.yyyy"));

    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT s.name, s.storage_type, s.location, s.capacity,
               (SELECT COUNT(*) FROM exhibits WHERE storage_id = s.id) as used,
               CASE WHEN s.climate_control = 1 THEN 'Да' ELSE 'Нет' END as climate
        FROM storages s
        WHERE s.is_active = 1
        ORDER BY s.name
    )");

    html += "<table>";
    html += "<tr><th>Название</th><th>Тип</th><th>Расположение</th>"
            "<th>Вместимость</th><th>Занято</th><th>Климат-контроль</th></tr>";

    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
        .arg(query.value(0).toString())
            .arg(query.value(1).toString())
            .arg(query.value(2).toString())
            .arg(query.value(3).toInt())
            .arg(query.value(4).toInt())
            .arg(query.value(5).toString());
    }
    html += "</table>";

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateEmployeeReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #319795; padding-bottom: 10px; }
            table { border-collapse: collapse; width: 100%; margin-top: 20px; }
            th { background: linear-gradient(135deg, #285e61, #319795); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #e6fffa; }
            .summary { background: #e6fffa; padding: 15px; border-radius: 8px; margin-top: 20px; }
        </style>
    )";
    html += "<h2>👥 Отчёт по сотрудникам</h2>";
    html += QString("<p style='color: #718096;'>📅 Дата формирования: %1</p>")
                .arg(QDate::currentDate().toString("dd.MM.yyyy"));

    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT CONCAT(e.last_name, ' ', e.first_name,
               CASE WHEN e.middle_name IS NOT NULL AND e.middle_name != ''
                    THEN CONCAT(' ', e.middle_name) ELSE '' END) as name,
               p.name as position, e.phone, e.email,
               DATE_FORMAT(e.hire_date, '%d.%m.%Y') as hire_date
        FROM employees e
        JOIN positions p ON e.position_id = p.id
        WHERE e.is_active = 1
        ORDER BY e.last_name, e.first_name
    )");

    html += "<table>";
    html += "<tr><th>ФИО</th><th>Должность</th><th>Телефон</th><th>Email</th><th>Дата приёма</th></tr>";

    int count = 0;
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td></tr>")
        .arg(query.value(0).toString())
            .arg(query.value(1).toString())
            .arg(query.value(2).toString())
            .arg(query.value(3).toString())
            .arg(query.value(4).toString());
        count++;
    }
    html += "</table>";
    html += QString("<div class='summary'><strong>📊 Всего сотрудников: %1</strong></div>").arg(count);

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateStatisticsReport()
{
    QString html = R"(
        <style>
            body { font-family: 'Segoe UI', Arial, sans-serif; color: #2d3748; }
            h2 { color: #1a202c; border-bottom: 3px solid #3182ce; padding-bottom: 10px; }
            h3 { color: #2d3748; margin-top: 30px; }
            table { border-collapse: collapse; width: 100%; margin-top: 15px; }
            th { background: linear-gradient(135deg, #2b6cb0, #3182ce); color: white; padding: 12px; text-align: left; }
            td { padding: 10px 12px; border-bottom: 1px solid #e2e8f0; }
            tr:hover { background-color: #ebf8ff; }
            .stat-card { background: linear-gradient(135deg, #ebf8ff, #bee3f8); padding: 20px; border-radius: 12px; margin: 10px 0; }
            .stat-value { font-size: 32px; font-weight: 700; color: #2b6cb0; }
            .stat-label { color: #4a5568; font-size: 14px; }
        </style>
    )";
    html += "<h2>📊 Общая статистика музея</h2>";
    html += QString("<p style='color: #718096;'>📅 Дата формирования: %1</p>")
                .arg(QDate::currentDate().toString("dd.MM.yyyy"));

    DatabaseManager& db = DatabaseManager::instance();

    html += "<h3>📈 Основные показатели</h3>";
    html += "<table>";
    html += QString("<tr><td><strong>🖼️ Всего экспонатов</strong></td><td style='font-size: 18px; color: #3182ce;'><strong>%1</strong></td></tr>").arg(db.getExhibitCount());
    html += QString("<tr><td><strong>📁 Активных коллекций</strong></td><td style='font-size: 18px; color: #38a169;'><strong>%1</strong></td></tr>").arg(db.getCollectionCount());
    html += QString("<tr><td><strong>🎭 Активных выставок</strong></td><td style='font-size: 18px; color: #805ad5;'><strong>%1</strong></td></tr>").arg(db.getExhibitionCount());
    html += QString("<tr><td><strong>👥 Сотрудников</strong></td><td style='font-size: 18px; color: #dd6b20;'><strong>%1</strong></td></tr>").arg(db.getEmployeeCount());
    html += QString("<tr><td><strong>✨ На выставке</strong></td><td style='font-size: 18px; color: #319795;'><strong>%1</strong></td></tr>").arg(db.getExhibitsOnDisplay());
    html += QString("<tr><td><strong>🔧 На реставрации</strong></td><td style='font-size: 18px; color: #e53e3e;'><strong>%1</strong></td></tr>").arg(db.getExhibitsInRestoration());
    html += "</table>";

    html += "<h3>📊 Экспонаты по категориям</h3>";
    html += "<table><tr><th>Категория</th><th>Количество</th></tr>";

    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT c.name, COUNT(e.id) as cnt
        FROM categories c
        LEFT JOIN exhibits e ON e.category_id = c.id
        GROUP BY c.id
        ORDER BY cnt DESC
    )");

    while (query.next()) {
        html += QString("<tr><td>%1</td><td><strong>%2</strong></td></tr>")
        .arg(query.value(0).toString())
            .arg(query.value(1).toInt());
    }
    html += "</table>";

    html += "<h3>📋 Экспонаты по статусам</h3>";
    html += "<table><tr><th>Статус</th><th>Количество</th></tr>";

    query.exec(R"(
        SELECT status, COUNT(*) as cnt
        FROM exhibits
        GROUP BY status
        ORDER BY cnt DESC
    )");

    while (query.next()) {
        html += QString("<tr><td>%1</td><td><strong>%2</strong></td></tr>")
        .arg(query.value(0).toString())
            .arg(query.value(1).toInt());
    }
    html += "</table>";

    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::onExportReport()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("💾 Экспорт отчёта"), "",
                                                    tr("HTML файлы (*.html);;Текстовые файлы (*.txt)"));

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось сохранить файл"));
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    if (fileName.endsWith(".html", Qt::CaseInsensitive)) {
        out << "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
            << "<title>Отчёт музея</title></head><body>"
            << m_currentReport << "</body></html>";
    } else {
        out << m_reportView->toPlainText();
    }

    file.close();
    QMessageBox::information(this, tr("✅ Успех"), tr("Отчёт успешно сохранён"));
}
