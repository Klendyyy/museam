#include "employeewidget.h"
#include "employeemodel.h"
#include "employeedialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QSqlQuery>

EmployeeWidget::EmployeeWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadFilters();
}

void EmployeeWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Заголовок
    QLabel *titleLabel = new QLabel(tr("Управление сотрудниками"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);
    
    // Фильтры
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    filterLayout->addWidget(new QLabel(tr("Должность:")));
    m_positionCombo = new QComboBox();
    m_positionCombo->setMinimumWidth(150);
    connect(m_positionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &EmployeeWidget::onPositionFilterChanged);
    filterLayout->addWidget(m_positionCombo);
    
    filterLayout->addSpacing(20);
    
    m_showInactiveCheck = new QCheckBox(tr("Показать уволенных"));
    connect(m_showInactiveCheck, &QCheckBox::stateChanged, this, &EmployeeWidget::onShowInactiveChanged);
    filterLayout->addWidget(m_showInactiveCheck);
    
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);
    
    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(100);
    connect(m_addButton, &QPushButton::clicked, this, &EmployeeWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);
    
    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(100);
    connect(m_editButton, &QPushButton::clicked, this, &EmployeeWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);
    
    m_deleteButton = new QPushButton(tr("Уволить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(100);
    connect(m_deleteButton, &QPushButton::clicked, this, &EmployeeWidget::onDeleteClicked);
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
    connect(m_tableView, &QTableView::doubleClicked, this, &EmployeeWidget::onTableDoubleClicked);
    
    m_model = new EmployeeModel(this);
    m_tableView->setModel(m_model);
    
    m_tableView->hideColumn(EmployeeModel::Id);
    m_tableView->hideColumn(EmployeeModel::CreatedAt);
    m_tableView->hideColumn(EmployeeModel::UpdatedAt);
    
    m_tableView->resizeColumnsToContents();
    mainLayout->addWidget(m_tableView, 1);
}

void EmployeeWidget::loadFilters()
{
    m_positionCombo->clear();
    m_positionCombo->addItem(tr("Все должности"), 0);
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec("SELECT id, name FROM positions ORDER BY name");
    while (query.next()) {
        m_positionCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void EmployeeWidget::refresh()
{
    m_model->refresh();
    loadFilters();
}

int EmployeeWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }
    
    return m_model->data(m_model->index(selected.first().row(), EmployeeModel::Id)).toInt();
}

void EmployeeWidget::onAddClicked()
{
    EmployeeDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление сотрудника"));
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addEmployee(dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Сотрудник успешно добавлен"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось добавить сотрудника"));
        }
    }
}

void EmployeeWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите сотрудника для редактирования"));
        return;
    }
    
    QVariantMap data = m_model->getEmployeeById(id);
    
    EmployeeDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование сотрудника"));
    dialog.setData(data);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateEmployee(id, dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Данные сотрудника обновлены"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось обновить данные"));
        }
    }
}

void EmployeeWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите сотрудника"));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение"),
        tr("Вы действительно хотите уволить выбранного сотрудника?"),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_model->deleteEmployee(id)) {
            QMessageBox::information(this, tr("Успех"), tr("Сотрудник уволен"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось уволить сотрудника"));
        }
    }
}

void EmployeeWidget::onPositionFilterChanged(int index)
{
    int positionId = m_positionCombo->itemData(index).toInt();
    m_model->setFilterByPosition(positionId);
}

void EmployeeWidget::onShowInactiveChanged(int state)
{
    m_model->setShowInactive(state == Qt::Checked);
}

void EmployeeWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onEditClicked();
}
