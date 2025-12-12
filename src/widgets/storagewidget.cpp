#include "storagewidget.h"
#include "storagemodel.h"
#include "storagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

StorageWidget::StorageWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void StorageWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Заголовок
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("Управление хранилищами"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    headerLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Места хранения музейных экспонатов"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerWidget);

    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(130);
    m_addButton->setObjectName("successButton");
    connect(m_addButton, &QPushButton::clicked, this, &StorageWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(140);
    connect(m_editButton, &QPushButton::clicked, this, &StorageWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);

    m_deleteButton = new QPushButton(tr("Удалить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(110);
    connect(m_deleteButton, &QPushButton::clicked, this, &StorageWidget::onDeleteClicked);
    buttonLayout->addWidget(m_deleteButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Таблица
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
    connect(m_tableView, &QTableView::doubleClicked, this, &StorageWidget::onTableDoubleClicked);

    m_model = new StorageModel(this);
    m_tableView->setModel(m_model);

    m_tableView->hideColumn(StorageModel::Id);
    m_tableView->hideColumn(StorageModel::ResponsibleEmployeeId);
    m_tableView->hideColumn(StorageModel::Description);
    m_tableView->hideColumn(StorageModel::IsActive);

    m_tableView->resizeColumnsToContents();
    tableLayout->addWidget(m_tableView);

    mainLayout->addWidget(tableCard, 1);
}

void StorageWidget::refresh()
{
    m_model->refresh();
    m_tableView->resizeColumnsToContents();
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
            m_tableView->resizeColumnsToContents();
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
            m_tableView->resizeColumnsToContents();
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
