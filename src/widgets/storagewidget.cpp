#include "storagewidget.h"
#include "storagemodel.h"
#include "storagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>

StorageWidget::StorageWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void StorageWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Заголовок
    QLabel *titleLabel = new QLabel(tr("Управление хранилищами"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);
    
    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(100);
    connect(m_addButton, &QPushButton::clicked, this, &StorageWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);
    
    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(100);
    connect(m_editButton, &QPushButton::clicked, this, &StorageWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);
    
    m_deleteButton = new QPushButton(tr("Удалить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(100);
    connect(m_deleteButton, &QPushButton::clicked, this, &StorageWidget::onDeleteClicked);
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
    connect(m_tableView, &QTableView::doubleClicked, this, &StorageWidget::onTableDoubleClicked);
    
    m_model = new StorageModel(this);
    m_tableView->setModel(m_model);
    
    m_tableView->hideColumn(StorageModel::Id);
    m_tableView->hideColumn(StorageModel::Description);
    m_tableView->hideColumn(StorageModel::IsActive);
    m_tableView->hideColumn(StorageModel::CreatedAt);
    
    m_tableView->resizeColumnsToContents();
    mainLayout->addWidget(m_tableView, 1);
}

void StorageWidget::refresh()
{
    m_model->refresh();
}

int StorageWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }
    
    return m_model->data(m_model->index(selected.first().row(), StorageModel::Id)).toInt();
}

void StorageWidget::onAddClicked()
{
    StorageDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление хранилища"));
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addStorage(dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Хранилище успешно добавлено"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось добавить хранилище"));
        }
    }
}

void StorageWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите хранилище для редактирования"));
        return;
    }
    
    QVariantMap data = m_model->getStorageById(id);
    
    StorageDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование хранилища"));
    dialog.setData(data);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateStorage(id, dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Хранилище успешно обновлено"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось обновить хранилище"));
        }
    }
}

void StorageWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите хранилище для удаления"));
        return;
    }
    
    int exhibitCount = m_model->getExhibitCount(id);
    if (exhibitCount > 0) {
        QMessageBox::warning(this, tr("Ошибка"), 
            tr("Невозможно удалить хранилище.\n"
               "В нём находится экспонатов: %1.\n"
               "Сначала переместите экспонаты в другое хранилище.").arg(exhibitCount));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение"),
        tr("Вы действительно хотите удалить выбранное хранилище?"),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_model->deleteStorage(id)) {
            QMessageBox::information(this, tr("Успех"), tr("Хранилище успешно удалено"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось удалить хранилище"));
        }
    }
}

void StorageWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onEditClicked();
}
