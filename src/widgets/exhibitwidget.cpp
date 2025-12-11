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

ExhibitWidget::ExhibitWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadFilters();
}

void ExhibitWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Заголовок
    QLabel *titleLabel = new QLabel(tr("Управление экспонатами"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);
    
    // Панель фильтров
    QGroupBox *filterGroup = new QGroupBox(tr("Поиск и фильтры"));
    QHBoxLayout *filterLayout = new QHBoxLayout(filterGroup);
    
    filterLayout->addWidget(new QLabel(tr("Поиск:")));
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(tr("Введите название, инв. номер или автора..."));
    m_searchEdit->setMinimumWidth(250);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ExhibitWidget::onSearchTextChanged);
    filterLayout->addWidget(m_searchEdit);
    
    filterLayout->addSpacing(20);
    
    filterLayout->addWidget(new QLabel(tr("Категория:")));
    m_categoryCombo = new QComboBox();
    m_categoryCombo->setMinimumWidth(150);
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ExhibitWidget::onCategoryFilterChanged);
    filterLayout->addWidget(m_categoryCombo);
    
    filterLayout->addSpacing(20);
    
    filterLayout->addWidget(new QLabel(tr("Статус:")));
    m_statusCombo = new QComboBox();
    m_statusCombo->setMinimumWidth(150);
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ExhibitWidget::onStatusFilterChanged);
    filterLayout->addWidget(m_statusCombo);
    
    filterLayout->addStretch();
    mainLayout->addWidget(filterGroup);
    
    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(100);
    connect(m_addButton, &QPushButton::clicked, this, &ExhibitWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);
    
    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(100);
    connect(m_editButton, &QPushButton::clicked, this, &ExhibitWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);
    
    m_viewButton = new QPushButton(tr("Просмотр"));
    m_viewButton->setMinimumWidth(100);
    connect(m_viewButton, &QPushButton::clicked, this, &ExhibitWidget::onViewClicked);
    buttonLayout->addWidget(m_viewButton);
    
    m_deleteButton = new QPushButton(tr("Удалить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(100);
    connect(m_deleteButton, &QPushButton::clicked, this, &ExhibitWidget::onDeleteClicked);
    buttonLayout->addWidget(m_deleteButton);
    
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // Таблица
    m_tableView = new QTableView();
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    connect(m_tableView, &QTableView::doubleClicked, this, &ExhibitWidget::onTableDoubleClicked);
    
    m_model = new ExhibitModel(this);
    m_tableView->setModel(m_model);
    
    // Скрываем ненужные колонки
    m_tableView->hideColumn(ExhibitModel::Id);
    m_tableView->hideColumn(ExhibitModel::Description);
    m_tableView->hideColumn(ExhibitModel::CreationPlace);
    m_tableView->hideColumn(ExhibitModel::Weight);
    m_tableView->hideColumn(ExhibitModel::InsuranceValue);
    m_tableView->hideColumn(ExhibitModel::IsOnDisplay);
    m_tableView->hideColumn(ExhibitModel::RequiresRestoration);
    m_tableView->hideColumn(ExhibitModel::Notes);
    m_tableView->hideColumn(ExhibitModel::CreatedAt);
    m_tableView->hideColumn(ExhibitModel::UpdatedAt);
    
    m_tableView->resizeColumnsToContents();
    mainLayout->addWidget(m_tableView, 1);
}

void ExhibitWidget::loadFilters()
{
    // Загружаем категории
    m_categoryCombo->clear();
    m_categoryCombo->addItem(tr("Все категории"), 0);
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec("SELECT id, name FROM categories ORDER BY name");
    while (query.next()) {
        m_categoryCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Загружаем статусы
    m_statusCombo->clear();
    m_statusCombo->addItem(tr("Все статусы"), "");
    for (const QString& status : m_model->getStatusList()) {
        m_statusCombo->addItem(status, status);
    }
}

void ExhibitWidget::refresh()
{
    m_model->refresh();
    loadFilters();
}

int ExhibitWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }
    
    return m_model->data(m_model->index(selected.first().row(), ExhibitModel::Id)).toInt();
}

void ExhibitWidget::onAddClicked()
{
    ExhibitDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление экспоната"));
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addExhibit(dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Экспонат успешно добавлен"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось добавить экспонат"));
        }
    }
}

void ExhibitWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите экспонат для редактирования"));
        return;
    }
    
    QVariantMap data = m_model->getExhibitById(id);
    
    ExhibitDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование экспоната"));
    dialog.setData(data);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateExhibit(id, dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Экспонат успешно обновлён"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось обновить экспонат"));
        }
    }
}

void ExhibitWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите экспонат для удаления"));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение"),
        tr("Вы действительно хотите удалить выбранный экспонат?\n"
           "Это действие нельзя отменить."),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_model->deleteExhibit(id)) {
            QMessageBox::information(this, tr("Успех"), tr("Экспонат успешно удалён"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось удалить экспонат"));
        }
    }
}

void ExhibitWidget::onViewClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите экспонат для просмотра"));
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
}

void ExhibitWidget::onCategoryFilterChanged(int index)
{
    int categoryId = m_categoryCombo->itemData(index).toInt();
    m_model->setFilterByCategory(categoryId);
}

void ExhibitWidget::onStatusFilterChanged(int index)
{
    QString status = m_statusCombo->itemData(index).toString();
    m_model->setFilterByStatus(status);
}

void ExhibitWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onViewClicked();
}
