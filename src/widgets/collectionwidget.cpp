#include "collectionwidget.h"
#include "collectionmodel.h"
#include "collectiondialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QSqlQuery>

CollectionWidget::CollectionWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void CollectionWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // ===== ЗАГОЛОВОК =====
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *titleWidget = new QWidget();
    QVBoxLayout *titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("📁 Управление коллекциями"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    titleLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Организация и систематизация музейных фондов"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    titleLayout->addWidget(subtitleLabel);

    headerLayout->addWidget(titleWidget);
    headerLayout->addStretch();

    // Счётчик коллекций
    m_countLabel = new QLabel(tr("Всего: 0"));
    m_countLabel->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #38a169, stop:1 #2f855a);"
        "color: white; padding: 8px 16px; border-radius: 20px; font-weight: 600;"
        );
    headerLayout->addWidget(m_countLabel);

    mainLayout->addWidget(headerWidget);

    // ===== СТАТИСТИКА КОЛЛЕКЦИЙ =====
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setSpacing(16);
    statsLayout->setContentsMargins(0, 0, 0, 0);

    // Карточка: Всего экспонатов в коллекциях
    QFrame *totalCard = createMiniStatCard(tr("Экспонатов в коллекциях"), "0", "#3182ce");
    m_totalExhibitsLabel = totalCard->findChild<QLabel*>("valueLabel");
    statsLayout->addWidget(totalCard);

    // Карточка: Постоянные коллекции
    QFrame *permanentCard = createMiniStatCard(tr("Постоянные"), "0", "#38a169");
    m_permanentLabel = permanentCard->findChild<QLabel*>("valueLabel");
    statsLayout->addWidget(permanentCard);

    // Карточка: Временные коллекции
    QFrame *tempCard = createMiniStatCard(tr("Временные"), "0", "#805ad5");
    m_temporaryLabel = tempCard->findChild<QLabel*>("valueLabel");
    statsLayout->addWidget(tempCard);

    statsLayout->addStretch();
    mainLayout->addWidget(statsWidget);

    // ===== ПАНЕЛЬ КНОПОК =====
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_addButton = new QPushButton(tr("➕ Создать коллекцию"));
    m_addButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #48bb78, stop:1 #38a169); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #68d391, stop:1 #48bb78); }"
        );
    connect(m_addButton, &QPushButton::clicked, this, &CollectionWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("✏️ Редактировать"));
    m_editButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #4299e1, stop:1 #3182ce); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #63b3ed, stop:1 #4299e1); }"
        );
    connect(m_editButton, &QPushButton::clicked, this, &CollectionWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);

    m_deleteButton = new QPushButton(tr("🗑️ Удалить"));
    m_deleteButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #fc8181, stop:1 #f56565); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #feb2b2, stop:1 #fc8181); }"
        );
    connect(m_deleteButton, &QPushButton::clicked, this, &CollectionWidget::onDeleteClicked);
    buttonLayout->addWidget(m_deleteButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // ===== ТАБЛИЦА =====
    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet("QWidget { background-color: #ffffff; border-radius: 12px; }");
    QGraphicsDropShadowEffect *tableShadow = new QGraphicsDropShadowEffect();
    tableShadow->setBlurRadius(20);
    tableShadow->setColor(QColor(0, 0, 0, 25));
    tableShadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(tableShadow);

    QVBoxLayout *tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(0, 0, 0, 0);

    m_tableView = new QTableView();
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setShowGrid(false);
    m_tableView->setFrameShape(QFrame::NoFrame);
    m_tableView->setStyleSheet(
        "QTableView { background-color: #ffffff; border: none; }"
        "QTableView::item { padding: 12px 8px; border-bottom: 1px solid #f1f5f9; }"
        "QTableView::item:selected { background-color: #38a169; color: white; }"
        "QTableView::item:hover { background-color: #f0fff4; }"
        );
    connect(m_tableView, &QTableView::doubleClicked, this, &CollectionWidget::onTableDoubleClicked);

    m_model = new CollectionModel(this);
    m_tableView->setModel(m_model);

    m_tableView->hideColumn(0);  // Скрываем ID
    m_tableView->resizeColumnsToContents();
    tableLayout->addWidget(m_tableView);

    mainLayout->addWidget(tableCard, 1);
}

