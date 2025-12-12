// Файл: src/widgets/dashboardwidget.cpp
// Действие: ПОЛНОСТЬЮ ЗАМЕНИТЬ

#include "dashboardwidget.h"
#include "databasemanager.h"
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QScrollArea>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refresh();
}

void DashboardWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Заголовок
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("Панель управления"));
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    headerLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Обзор состояния музейных фондов"));
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerWidget);

    // Карточки статистики
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    QFrame *exhibitCard = createStatCard(tr("Экспонаты"), "0", "#3182ce", "🖼️");
    m_exhibitCountLabel = exhibitCard->findChild<QLabel*>("valueLabel");

    QFrame *collectionCard = createStatCard(tr("Коллекции"), "0", "#38a169", "📁");
    m_collectionCountLabel = collectionCard->findChild<QLabel*>("valueLabel");

    QFrame *exhibitionCard = createStatCard(tr("Активные выставки"), "0", "#805ad5", "🎭");
    m_exhibitionCountLabel = exhibitionCard->findChild<QLabel*>("valueLabel");

    QFrame *employeeCard = createStatCard(tr("Сотрудники"), "0", "#dd6b20", "👥");
    m_employeeCountLabel = employeeCard->findChild<QLabel*>("valueLabel");

    QFrame *displayCard = createStatCard(tr("На выставке"), "0", "#319795", "✨");
    m_onDisplayLabel = displayCard->findChild<QLabel*>("valueLabel");

    QFrame *restorationCard = createStatCard(tr("На реставрации"), "0", "#e53e3e", "🔧");
    m_inRestorationLabel = restorationCard->findChild<QLabel*>("valueLabel");

    statsLayout->addWidget(exhibitCard);
    statsLayout->addWidget(collectionCard);
    statsLayout->addWidget(exhibitionCard);
    statsLayout->addWidget(employeeCard);
    statsLayout->addWidget(displayCard);
    statsLayout->addWidget(restorationCard);

    mainLayout->addLayout(statsLayout);

    // Таблицы
    QHBoxLayout *tablesLayout = new QHBoxLayout();
    tablesLayout->setSpacing(16);

    // Последние экспонаты
    QWidget *recentWidget = createTableCard(tr("Последние добавленные экспонаты"), "🕐");
    QVBoxLayout *recentLayout = qobject_cast<QVBoxLayout*>(recentWidget->layout());
    m_recentExhibitsTable = new QTableView();
    setupTableStyle(m_recentExhibitsTable);
    recentLayout->addWidget(m_recentExhibitsTable);
    tablesLayout->addWidget(recentWidget);

    // Активные выставки
    QWidget *exhibitionsWidget = createTableCard(tr("Текущие выставки"), "🎭");
    QVBoxLayout *exhibitionsLayout = qobject_cast<QVBoxLayout*>(exhibitionsWidget->layout());
    m_activeExhibitionsTable = new QTableView();
    setupTableStyle(m_activeExhibitionsTable);
    exhibitionsLayout->addWidget(m_activeExhibitionsTable);
    tablesLayout->addWidget(exhibitionsWidget);

    // Реставрации
    QWidget *restorationWidget = createTableCard(tr("Реставрация в процессе"), "🔧");
    QVBoxLayout *restorationLayout = qobject_cast<QVBoxLayout*>(restorationWidget->layout());
    m_restorationTable = new QTableView();
    setupTableStyle(m_restorationTable);
    restorationLayout->addWidget(m_restorationTable);
    tablesLayout->addWidget(restorationWidget);

    mainLayout->addLayout(tablesLayout, 1);
}

void DashboardWidget::setupTableStyle(QTableView *table)
{
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setShowGrid(false);
    table->setFrameShape(QFrame::NoFrame);
}

QWidget* DashboardWidget::createTableCard(const QString& title, const QString& icon)
{
    QWidget *card = new QWidget();
    card->setStyleSheet(
        "QWidget { background-color: #ffffff; border-radius: 12px; }"
        );

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(16);

    QLabel *titleLabel = new QLabel(icon + " " + title);
    titleLabel->setStyleSheet(
        "font-size: 16px; font-weight: 600; color: #2d3748; padding-bottom: 8px;"
        "border-bottom: 2px solid #e2e8f0;"
        );
    layout->addWidget(titleLabel);

    // Добавляем тень
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    return card;
}

QFrame* DashboardWidget::createStatCard(const QString& title, const QString& value,
                                        const QString& color, const QString& icon)
{
    QFrame *card = new QFrame();
    card->setObjectName("statCard");
    card->setStyleSheet(QString(
                            "QFrame#statCard {"
                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                            "        stop:0 %1, stop:1 %2);"
                            "    border-radius: 12px;"
                            "    padding: 20px;"
                            "}"
                            ).arg(color).arg(QColor(color).darker(120).name()));
    card->setMinimumSize(180, 120);
    card->setMaximumHeight(140);

    // Тень для карточки
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(color).darker(150));
    shadow->setOffset(0, 8);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(8);
    layout->setContentsMargins(20, 16, 20, 16);

    // Иконка и заголовок
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet("font-size: 24px; background: transparent;");
    headerLayout->addWidget(iconLabel);

    headerLayout->addStretch();
    layout->addLayout(headerLayout);

    // Значение
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("valueLabel");
    valueLabel->setStyleSheet(
        "color: white; font-size: 36px; font-weight: 700; background: transparent;"
        );
    layout->addWidget(valueLabel);

    // Название
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "color: rgba(255,255,255,0.85); font-size: 13px; font-weight: 500; background: transparent;"
        );
    layout->addWidget(titleLabel);

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
               DATE_FORMAT(created_at, '%d.%m.%Y') as 'Добавлен'
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
               DATE_FORMAT(start_date, '%d.%m.%Y') as 'Начало',
               DATE_FORMAT(end_date, '%d.%m.%Y') as 'Окончание'
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
               DATE_FORMAT(r.start_date, '%d.%m.%Y') as 'Начало',
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
