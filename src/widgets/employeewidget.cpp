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
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

EmployeeWidget::EmployeeWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadFilters();
}

void EmployeeWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Заголовок
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(tr("Управление сотрудниками"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #1a202c;");
    headerLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Учёт персонала музея"));
    subtitleLabel->setStyleSheet("font-size: 14px; color: #718096;");
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerWidget);

    // Панель фильтров
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

    QLabel *positionLabel = new QLabel(tr("Должность:"));
    positionLabel->setStyleSheet("font-weight: 600; color: #4a5568;");
    filterLayout->addWidget(positionLabel);

    m_positionCombo = new QComboBox();
    m_positionCombo->setMinimumWidth(200);
    connect(m_positionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EmployeeWidget::onPositionFilterChanged);
    filterLayout->addWidget(m_positionCombo);

    filterLayout->addSpacing(30);

    m_showInactiveCheck = new QCheckBox(tr("Показать уволенных"));
    m_showInactiveCheck->setStyleSheet("font-weight: 500; color: #4a5568;");
    connect(m_showInactiveCheck, &QCheckBox::stateChanged, this, &EmployeeWidget::onShowInactiveChanged);
    filterLayout->addWidget(m_showInactiveCheck);

    filterLayout->addStretch();
    mainLayout->addWidget(filterCard);

    // Панель кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    m_addButton = new QPushButton(tr("➕ Добавить"));
    m_addButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #38a169, stop:1 #2f855a); color: white; border: none; "
        "border-radius: 6px; padding: 10px 20px; font-weight: 600; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #48bb78, stop:1 #38a169); }"
        );
    connect(m_addButton, &QPushButton::clicked, this, &EmployeeWidget::onAddClicked);
    buttonLayout->addWidget(m_addButton);

    m_editButton = new QPushButton(tr("✏️ Редактировать"));
    connect(m_editButton, &QPushButton::clicked, this, &EmployeeWidget::onEditClicked);
    buttonLayout->addWidget(m_editButton);

    m_deleteButton = new QPushButton(tr("🚫 Уволить"));
    m_deleteButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #e53e3e, stop:1 #c53030); color: white; border: none; "
        "border-radius: 6px; padding: 10px 20px; font-weight: 600; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #fc8181, stop:1 #e53e3e); }"
        );
    connect(m_deleteButton, &QPushButton::clicked, this, &EmployeeWidget::onDeleteClicked);
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
    connect(m_tableView, &QTableView::doubleClicked, this, &EmployeeWidget::onTableDoubleClicked);

    m_model = new EmployeeModel(this);
    m_tableView->setModel(m_model);

    // Скрываем колонку ID
    m_tableView->hideColumn(0);

    m_tableView->resizeColumnsToContents();
    tableLayout->addWidget(m_tableView);

    mainLayout->addWidget(tableCard, 1);
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
    m_tableView->resizeColumnsToContents();
}

int EmployeeWidget::getSelectedId()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }

    return m_model->data(m_model->index(selected.first().row(), 0)).toInt();
}

void EmployeeWidget::onAddClicked()
{
    EmployeeDialog dialog(this);
    dialog.setWindowTitle(tr("Добавление сотрудника"));

    if (dialog.exec() == QDialog::Accepted) {
        if (m_model->addEmployee(dialog.getData())) {
            QMessageBox::information(this, tr("Успех"), tr("Сотрудник успешно добавлен"));
            m_tableView->resizeColumnsToContents();
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
            m_tableView->resizeColumnsToContents();
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

    // Проверяем, показываем ли мы уволенных сотрудников
    if (m_model->isShowingInactive()) {
        // Восстановление сотрудника
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("Подтверждение"),
            tr("Вы действительно хотите восстановить выбранного сотрудника?"),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            if (m_model->reinstateEmployee(id)) {
                QMessageBox::information(this, tr("Успех"), tr("Сотрудник восстановлен"));
                m_tableView->resizeColumnsToContents();
            } else {
                QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось восстановить сотрудника"));
            }
        }
    } else {
        // Увольнение сотрудника
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("Подтверждение"),
            tr("Вы действительно хотите уволить выбранного сотрудника?"),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            if (m_model->deleteEmployee(id)) {
                QMessageBox::information(this, tr("Успех"), tr("Сотрудник уволен"));
                m_tableView->resizeColumnsToContents();
            } else {
                QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось уволить сотрудника"));
            }
        }
    }
}

void EmployeeWidget::onPositionFilterChanged(int index)
{
    int positionId = m_positionCombo->itemData(index).toInt();
    m_model->setFilterByPosition(positionId);
    m_tableView->resizeColumnsToContents();
}

void EmployeeWidget::onShowInactiveChanged(int state)
{
    bool showInactive = (state == Qt::Checked);
    m_model->setShowInactive(showInactive);

    // Меняем текст и стиль кнопки в зависимости от режима
    if (showInactive) {
        m_deleteButton->setText(tr("✅ Восстановить"));
        m_deleteButton->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "stop:0 #38a169, stop:1 #2f855a); color: white; border: none; "
            "border-radius: 6px; padding: 10px 20px; font-weight: 600; }"
            "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "stop:0 #48bb78, stop:1 #38a169); }"
            );
        m_addButton->setEnabled(false);  // Нельзя добавлять в режиме просмотра уволенных
    } else {
        m_deleteButton->setText(tr("🚫 Уволить"));
        m_deleteButton->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "stop:0 #e53e3e, stop:1 #c53030); color: white; border: none; "
            "border-radius: 6px; padding: 10px 20px; font-weight: 600; }"
            "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "stop:0 #fc8181, stop:1 #e53e3e); }"
            );
        m_addButton->setEnabled(true);
    }

    m_tableView->resizeColumnsToContents();
}

void EmployeeWidget::onTableDoubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    onEditClicked();
}
