#include "exhibitwidget.h"
#include "exhibitmodel.h"
#include "exhibitdialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QSqlQuery>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

ExhibitWidget::ExhibitWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadFilters();
}

void ExhibitWidget::setupUi()
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

    QLabel *titleLabel = new QLabel(tr("🖼️ Каталог экспонатов"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    titleLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Управление музейными экспонатами"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    titleLayout->addWidget(subtitleLabel);

    headerLayout->addWidget(titleWidget);
    headerLayout->addStretch();

    // Счётчик записей
    m_countLabel = new QLabel(tr("Всего: 0"));
    m_countLabel->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "color: white; padding: 8px 16px; border-radius: 20px; font-weight: 600;"
        );
    headerLayout->addWidget(m_countLabel);

    mainLayout->addWidget(headerWidget);

    // ===== ПАНЕЛЬ ПОИСКА И ФИЛЬТРОВ =====
    QWidget *filterCard = new QWidget();
    filterCard->setStyleSheet("QWidget { background-color: #ffffff; border-radius: 12px; }");
    QGraphicsDropShadowEffect *filterShadow = new QGraphicsDropShadowEffect();
    filterShadow->setBlurRadius(15);
    filterShadow->setColor(QColor(0, 0, 0, 20));
    filterShadow->setOffset(0, 2);
    filterCard->setGraphicsEffect(filterShadow);

    QHBoxLayout *filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(20, 16, 20, 16);
    filterLayout->setSpacing(20);

    // Поиск
    QLabel *searchIcon = new QLabel(tr("🔍"));
    searchIcon->setStyleSheet("font-size: 18px;");
    filterLayout->addWidget(searchIcon);

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(tr("Поиск по названию, инв. номеру или автору..."));
    m_searchEdit->setMinimumWidth(300);
    m_searchEdit->setStyleSheet(
        "QLineEdit { border: 2px solid #e2e8f0; border-radius: 8px; padding: 10px 15px; "
        "font-size: 10pt; background: #f8fafc; }"
        "QLineEdit:focus { border-color: #667eea; background: white; }"
        );
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ExhibitWidget::onSearchTextChanged);
    filterLayout->addWidget(m_searchEdit);

    // Разделитель
    QFrame *sep1 = new QFrame();
    sep1->setFrameShape(QFrame::VLine);
    sep1->setStyleSheet("background-color: #e2e8f0;");
    filterLayout->addWidget(sep1);

    // Категория
    QLabel *categoryLabel = new QLabel(tr("Категория:"));
    categoryLabel->setStyleSheet("font-weight: 600; color: #4a5568;");
    filterLayout->addWidget(categoryLabel);

    m_categoryCombo = new QComboBox();
    m_categoryCombo->setMinimumWidth(180);
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ExhibitWidget::onCategoryFilterChanged);
    filterLayout->addWidget(m_categoryCombo);

    // Статус
    QLabel *statusLabel = new QLabel(tr("Статус:"));
    statusLabel->setStyleSheet("font-weight: 600; color: #4a5568;");
    filterLayout->addWidget(statusLabel);

    m_statusCombo = new QComboBox();
    m_statusCombo->setMinimumWidth(150);
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ExhibitWidget::onStatusFilterChanged);
    filterLayout->addWidget(m_statusCombo);

    filterLayout->addStretch();
    mainLayout->addWidget(filterCard);

    // ===== ПАНЕЛЬ КНОПОК =====
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_addButton = new QPushButton(tr("➕ Добавить"));
    m_addButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #48bb78, stop:1 #38a169); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #68d391, stop:1 #48bb78); }"
        );
    connect(m_addButton, &QPushButton::clicked, this, &ExhibitWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("✏️ Редактировать"));
    m_editButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #4299e1, stop:1 #3182ce); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #63b3ed, stop:1 #4299e1); }"
        );
    connect(m_editButton, &QPushButton::clicked, this, &ExhibitWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);

    m_viewButton = new QPushButton(tr("👁️ Просмотр"));
    m_viewButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #9f7aea, stop:1 #805ad5); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #b794f4, stop:1 #9f7aea); }"
        );
    connect(m_viewButton, &QPushButton::clicked, this, &ExhibitWidget::onViewClicked);
    buttonLayout->addWidget(m_viewButton);

    m_deleteButton = new QPushButton(tr("🗑️ Удалить"));
    m_deleteButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #fc8181, stop:1 #f56565); color: white; border: none; "
        "border-radius: 8px; padding: 12px 24px; font-weight: 600; font-size: 10pt; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #feb2b2, stop:1 #fc8181); }"
        );
    connect(m_deleteButton, &QPushButton::clicked, this, &ExhibitWidget::onDeleteClicked);
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
        "QTableView::item:selected { background-color: #667eea; color: white; }"
        "QTableView::item:hover { background-color: #f0f4ff; }"
        );
    connect(m_tableView, &QTableView::doubleClicked, this, &ExhibitWidget::onTableDoubleClicked);

    m_model = new ExhibitModel(this);
    m_tableView->setModel(m_model);

    // Скрываем колонку ID
    m_tableView->hideColumn(0);

    m_tableView->resizeColumnsToContents();
    tableLayout->addWidget(m_tableView);

    mainLayout->addWidget(tableCard, 1);
}

