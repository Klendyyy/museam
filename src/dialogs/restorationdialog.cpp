#include "restorationdialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

RestorationDialog::RestorationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(600, 650);
}

void RestorationDialog::setupUi()
{
    setWindowTitle(tr("Реставрация"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Основная информация
    QGroupBox *basicGroup = new QGroupBox(tr("Основная информация"));
    QFormLayout *basicForm = new QFormLayout(basicGroup);
    basicForm->setSpacing(8);
    
    m_exhibitCombo = new QComboBox();
    basicForm->addRow(tr("Экспонат*:"), m_exhibitCombo);
    
    m_typeCombo = new QComboBox();
    basicForm->addRow(tr("Тип работ:"), m_typeCombo);
    
    m_restorerCombo = new QComboBox();
    basicForm->addRow(tr("Реставратор:"), m_restorerCombo);
    
    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({"В процессе", "Приостановлена", "Завершена", "Отменена"});
    basicForm->addRow(tr("Статус:"), m_statusCombo);
    
    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDate(QDate::currentDate());
    m_startDateEdit->setDisplayFormat("dd.MM.yyyy");
    basicForm->addRow(tr("Дата начала*:"), m_startDateEdit);
    
    m_endDateEdit = new QDateEdit();
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_endDateEdit->setSpecialValueText(tr("Не завершена"));
    basicForm->addRow(tr("Дата окончания:"), m_endDateEdit);
    
    m_costSpin = new QDoubleSpinBox();
    m_costSpin->setRange(0, 999999999);
    m_costSpin->setDecimals(2);
    m_costSpin->setSuffix(tr(" ₽"));
    basicForm->addRow(tr("Стоимость:"), m_costSpin);
    
    mainLayout->addWidget(basicGroup);
    
    // Состояние
    QGroupBox *conditionGroup = new QGroupBox(tr("Состояние экспоната"));
    QVBoxLayout *conditionLayout = new QVBoxLayout(conditionGroup);
    
    conditionLayout->addWidget(new QLabel(tr("До реставрации:")));
    m_conditionBeforeEdit = new QTextEdit();
    m_conditionBeforeEdit->setMaximumHeight(50);
    conditionLayout->addWidget(m_conditionBeforeEdit);
    
    conditionLayout->addWidget(new QLabel(tr("После реставрации:")));
    m_conditionAfterEdit = new QTextEdit();
    m_conditionAfterEdit->setMaximumHeight(50);
    conditionLayout->addWidget(m_conditionAfterEdit);
    
    mainLayout->addWidget(conditionGroup);
    
    // Описание работ
    QGroupBox *workGroup = new QGroupBox(tr("Описание работ"));
    QVBoxLayout *workLayout = new QVBoxLayout(workGroup);
    
    m_workDescriptionEdit = new QTextEdit();
    m_workDescriptionEdit->setMaximumHeight(60);
    m_workDescriptionEdit->setPlaceholderText(tr("Опишите выполненные работы..."));
    workLayout->addWidget(m_workDescriptionEdit);
    
    workLayout->addWidget(new QLabel(tr("Использованные материалы:")));
    m_materialsEdit = new QTextEdit();
    m_materialsEdit->setMaximumHeight(40);
    workLayout->addWidget(m_materialsEdit);
    
    mainLayout->addWidget(workGroup);
    
    // Кнопки
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_exhibitCombo->currentData().isNull()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Выберите экспонат"));
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void RestorationDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    // Экспонаты
    m_exhibitCombo->clear();
    m_exhibitCombo->addItem(tr("-- Выберите экспонат --"), QVariant());
    query.exec("SELECT id, inventory_number || ' - ' || name FROM exhibits ORDER BY inventory_number");
    while (query.next()) {
        m_exhibitCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Типы реставрации
    m_typeCombo->clear();
    m_typeCombo->addItem(tr("-- Не указан --"), QVariant());
    query.exec("SELECT id, name FROM restoration_types ORDER BY name");
    while (query.next()) {
        m_typeCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Реставраторы
    m_restorerCombo->clear();
    m_restorerCombo->addItem(tr("-- Не назначен --"), QVariant());
    query.exec(R"(
        SELECT e.id, e.last_name || ' ' || e.first_name 
        FROM employees e
        JOIN positions p ON e.position_id = p.id
        WHERE e.is_active = 1
        ORDER BY e.last_name
    )");
    while (query.next()) {
        m_restorerCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void RestorationDialog::setExhibitId(int exhibitId)
{
    int idx = m_exhibitCombo->findData(exhibitId);
    if (idx >= 0) {
        m_exhibitCombo->setCurrentIndex(idx);
        m_exhibitCombo->setEnabled(false);
    }
}

void RestorationDialog::setData(const QVariantMap& data)
{
    int exhibitIdx = m_exhibitCombo->findData(data.value("exhibit_id"));
    if (exhibitIdx >= 0) m_exhibitCombo->setCurrentIndex(exhibitIdx);
    
    int typeIdx = m_typeCombo->findData(data.value("restoration_type_id"));
    if (typeIdx >= 0) m_typeCombo->setCurrentIndex(typeIdx);
    
    int restorerIdx = m_restorerCombo->findData(data.value("restorer_id"));
    if (restorerIdx >= 0) m_restorerCombo->setCurrentIndex(restorerIdx);
    
    m_statusCombo->setCurrentText(data.value("status", "В процессе").toString());
    
    QDate startDate = QDate::fromString(data.value("start_date").toString(), "yyyy-MM-dd");
    if (startDate.isValid()) m_startDateEdit->setDate(startDate);
    
    QDate endDate = QDate::fromString(data.value("end_date").toString(), "yyyy-MM-dd");
    if (endDate.isValid()) m_endDateEdit->setDate(endDate);
    
    m_costSpin->setValue(data.value("cost", 0).toDouble());
    m_conditionBeforeEdit->setPlainText(data.value("condition_before").toString());
    m_conditionAfterEdit->setPlainText(data.value("condition_after").toString());
    m_workDescriptionEdit->setPlainText(data.value("work_description").toString());
    m_materialsEdit->setPlainText(data.value("materials_used").toString());
}

QVariantMap RestorationDialog::getData() const
{
    QVariantMap data;
    
    data["exhibit_id"] = m_exhibitCombo->currentData();
    data["restoration_type_id"] = m_typeCombo->currentData();
    data["restorer_id"] = m_restorerCombo->currentData();
    data["status"] = m_statusCombo->currentText();
    data["start_date"] = m_startDateEdit->date().toString("yyyy-MM-dd");
    data["end_date"] = m_endDateEdit->date().toString("yyyy-MM-dd");
    data["cost"] = m_costSpin->value();
    data["condition_before"] = m_conditionBeforeEdit->toPlainText().trimmed();
    data["condition_after"] = m_conditionAfterEdit->toPlainText().trimmed();
    data["work_description"] = m_workDescriptionEdit->toPlainText().trimmed();
    data["materials_used"] = m_materialsEdit->toPlainText().trimmed();
    
    return data;
}
