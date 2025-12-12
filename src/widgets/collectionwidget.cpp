#include "collectionwidget.h"
#include "collectionmodel.h"
#include "collectiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

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

    // Заголовок
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("Управление коллекциями"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    headerLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Организация музейных коллекций"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerWidget);

    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(130);
    m_addButton->setObjectName("successButton");
    connect(m_addButton, &QPushButton::clicked, this, &CollectionWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(140);
    connect(m_editButton, &QPushButton::clicked, this, &CollectionWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);

    m_deleteButton = new QPushButton(tr("Удалить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(110);
    connect(m_deleteButton, &QPushButton::clicked, this, &CollectionWidget::onDeleteClicked);
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
    connect(m_tableView, &QTableView::doubleClicked, this, &CollectionWidget::onTableDoubleClicked);

    m_model = new CollectionModel(this);
    m_tableView->setModel(m_model);

    m_tableView->hideColumn(CollectionModel::Id);
    m_tableView->hideColumn(CollectionModel::CuratorId);
    m_tableView->hideColumn(CollectionModel::IsActive);

    m_tableView->resizeColumnsToContents();
    tableLayout->addWidget(m_tableView);

    mainLayout->addWidget(tableCard, 1);
}

void CollectionWidget::refresh()
{
    m_model->refresh();
    m_tableView->resizeColumnsToContents();
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
            m_tableView->resizeColumnsToContents();
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
            m_tableView->resizeColumnsToContents();
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
