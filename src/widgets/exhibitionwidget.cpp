#include "exhibitionwidget.h"
#include "exhibitionmodel.h"
#include "exhibitiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

ExhibitionWidget::ExhibitionWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ExhibitionWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Заголовок
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("Управление выставками"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    headerLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Планирование и проведение выставок"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerWidget);

    // Фильтр по статусу
    QWidget *filterCard = new QWidget();
    filterCard->setStyleSheet("QWidget { background-color: #ffffff; border-radius: 12px; }");
    QGraphicsDropShadowEffect *filterShadow = new QGraphicsDropShadowEffect();
    filterShadow->setBlurRadius(15);
    filterShadow->setColor(QColor(0, 0, 0, 20));
    filterShadow->setOffset(0, 2);
    filterCard->setGraphicsEffect(filterShadow);

    QHBoxLayout *filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(20, 16, 20, 16);
    filterLayout->setSpacing(16);

    QLabel *statusLabel = new QLabel(tr("Статус:"));
    statusLabel->setStyleSheet("color: #718096; font-weight: 500;");
    filterLayout->addWidget(statusLabel);

    m_statusCombo = new QComboBox();
    m_statusCombo->setMinimumWidth(180);
    m_statusCombo->addItem(tr("Все"), "");
    m_statusCombo->addItems({"Планируется", "Активна", "Приостановлена", "Завершена", "Отменена"});
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ExhibitionWidget::onStatusFilterChanged);
    filterLayout->addWidget(m_statusCombo);
    filterLayout->addStretch();
    mainLayout->addWidget(filterCard);

    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_addButton = new QPushButton(tr("Добавить"));
    m_addButton->setMinimumWidth(130);
    m_addButton->setObjectName("successButton");
    connect(m_addButton, &QPushButton::clicked, this, &ExhibitionWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("Редактировать"));
    m_editButton->setMinimumWidth(140);
    connect(m_editButton, &QPushButton::clicked, this, &ExhibitionWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);

    m_manageExhibitsButton = new QPushButton(tr("Управление экспонатами"));
    m_manageExhibitsButton->setMinimumWidth(180);
    connect(m_manageExhibitsButton, &QPushButton::clicked, this, &ExhibitionWidget::onManageExhibitsClicked);
    buttonLayout->addWidget(m_manageExhibitsButton);

    m_deleteButton = new QPushButton(tr("Удалить"));
    m_deleteButton->setObjectName("deleteButton");
    m_deleteButton->setMinimumWidth(110);
    connect(m_deleteButton, &QPushButton::clicked, this, &ExhibitionWidget::onDeleteClicked);
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
    connect(m_tableView, &QTableView::doubleClicked, this, &ExhibitionWidget::onTableDoubleClicked);

    m_model = new ExhibitionModel(this);
    m_tableView->setModel(m_model);

    m_tableView->hideColumn(ExhibitionModel::Id);
    m_tableView->hideColumn(ExhibitionModel::Description);
    m_tableView->hideColumn(ExhibitionModel::CuratorId);

    m_tableView->resizeColumnsToContents();
    tableLayout->addWidget(m_tableView);

    mainLayout->addWidget(tableCard, 1);
}

void ExhibitionWidget::refresh()
{
    m_model->refresh();
    m_tableView->resizeColumnsToContents();
}

int ExhibitionWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }

    return m_model->data(m_model->index(selected.first().row(), ExhibitionModel::Id)).toInt();
}

void ExhibitionWidget::onAddClicked()
{
    ExhibitionDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление выставки"));

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addExhibition(dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Выставка успешно добавлена"));
            m_tableView->resizeColumnsToContents();
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось добавить выставку"));
        }
    }
}

void ExhibitionWidget::onEditClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите выставку для редактирования"));
        return;
    }

    QVariantMap data = m_model->getExhibitionById(id);

    ExhibitionDialog dialog(this);
    dialog.setWindowTitle(tr("Редактирование выставки"));
    dialog.setData(data);

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->updateExhibition(id, dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Выставка успешно обновлена"));
            m_tableView->resizeColumnsToContents();
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось обновить выставку"));
        }
    }
}

void ExhibitionWidget::onDeleteClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите выставку для удаления"));
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Подтверждение"),
        tr("Вы действительно хотите удалить выбранную выставку?"),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (m_model->deleteExhibition(id)) {
            QMessageBox::information(this, tr("Успех"), tr("Выставка успешно удалена"));
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось удалить выставку"));
        }
    }
}

void ExhibitionWidget::onManageExhibitsClicked()
{
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, tr("Внимание"), tr("Выберите выставку"));
        return;
    }

    QMessageBox::information(this, tr("Информация"),
                             tr("Функция управления экспонатами выставки будет доступна в следующей версии"));
}

void ExhibitionWidget::onStatusFilterChanged(int index)
{
    QString status = m_statusCombo->itemText(index);
    m_model->setFilterByStatus(status);
    m_tableView->resizeColumnsToContents();
}

void ExhibitionWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onEditClicked();
}
