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

ReportsWidget::ReportsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ReportsWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Заголовок
    QLabel *titleLabel = new QLabel(tr("Формирование отчётов"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);
    
    // Панель параметров
    QGroupBox *paramsGroup = new QGroupBox(tr("Параметры отчёта"));
    QHBoxLayout *paramsLayout = new QHBoxLayout(paramsGroup);
    
    paramsLayout->addWidget(new QLabel(tr("Тип отчёта:")));
    m_reportTypeCombo = new QComboBox();
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
    
    paramsLayout->addSpacing(20);
    
    paramsLayout->addWidget(new QLabel(tr("Период с:")));
    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDate(QDate::currentDate().addYears(-1));
    m_startDateEdit->setDisplayFormat("dd.MM.yyyy");
    paramsLayout->addWidget(m_startDateEdit);
    
    paramsLayout->addWidget(new QLabel(tr("по:")));
    m_endDateEdit = new QDateEdit();
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDate(QDate::currentDate());
    m_endDateEdit->setDisplayFormat("dd.MM.yyyy");
    paramsLayout->addWidget(m_endDateEdit);
    
    paramsLayout->addStretch();
    mainLayout->addWidget(paramsGroup);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_generateButton = new QPushButton(tr("Сформировать отчёт"));
    m_generateButton->setMinimumWidth(150);
    connect(m_generateButton, &QPushButton::clicked, this, &ReportsWidget::onGenerateReport);
    buttonLayout->addWidget(m_generateButton);
    
    m_exportButton = new QPushButton(tr("Экспорт в файл"));
    m_exportButton->setMinimumWidth(120);
    m_exportButton->setEnabled(false);
    connect(m_exportButton, &QPushButton::clicked, this, &ReportsWidget::onExportReport);
    buttonLayout->addWidget(m_exportButton);
    
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // Область отчёта
    m_reportView = new QTextEdit();
    m_reportView->setReadOnly(true);
    m_reportView->setStyleSheet("QTextEdit { background-color: white; font-family: 'Segoe UI', Arial; }");
    mainLayout->addWidget(m_reportView, 1);
}

void ReportsWidget::onReportTypeChanged(int index)
{
    Q_UNUSED(index)
    // Можно настроить видимость полей дат в зависимости от типа отчёта
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
    QString html = "<h2 style='color: #2c3e50;'>Каталог экспонатов</h2>";
    html += QString("<p><i>Дата формирования: %1</i></p>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT e.inventory_number, e.name, c.name as category, 
               e.author, e.creation_date, e.status
        FROM exhibits e
        LEFT JOIN categories c ON e.category_id = c.id
        ORDER BY e.inventory_number
    )");
    
    html += "<table border='1' cellpadding='5' cellspacing='0' width='100%'>";
    html += "<tr style='background-color: #34495e; color: white;'>"
            "<th>Инв. №</th><th>Название</th><th>Категория</th>"
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
    html += QString("<p><b>Всего экспонатов: %1</b></p>").arg(count);
    
    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateCollectionReport()
{
    QString html = "<h2 style='color: #2c3e50;'>Отчёт по коллекциям</h2>";
    html += QString("<p><i>Дата формирования: %1</i></p>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT c.name, 
               e.last_name || ' ' || e.first_name as curator,
               c.creation_date,
               CASE WHEN c.is_permanent = 1 THEN 'Да' ELSE 'Нет' END as permanent,
               (SELECT COUNT(*) FROM exhibits WHERE collection_id = c.id) as exhibit_count
        FROM collections c
        LEFT JOIN employees e ON c.curator_id = e.id
        WHERE c.is_active = 1
        ORDER BY c.name
    )");
    
    html += "<table border='1' cellpadding='5' cellspacing='0' width='100%'>";
    html += "<tr style='background-color: #34495e; color: white;'>"
            "<th>Название</th><th>Куратор</th><th>Дата создания</th>"
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
    QString html = "<h2 style='color: #2c3e50;'>Отчёт по выставкам</h2>";
    html += QString("<p><i>Период: %1 — %2</i></p>")
                .arg(m_startDateEdit->date().toString("dd.MM.yyyy"))
                .arg(m_endDateEdit->date().toString("dd.MM.yyyy"));
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT name, location, start_date, end_date, status, visitor_count, budget
        FROM exhibitions
        WHERE start_date >= :start_date AND start_date <= :end_date
        ORDER BY start_date DESC
    )");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    query.exec();
    
    html += "<table border='1' cellpadding='5' cellspacing='0' width='100%'>";
    html += "<tr style='background-color: #34495e; color: white;'>"
            "<th>Название</th><th>Место</th><th>Начало</th><th>Окончание</th>"
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
                    .arg(query.value(6).toDouble(), 0, 'f', 2);
        totalVisitors += query.value(5).toInt();
        totalBudget += query.value(6).toDouble();
    }
    html += "</table>";
    html += QString("<p><b>Всего посетителей: %1</b></p>").arg(totalVisitors);
    html += QString("<p><b>Общий бюджет: %1 ₽</b></p>").arg(totalBudget, 0, 'f', 2);
    
    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateRestorationReport()
{
    QString html = "<h2 style='color: #2c3e50;'>Отчёт о реставрационных работах</h2>";
    html += QString("<p><i>Период: %1 — %2</i></p>")
                .arg(m_startDateEdit->date().toString("dd.MM.yyyy"))
                .arg(m_endDateEdit->date().toString("dd.MM.yyyy"));
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT e.name, rt.name as type, r.start_date, r.end_date, 
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
    
    html += "<table border='1' cellpadding='5' cellspacing='0' width='100%'>";
    html += "<tr style='background-color: #34495e; color: white;'>"
            "<th>Экспонат</th><th>Тип работ</th><th>Начало</th><th>Окончание</th>"
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
                    .arg(query.value(6).toDouble(), 0, 'f', 2);
        totalCost += query.value(6).toDouble();
    }
    html += "</table>";
    html += QString("<p><b>Общая стоимость работ: %1 ₽</b></p>").arg(totalCost, 0, 'f', 2);
    
    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateStorageReport()
{
    QString html = "<h2 style='color: #2c3e50;'>Отчёт по хранилищам</h2>";
    html += QString("<p><i>Дата формирования: %1</i></p>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT s.name, s.storage_type, s.location, s.capacity,
               (SELECT COUNT(*) FROM exhibits WHERE storage_id = s.id) as used,
               CASE WHEN s.climate_control = 1 THEN 'Да' ELSE 'Нет' END as climate
        FROM storages s
        WHERE s.is_active = 1
        ORDER BY s.name
    )");
    
    html += "<table border='1' cellpadding='5' cellspacing='0' width='100%'>";
    html += "<tr style='background-color: #34495e; color: white;'>"
            "<th>Название</th><th>Тип</th><th>Расположение</th>"
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
    QString html = "<h2 style='color: #2c3e50;'>Отчёт по сотрудникам</h2>";
    html += QString("<p><i>Дата формирования: %1</i></p>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT e.last_name || ' ' || e.first_name || 
               CASE WHEN e.middle_name IS NOT NULL THEN ' ' || e.middle_name ELSE '' END as name,
               p.name as position, e.phone, e.email, e.hire_date
        FROM employees e
        JOIN positions p ON e.position_id = p.id
        WHERE e.is_active = 1
        ORDER BY e.last_name, e.first_name
    )");
    
    html += "<table border='1' cellpadding='5' cellspacing='0' width='100%'>";
    html += "<tr style='background-color: #34495e; color: white;'>"
            "<th>ФИО</th><th>Должность</th><th>Телефон</th><th>Email</th><th>Дата приёма</th></tr>";
    
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
    html += QString("<p><b>Всего сотрудников: %1</b></p>").arg(count);
    
    m_currentReport = html;
    m_reportView->setHtml(html);
}

