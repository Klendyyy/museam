#include "collectionwidget.h"
#include "collectionmodel.h"
#include "collectiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>

CollectionWidget::CollectionWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void CollectionWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Заголовок
    QLabel *titleLabel = new QLabel(tr("Управление коллекциями"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);
    
    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(100);
    connect(m_addButton, &QPushButton::clicked, this, &CollectionWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);
    
    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(100);
    connect(m_editButton, &QPushButton::clicked, this, &CollectionWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);
    
    m_deleteButton = new QPushButton(tr("Удалить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(100);
    connect(m_deleteButton, &QPushButton::clicked, this, &CollectionWidget::onDeleteClicked);
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
    connect(m_tableView, &QTableView::doubleClicked, this, &CollectionWidget::onTableDoubleClicked);
    
    m_model = new CollectionModel(this);
    m_tableView->setModel(m_model);
    
    m_tableView->hideColumn(CollectionModel::Id);
    m_tableView->hideColumn(CollectionModel::CreatedAt);
    m_tableView->hideColumn(CollectionModel::UpdatedAt);
    
    m_tableView->resizeColumnsToContents();
    mainLayout->addWidget(m_tableView, 1);
}

void CollectionWidget::refresh()
{
    m_model->refresh();
}

int CollectionWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }
    
    return m_model->data(m_model->index(selected.first().row(), CollectionModel::Id)).toInt();
}

void CollectionWidget::onAddClicked()
{
    CollectionDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление коллекции"));
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addCollection(dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Коллекция успешно добавлена"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось добавить коллекцию"));
        }
    }
}

void CollectionWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите коллекцию для редактирования"));
        return;
    }
    
    QVariantMap data = m_model->getCollectionById(id);
    
    CollectionDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование коллекции"));
    dialog.setData(data);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateCollection(id, dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Коллекция успешно обновлена"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось обновить коллекцию"));
        }
    }
}

void CollectionWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите коллекцию для удаления"));
        return;
    }
    
    int exhibitCount = m_model->getExhibitCount(id);
    QString message = tr("Вы действительно хотите удалить выбранную коллекцию?");
    if (exhibitCount > 0) {
        message += tr("\n\nВ коллекции находится экспонатов: %1.\n"
                     "Экспонаты не будут удалены, но связь с коллекцией будет потеряна.")
                     .arg(exhibitCount);
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение"), message,
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_model->deleteCollection(id)) {
            QMessageBox::information(this, tr("Успех"), tr("Коллекция успешно удалена"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось удалить коллекцию"));
        }
    }
}

void CollectionWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onEditClicked();
}