void ExhibitWidget::loadFilters()
{
    // Загрузка категорий
    m_categoryCombo->clear();
    m_categoryCombo->addItem(tr("Все категории"), 0);

    QSqlQuery query(DatabaseManager::instance().database());
    query.exec("SELECT id, name FROM categories ORDER BY name");
    while (query.next()) {
        m_categoryCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }

    // Загрузка статусов
    m_statusCombo->clear();
    m_statusCombo->addItem(tr("Все статусы"), "");
    m_statusCombo->addItem(tr("🏠 В хранении"), "В хранении");
    m_statusCombo->addItem(tr("🎭 На выставке"), "На выставке");
    m_statusCombo->addItem(tr("🔧 На реставрации"), "На реставрации");
    m_statusCombo->addItem(tr("📦 Во временном хранении"), "Во временном хранении");
    m_statusCombo->addItem(tr("🔬 На экспертизе"), "На экспертизе");
}

void ExhibitWidget::refresh()
{
    m_model->refresh();
    loadFilters();
    m_tableView->resizeColumnsToContents();
    updateCount();
}

void ExhibitWidget::updateCount()
{
    int count = m_model->rowCount();
    m_countLabel->setText(tr("Всего: %1").arg(count));
}

int ExhibitWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }

    return m_model->data(m_model->index(selected.first().row(), 0)).toInt();
}

void ExhibitWidget::onAddClicked()
{
    ExhibitDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление экспоната"));

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addExhibit(dialog.getData())) {
            QMessageBox::information(this, tr("✅ Успех"), tr("Экспонат успешно добавлен в каталог"));
            m_tableView->resizeColumnsToContents();
            updateCount();
        } else {
            QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось добавить экспонат"));
        }
    }
}

void ExhibitWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("⚠️ Внимание"), tr("Выберите экспонат для редактирования"));
        return;
    }

    QVariantMap data = m_model->getExhibitById(id);

    ExhibitDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование экспоната"));
    dialog.setData(data);

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateExhibit(id, dialog.getData())) {
            QMessageBox::information(this, tr("✅ Успех"), tr("Данные экспоната обновлены"));
            m_tableView->resizeColumnsToContents();
        } else {
            QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось обновить данные"));
        }
    }
}

void ExhibitWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("⚠️ Внимание"), tr("Выберите экспонат для удаления"));
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение удаления"),
        tr("Вы действительно хотите удалить выбранный экспонат?\n\n"
           "Это действие нельзя отменить."),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (m_model->deleteExhibit(id)) {
            QMessageBox::information(this, tr("✅ Успех"), tr("Экспонат удалён из каталога"));
            updateCount();
        } else {
            QMessageBox::warning(this, tr("❌ Ошибка"), tr("Не удалось удалить экспонат"));
        }
    }
}

void ExhibitWidget::onViewClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("⚠️ Внимание"), tr("Выберите экспонат для просмотра"));
        return;
    }

    QVariantMap data = m_model->getExhibitById(id);

    ExhibitDialog dialog(this);
    dialog.setWindowTitle(tr("Просмотр экспоната"));
    dialog.setData(data);
    dialog.setReadOnly(true);
    dialog.exec();
}

void ExhibitWidget::onSearchTextChanged(const QString& text)
{
    m_model->setSearchFilter(text);
    m_tableView->resizeColumnsToContents();
    updateCount();
}

void ExhibitWidget::onCategoryFilterChanged(int index)
{
    int categoryId = m_categoryCombo->itemData(index).toInt();
    m_model->setFilterByCategory(categoryId);
    m_tableView->resizeColumnsToContents();
    updateCount();
}

void ExhibitWidget::onStatusFilterChanged(int index)
{
    QString status = m_statusCombo->itemData(index).toString();
    m_model->setFilterByStatus(status);
    m_tableView->resizeColumnsToContents();
    updateCount();
}

void ExhibitWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onViewClicked();
}
