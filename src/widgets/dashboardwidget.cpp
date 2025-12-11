#include "dashboardwidget.h"
#include "databasemanager.h"
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QScrollArea>
#include <QGroupBox>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refresh();
}

void DashboardWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Заголовок
    QLabel *titleLabel = new QLabel(tr("Панель управления"));
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);
    
    // Карточки статистики
    QGridLayout *statsLayout = new QGridLayout();
    statsLayout->setSpacing(15);
    
    // Создаём карточки
    QFrame *exhibitCard = createStatCard(tr("Экспонаты"), "0", "#3498db");
    m_exhibitCountLabel = exhibitCard->findChild<QLabel*>("valueLabel");
    
    QFrame *collectionCard = createStatCard(tr("Коллекции"), "0", "#2ecc71");
    m_collectionCountLabel = collectionCard->findChild<QLabel*>("valueLabel");
    
    QFrame *exhibitionCard = createStatCard(tr("Активные выставки"), "0", "#9b59b6");
    m_exhibitionCountLabel = exhibitionCard->findChild<QLabel*>("valueLabel");
    
    QFrame *employeeCard = createStatCard(tr("Сотрудники"), "0", "#e67e22");
    m_employeeCountLabel = employeeCard->findChild<QLabel*>("valueLabel");
    
    QFrame *displayCard = createStatCard(tr("На выставке"), "0", "#1abc9c");
    m_onDisplayLabel = displayCard->findChild<QLabel*>("valueLabel");
    
    QFrame *restorationCard = createStatCard(tr("На реставрации"), "0", "#e74c3c");
    m_inRestorationLabel = restorationCard->findChild<QLabel*>("valueLabel");
    
    statsLayout->addWidget(exhibitCard, 0, 0);
    statsLayout->addWidget(collectionCard, 0, 1);
    statsLayout->addWidget(exhibitionCard, 0, 2);
    statsLayout->addWidget(employeeCard, 0, 3);
    statsLayout->addWidget(displayCard, 0, 4);
    statsLayout->addWidget(restorationCard, 0, 5);
    
    mainLayout->addLayout(statsLayout);
    
    // Горизонтальный layout для таблиц
    QHBoxLayout *tablesLayout = new QHBoxLayout();
    tablesLayout->setSpacing(15);
    
    // Последние экспонаты
    QGroupBox *recentGroup = new QGroupBox(tr("Последние добавленные экспонаты"));
    QVBoxLayout *recentLayout = new QVBoxLayout(recentGroup);
    m_recentExhibitsTable = new QTableView();
    m_recentExhibitsTable->setAlternatingRowColors(true);
    m_recentExhibitsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_recentExhibitsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentExhibitsTable->horizontalHeader()->setStretchLastSection(true);
    m_recentExhibitsTable->verticalHeader()->setVisible(false);
    recentLayout->addWidget(m_recentExhibitsTable);
    tablesLayout->addWidget(recentGroup);
    
    // Активные выставки
    QGroupBox *exhibitionsGroup = new QGroupBox(tr("Текущие выставки"));
    QVBoxLayout *exhibitionsLayout = new QVBoxLayout(exhibitionsGroup);
    m_activeExhibitionsTable = new QTableView();
    m_activeExhibitionsTable->setAlternatingRowColors(true);
    m_activeExhibitionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_activeExhibitionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_activeExhibitionsTable->horizontalHeader()->setStretchLastSection(true);
    m_activeExhibitionsTable->verticalHeader()->setVisible(false);
    exhibitionsLayout->addWidget(m_activeExhibitionsTable);
    tablesLayout->addWidget(exhibitionsGroup);
    
    // Реставрации в процессе
    QGroupBox *restorationGroup = new QGroupBox(tr("Реставрация в процессе"));
    QVBoxLayout *restorationLayout = new QVBoxLayout(restorationGroup);
    m_restorationTable = new QTableView();
    m_restorationTable->setAlternatingRowColors(true);
    m_restorationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_restorationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_restorationTable->horizontalHeader()->setStretchLastSection(true);
    m_restorationTable->verticalHeader()->setVisible(false);
    restorationLayout->addWidget(m_restorationTable);
    tablesLayout->addWidget(restorationGroup);
    
    mainLayout->addLayout(tablesLayout, 1);
}

QFrame* DashboardWidget::createStatCard(const QString& title, const QString& value, 
                                        const QString& color, const QString& icon)
{
    Q_UNUSED(icon)
    
    QFrame *card = new QFrame();
    card->setObjectName("statCard");
    card->setStyleSheet(QString(
        "QFrame#statCard {"
        "    background-color: %1;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "}"
    ).arg(color));
    card->setMinimumSize(150, 100);
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(5);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 12px;");
    layout->addWidget(titleLabel);
    
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("valueLabel");
    valueLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    layout->addWidget(valueLabel);
    
    layout->addStretch();
    
    return card;
}

void DashboardWidget::refresh()
{
    updateStatistics();
    loadRecentExhibits();
    loadActiveExhibitions();
    loadRestorationInProgress();
}

void DashboardWidget::updateStatistics()
{
    DatabaseManager& db = DatabaseManager::instance();
    
    m_exhibitCountLabel->setText(QString::number(db.getExhibitCount()));
    m_collectionCountLabel->setText(QString::number(db.getCollectionCount()));
    m_exhibitionCountLabel->setText(QString::number(db.getExhibitionCount()));
    m_employeeCountLabel->setText(QString::number(db.getEmployeeCount()));
    m_onDisplayLabel->setText(QString::number(db.getExhibitsOnDisplay()));
    m_inRestorationLabel->setText(QString::number(db.getExhibitsInRestoration()));
}

void DashboardWidget::loadRecentExhibits()
{
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(R"(
        SELECT inventory_number as 'Инв. номер', 
               name as 'Название', 
               DATE(created_at) as 'Дата добавления'
        FROM exhibits 
        ORDER BY created_at DESC 
        LIMIT 10
    )", DatabaseManager::instance().database());
    
    m_recentExhibitsTable->setModel(model);
    m_recentExhibitsTable->resizeColumnsToContents();
}

void DashboardWidget::loadActiveExhibitions()
{
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(R"(
        SELECT name as 'Название', 
               location as 'Место',
               start_date as 'Начало',
               end_date as 'Окончание'
        FROM exhibitions 
        WHERE status = 'Активна'
        ORDER BY start_date DESC
        LIMIT 10
    )", DatabaseManager::instance().database());
    
    m_activeExhibitionsTable->setModel(model);
    m_activeExhibitionsTable->resizeColumnsToContents();
}

void DashboardWidget::loadRestorationInProgress()
{
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(R"(
        SELECT e.name as 'Экспонат',
               rt.name as 'Тип работ',
               r.start_date as 'Начало',
               emp.last_name as 'Реставратор'
        FROM restorations r
        JOIN exhibits e ON r.exhibit_id = e.id
        LEFT JOIN restoration_types rt ON r.restoration_type_id = rt.id
        LEFT JOIN employees emp ON r.restorer_id = emp.id
        WHERE r.status = 'В процессе'
        ORDER BY r.start_date DESC
        LIMIT 10
    )", DatabaseManager::instance().database());
    
    m_restorationTable->setModel(model);
    m_restorationTable->resizeColumnsToContents();
}
