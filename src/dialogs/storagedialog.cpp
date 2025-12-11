#include "storagedialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

StorageDialog::StorageDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(500, 550);
}

void StorageDialog::setupUi()
{
    setWindowTitle(tr("Хранилище"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Основная информация
    QGroupBox *basicGroup = new QGroupBox(tr("Основная информация"));
    QFormLayout *basicForm = new QFormLayout(basicGroup);
    basicForm->setSpacing(10);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Название хранилища"));
    basicForm->addRow(tr("Название*:"), m_nameEdit);
    
    m_locationEdit = new QLineEdit();
    m_locationEdit->setPlaceholderText(tr("Здание, этаж, помещение"));
    basicForm->addRow(tr("Расположение:"), m_locationEdit);
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItems({"Основное хранилище", "Запасник", "Выставочный зал", 
                          "Реставрационная мастерская", "Временное хранилище", "Депозитарий"});
    basicForm->addRow(tr("Тип*:"), m_typeCombo);
    
    m_capacitySpin = new QSpinBox();
    m_capacitySpin->setRange(0, 100000);
    m_capacitySpin->setSuffix(tr(" ед."));
    basicForm->addRow(tr("Вместимость:"), m_capacitySpin);
    
    m_responsibleCombo = new QComboBox();
    basicForm->addRow(tr("Ответственный:"), m_responsibleCombo);
    
    mainLayout->addWidget(basicGroup);
    
    // Климат-контроль
    QGroupBox *climateGroup = new QGroupBox(tr("Климат-контроль"));
    QVBoxLayout *climateLayout = new QVBoxLayout(climateGroup);
    
    m_climateControlCheck = new QCheckBox(tr("Есть климат-контроль"));
    connect(m_climateControlCheck, &QCheckBox::stateChanged, this, &StorageDialog::onClimateControlChanged);
    climateLayout->addWidget(m_climateControlCheck);
    
    QFormLayout *climateForm = new QFormLayout();
    climateForm->setSpacing(10);
    
    QHBoxLayout *tempLayout = new QHBoxLayout();
    m_tempMinSpin = new QDoubleSpinBox();
    m_tempMinSpin->setRange(-50, 100);
    m_tempMinSpin->setSuffix(tr(" °C"));
    m_tempMinSpin->setEnabled(false);
    tempLayout->addWidget(m_tempMinSpin);
    tempLayout->addWidget(new QLabel(tr(" — ")));
    m_tempMaxSpin = new QDoubleSpinBox();
    m_tempMaxSpin->setRange(-50, 100);
    m_tempMaxSpin->setSuffix(tr(" °C"));
    m_tempMaxSpin->setEnabled(false);
    tempLayout->addWidget(m_tempMaxSpin);
    climateForm->addRow(tr("Температура:"), tempLayout);
    
    QHBoxLayout *humidityLayout = new QHBoxLayout();
    m_humidityMinSpin = new QDoubleSpinBox();
    m_humidityMinSpin->setRange(0, 100);
    m_humidityMinSpin->setSuffix(tr(" %"));
    m_humidityMinSpin->setEnabled(false);
    humidityLayout->addWidget(m_humidityMinSpin);
    humidityLayout->addWidget(new QLabel(tr(" — ")));
    m_humidityMaxSpin = new QDoubleSpinBox();
    m_humidityMaxSpin->setRange(0, 100);
    m_humidityMaxSpin->setSuffix(tr(" %"));
    m_humidityMaxSpin->setEnabled(false);
    humidityLayout->addWidget(m_humidityMaxSpin);
    climateForm->addRow(tr("Влажность:"), humidityLayout);
    
    climateLayout->addLayout(climateForm);
    mainLayout->addWidget(climateGroup);
    
    // Описание
    QLabel *descLabel = new QLabel(tr("Описание:"));
    mainLayout->addWidget(descLabel);
    
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(70);
    mainLayout->addWidget(m_descriptionEdit);
    
    mainLayout->addStretch();
    
    // Кнопки
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите название хранилища"));
            m_nameEdit->setFocus();
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void StorageDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    m_responsibleCombo->clear();
    m_responsibleCombo->addItem(tr("-- Не назначен --"), QVariant());
    
    query.exec(R"(
        SELECT id, last_name || ' ' || first_name as name 
        FROM employees 
        WHERE is_active = 1 
        ORDER BY last_name, first_name
    )");
    while (query.next()) {
        m_responsibleCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void StorageDialog::onClimateControlChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    m_tempMinSpin->setEnabled(enabled);
    m_tempMaxSpin->setEnabled(enabled);
    m_humidityMinSpin->setEnabled(enabled);
    m_humidityMaxSpin->setEnabled(enabled);
}

void StorageDialog::setData(const QVariantMap& data)
{
    m_nameEdit->setText(data.value("name").toString());
    m_locationEdit->setText(data.value("location").toString());
    
    int typeIdx = m_typeCombo->findText(data.value("storage_type").toString());
    if (typeIdx >= 0) m_typeCombo->setCurrentIndex(typeIdx);
    
    m_capacitySpin->setValue(data.value("capacity", 0).toInt());
    
    int respIdx = m_responsibleCombo->findData(data.value("responsible_employee_id"));
    if (respIdx >= 0) m_responsibleCombo->setCurrentIndex(respIdx);
    
    bool hasClimate = data.value("climate_control", false).toBool();
    m_climateControlCheck->setChecked(hasClimate);
    
    m_tempMinSpin->setValue(data.value("temperature_min", 18).toDouble());
    m_tempMaxSpin->setValue(data.value("temperature_max", 22).toDouble());
    m_humidityMinSpin->setValue(data.value("humidity_min", 40).toDouble());
    m_humidityMaxSpin->setValue(data.value("humidity_max", 60).toDouble());
    
    m_descriptionEdit->setPlainText(data.value("description").toString());
}

QVariantMap StorageDialog::getData() const
{
    QVariantMap data;
    
    data["name"] = m_nameEdit->text().trimmed();
    data["location"] = m_locationEdit->text().trimmed();
    data["storage_type"] = m_typeCombo->currentText();
    data["capacity"] = m_capacitySpin->value();
    data["responsible_employee_id"] = m_responsibleCombo->currentData();
    data["climate_control"] = m_climateControlCheck->isChecked();
    
    if (m_climateControlCheck->isChecked()) {
        data["temperature_min"] = m_tempMinSpin->value();
        data["temperature_max"] = m_tempMaxSpin->value();
        data["humidity_min"] = m_humidityMinSpin->value();
        data["humidity_max"] = m_humidityMaxSpin->value();
    }
    
    data["description"] = m_descriptionEdit->toPlainText().trimmed();
    
    return data;
}