QFrame* CollectionWidget::createMiniStatCard(const QString& title, const QString& value, const QString& color)
{
    QFrame *card = new QFrame();
    card->setStyleSheet(QString(
                            "QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                            "stop:0 %1, stop:1 %2); border-radius: 10px; }"
                            ).arg(color).arg(QColor(color).darker(115).name()));
    card->setMinimumSize(180, 80);
    card->setMaximumHeight(90);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 3);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(4);
    layout->setContentsMargins(16, 12, 16, 12);

    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("valueLabel");
    valueLabel->setStyleSheet("color: white; font-size: 24px; font-weight: 700; background: transparent;");
    layout->addWidget(valueLabel);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: rgba(255,255,255,0.9); font-size: 11px; font-weight: 500; background: transparent;");
    layout->addWidget(titleLabel);

    return card;
}

void CollectionWidget::refresh()
{
    m_model->refresh();
    m_tableView->resizeColumnsToContents();
    updateStatistics();
}

void CollectionWidget::updateStatistics()
{
    DatabaseManager& db = DatabaseManager::instance();

    // Количество коллекций
    int count = m_model->rowCount();
    m_countLabel->setText(tr("Всего: %1").arg(count));

    // Экспонатов в коллекциях
    QSqlQuery query(db.database());
    query.exec("SELECT COUNT(*) FROM exhibits WHERE collection_id IS NOT NULL");
    if (query.next() && m_totalExhibitsLabel) {
        m_totalExhibitsLabel->setText(query.value(0).toString());
    }

    // Постоянные коллекции
    query.exec("SELECT COUNT(*) FROM collections WHERE is_permanent = 1 AND is_active = 1");
    if (query.next() && m_permanentLabel) {
        m_permanentLabel->setText(query.value(0).toString());
    }

    // Временные коллекции
    query.exec("SELECT COUNT(*) FROM collections WHERE is_permanent = 0 AND is_active = 1");
    if (query.next() && m_temporaryLabel) {
        m_temporaryLabel->setText(query.value(0).toString());
    }
}

int CollectionWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }

    return m_model->data(m_model->index(selected.first().row(), 0)).toInt();
}

void CollectionWidget::onAddClicked()
{
    CollectionDialog dialog(this);
    dialog.setWindowTitle(tr("Создание новой коллекции"));

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addCollection(dialog.getData())) {
            QMessageBox::information(this, tr("✅ Успех"), tr("Коллекция успешно создана"));
            m_tableView->resizeColumnsToContents();
            updateStatistics();
        } else {
            QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось создать коллекцию"));
        }
    }
}

void CollectionWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("⚠️ Внимание"), tr("Выберите коллекцию для редактирования"));
        return;
    }

    QVariantMap data = m_model->getCollectionById(id);

    CollectionDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование коллекции"));
    dialog.setData(data);

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateCollection(id, dialog.getData())) {
            QMessageBox::information(this, tr("✅ Успех"), tr("Коллекция обновлена"));
            m_tableView->resizeColumnsToContents();
            updateStatistics();
        } else {
            QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось обновить коллекцию"));
        }
    }
}

void CollectionWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("⚠️ Внимание"), tr("Выберите коллекцию для удаления"));
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение удаления"),
        tr("Вы действительно хотите удалить выбранную коллекцию?\n\n"
           "Экспонаты из этой коллекции не будут удалены, "
           "но потеряют привязку к коллекции."),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (m_model->deleteCollection(id)) {
            QMessageBox::information(this, tr("✅ Успех"), tr("Коллекция удалена"));
            updateStatistics();
        } else {
            QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось удалить коллекцию"));
        }
    }
}

void CollectionWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onEditClicked();
}
