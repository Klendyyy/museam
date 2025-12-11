#include "exhibitiondialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

ExhibitionDialog::ExhibitionDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(550, 500);
}

void ExhibitionDialog::setupUi()
{
    setWindowTitle(tr("Выставка"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Основная информация
    QGroupBox *basicGroup = new QGroupBox(tr("Основная информация"));
    QFormLayout *basicForm = new QFormLayout(basicGroup);
    basicForm->setSpacing(10);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Название выставки"));
    basicForm->addRow(tr("Название*:"), m_nameEdit);
    
    m_locationEdit = new QLineEdit();
    m_locationEdit->setPlaceholderText(tr("Зал, павильон"));
    basicForm->addRow(tr("Место проведения:"), m_locationEdit);
    
    m_curatorCombo = new QComboBox();
    basicForm->addRow(tr("Куратор:"), m_curatorCombo);
    
    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({"Планируется", "Активна", "Приостановлена", "Завершена", "Отменена"});
    basicForm->addRow(tr("Статус:"), m_statusCombo);
    
    m_isPermanentCheck = new QCheckBox(tr("Постоянная выставка"));
    basicForm->addRow("", m_isPermanentCheck);
    
    mainLayout->addWidget(basicGroup);
    
    // Даты и статистика
    QGroupBox *datesGroup = new QGroupBox(tr("Сроки и статистика"));
    QFormLayout *datesForm = new QFormLayout(datesGroup);
    datesForm->setSpacing(10);
    
    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDate(QDate::currentDate());
    m_startDateEdit->setDisplayFormat("dd.MM.yyyy");
    datesForm->addRow(tr("Дата начала*:"), m_startDateEdit);
    
    m_endDateEdit = new QDateEdit();
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDate(QDate::currentDate().addMonths(3));
    m_endDateEdit->setDisplayFormat("dd.MM.yyyy");
    datesForm->addRow(tr("Дата окончания:"), m_endDateEdit);
    
    m_visitorCountSpin = new QSpinBox();
    m_visitorCountSpin->setRange(0, 10000000);
    m_visitorCountSpin->setSuffix(tr(" чел."));
    datesForm->addRow(tr("Посетителей:"), m_visitorCountSpin);
    
    m_budgetSpin = new QDoubleSpinBox();
    m_budgetSpin->setRange(0, 999999999);
    m_budgetSpin->setDecimals(2);
    m_budgetSpin->setSuffix(tr(" ₽"));
    m_budgetSpin->setGroupSeparatorShown(true);
    datesForm->addRow(tr("Бюджет:"), m_budgetSpin);
    
    mainLayout->addWidget(datesGroup);
    
    // Описание
    QLabel *descLabel = new QLabel(tr("Описание:"));
    mainLayout->addWidget(descLabel);
    
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(80);
    mainLayout->addWidget(m_descriptionEdit);
    
    mainLayout->addStretch();
    
    // Кнопки
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите название выставки"));
            m_nameEdit->setFocus();
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void ExhibitionDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    m_curatorCombo->clear();
    m_curatorCombo->addItem(tr("-- Не назначен --"), QVariant());
    
    query.exec(R"(
        SELECT id, last_name || ' ' || first_name as name 
        FROM employees 
        WHERE is_active = 1 
        ORDER BY last_name, first_name
    )");
    while (query.next()) {
        m_curatorCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void ExhibitionDialog::setData(const QVariantMap& data)
{
    m_nameEdit->setText(data.value("name").toString());
    m_descriptionEdit->setPlainText(data.value("description").toString());
    m_locationEdit->setText(data.value("location").toString());
    
    int curatorIdx = m_curatorCombo->findData(data.value("curator_id"));
    if (curatorIdx >= 0) m_curatorCombo->setCurrentIndex(curatorIdx);
    
    m_statusCombo->setCurrentText(data.value("status", "Планируется").toString());
    
    QDate startDate = QDate::fromString(data.value("start_date").toString(), "yyyy-MM-dd");
    if (startDate.isValid()) m_startDateEdit->setDate(startDate);
    
    QDate endDate = QDate::fromString(data.value("end_date").toString(), "yyyy-MM-dd");
    if (endDate.isValid()) m_endDateEdit->setDate(endDate);
    
    m_visitorCountSpin->setValue(data.value("visitor_count", 0).toInt());
    m_budgetSpin->setValue(data.value("budget", 0).toDouble());
    m_isPermanentCheck->setChecked(data.value("is_permanent", false).toBool());
}

QVariantMap ExhibitionDialog::getData() const
{
    QVariantMap data;
    
    data["name"] = m_nameEdit->text().trimmed();
    data["description"] = m_descriptionEdit->toPlainText().trimmed();
    data["location"] = m_locationEdit->text().trimmed();
    data["curator_id"] = m_curatorCombo->currentData();
    data["status"] = m_statusCombo->currentText();
    data["start_date"] = m_startDateEdit->date().toString("yyyy-MM-dd");
    data["end_date"] = m_endDateEdit->date().toString("yyyy-MM-dd");
    data["visitor_count"] = m_visitorCountSpin->value();
    data["budget"] = m_budgetSpin->value();
    data["is_permanent"] = m_isPermanentCheck->isChecked();
    
    return data;
}