void ReportsWidget::generateStatisticsReport()
{
    QString html = "<h2 style='color: #2c3e50;'>Общая статистика музея</h2>";
    html += QString("<p><i>Дата формирования: %1</i></p>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
    
    DatabaseManager& db = DatabaseManager::instance();
    
    html += "<h3>Основные показатели</h3>";
    html += "<table border='1' cellpadding='8' cellspacing='0'>";
    html += QString("<tr><td><b>Всего экспонатов</b></td><td>%1</td></tr>").arg(db.getExhibitCount());
    html += QString("<tr><td><b>Активных коллекций</b></td><td>%1</td></tr>").arg(db.getCollectionCount());
    html += QString("<tr><td><b>Активных выставок</b></td><td>%1</td></tr>").arg(db.getExhibitionCount());
    html += QString("<tr><td><b>Сотрудников</b></td><td>%1</td></tr>").arg(db.getEmployeeCount());
    html += QString("<tr><td><b>На выставке</b></td><td>%1</td></tr>").arg(db.getExhibitsOnDisplay());
    html += QString("<tr><td><b>На реставрации</b></td><td>%1</td></tr>").arg(db.getExhibitsInRestoration());
    html += "</table>";
    
    // Статистика по категориям
    html += "<h3>Экспонаты по категориям</h3>";
    html += "<table border='1' cellpadding='5' cellspacing='0'>";
    html += "<tr style='background-color: #34495e; color: white;'><th>Категория</th><th>Количество</th></tr>";
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec(R"(
        SELECT c.name, COUNT(e.id) as cnt
        FROM categories c
        LEFT JOIN exhibits e ON e.category_id = c.id
        GROUP BY c.id
        ORDER BY cnt DESC
    )");
    
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td></tr>")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toInt());
    }
    html += "</table>";
    
    // Статистика по статусам
    html += "<h3>Экспонаты по статусам</h3>";
    html += "<table border='1' cellpadding='5' cellspacing='0'>";
    html += "<tr style='background-color: #34495e; color: white;'><th>Статус</th><th>Количество</th></tr>";
    
    query.exec(R"(
        SELECT status, COUNT(*) as cnt
        FROM exhibits
        GROUP BY status
        ORDER BY cnt DESC
    )");
    
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td></tr>")
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
        tr("Экспорт отчёта"), "",
        tr("HTML файлы (*.html);;Текстовые файлы (*.txt)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось сохранить файл"));
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
    QMessageBox::information(this, tr("Успех"), tr("Отчёт успешно сохранён"));
}
