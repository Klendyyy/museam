#include "exhibitdialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

ExhibitDialog::ExhibitDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(700, 600);
}

void ExhibitDialog::setupUi()
{
    setWindowTitle(tr("Экспонат"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Создаём вкладки для группировки полей
    QTabWidget *tabWidget = new QTabWidget();
    
    // === Вкладка "Основное" ===
    QWidget *basicTab = new QWidget();
    QVBoxLayout *basicLayout = new QVBoxLayout(basicTab);
    
    QFormLayout *basicForm = new QFormLayout();
    basicForm->setSpacing(10);
    
    m_inventoryEdit = new QLineEdit();
    m_inventoryEdit->setPlaceholderText(tr("Например: ЖИВ-001-2024"));
    basicForm->addRow(tr("Инвентарный номер*:"), m_inventoryEdit);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Полное название экспоната"));
    basicForm->addRow(tr("Название*:"), m_nameEdit);
    
    m_categoryCombo = new QComboBox();
    basicForm->addRow(tr("Категория:"), m_categoryCombo);
    
    m_collectionCombo = new QComboBox();
    basicForm->addRow(tr("Коллекция:"), m_collectionCombo);
    
    m_storageCombo = new QComboBox();
    basicForm->addRow(tr("Хранилище:"), m_storageCombo);
    
    basicLayout->addLayout(basicForm);
    
    QGroupBox *descGroup = new QGroupBox(tr("Описание"));
    QVBoxLayout *descLayout = new QVBoxLayout(descGroup);
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(100);
    descLayout->addWidget(m_descriptionEdit);
    basicLayout->addWidget(descGroup);
    
    basicLayout->addStretch();
    tabWidget->addTab(basicTab, tr("Основное"));
    
    // === Вкладка "Детали" ===
    QWidget *detailsTab = new QWidget();
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsTab);
    
    QFormLayout *detailsForm = new QFormLayout();
    detailsForm->setSpacing(10);
    
    m_authorEdit = new QLineEdit();
    m_authorEdit->setPlaceholderText(tr("Автор или мастер"));
    detailsForm->addRow(tr("Автор:"), m_authorEdit);
    
    m_creationDateEdit = new QLineEdit();
    m_creationDateEdit->setPlaceholderText(tr("Например: XVIII век, 1856 г., ок. 1900"));
    detailsForm->addRow(tr("Дата создания:"), m_creationDateEdit);
    
    m_creationPlaceEdit = new QLineEdit();
    m_creationPlaceEdit->setPlaceholderText(tr("Место создания или происхождения"));
    detailsForm->addRow(tr("Место создания:"), m_creationPlaceEdit);
    
    m_acquisitionDateEdit = new QDateEdit();
    m_acquisitionDateEdit->setCalendarPopup(true);
    m_acquisitionDateEdit->setDate(QDate::currentDate());
    m_acquisitionDateEdit->setDisplayFormat("dd.MM.yyyy");
    detailsForm->addRow(tr("Дата поступления:"), m_acquisitionDateEdit);
    
    detailsLayout->addLayout(detailsForm);
    detailsLayout->addStretch();
    tabWidget->addTab(detailsTab, tr("Детали"));
    
    // === Вкладка "Характеристики" ===
    QWidget *physicalTab = new QWidget();
    QVBoxLayout *physicalLayout = new QVBoxLayout(physicalTab);
    
    QFormLayout *physicalForm = new QFormLayout();
    physicalForm->setSpacing(10);
    
    m_materialEdit = new QLineEdit();
    m_materialEdit->setPlaceholderText(tr("Холст, масло; бронза; дерево и т.д."));
    physicalForm->addRow(tr("Материал:"), m_materialEdit);
    
    m_techniqueEdit = new QLineEdit();
    m_techniqueEdit->setPlaceholderText(tr("Техника исполнения"));
    physicalForm->addRow(tr("Техника:"), m_techniqueEdit);
    
    m_dimensionsEdit = new QLineEdit();
    m_dimensionsEdit->setPlaceholderText(tr("Например: 100 x 80 см"));
    physicalForm->addRow(tr("Размеры:"), m_dimensionsEdit);
    
    m_weightSpin = new QDoubleSpinBox();
    m_weightSpin->setRange(0, 100000);
    m_weightSpin->setDecimals(3);
    m_weightSpin->setSuffix(tr(" кг"));
    physicalForm->addRow(tr("Вес:"), m_weightSpin);
    
    m_insuranceValueSpin = new QDoubleSpinBox();
    m_insuranceValueSpin->setRange(0, 999999999);
    m_insuranceValueSpin->setDecimals(2);
    m_insuranceValueSpin->setSuffix(tr(" ₽"));
    m_insuranceValueSpin->setGroupSeparatorShown(true);
    physicalForm->addRow(tr("Страховая стоимость:"), m_insuranceValueSpin);
    
    physicalLayout->addLayout(physicalForm);
    physicalLayout->addStretch();
    tabWidget->addTab(physicalTab, tr("Характеристики"));
    
    // === Вкладка "Статус" ===
    QWidget *statusTab = new QWidget();
    QVBoxLayout *statusLayout = new QVBoxLayout(statusTab);
    
    QFormLayout *statusForm = new QFormLayout();
    statusForm->setSpacing(10);
    
    m_conditionCombo = new QComboBox();
    m_conditionCombo->addItems({"Отличное", "Хорошее", "Удовлетворительное", 
                                "Требует реставрации", "Аварийное"});
    m_conditionCombo->setCurrentText("Хорошее");
    statusForm->addRow(tr("Состояние:"), m_conditionCombo);
    
    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({"В хранении", "На выставке", "На реставрации", 
                             "Во временном хранении", "На экспертизе", "Утрачен", "Списан"});
    statusForm->addRow(tr("Статус:"), m_statusCombo);
    
    m_onDisplayCheck = new QCheckBox(tr("Экспонат находится на выставке"));
    statusForm->addRow("", m_onDisplayCheck);
    
    m_requiresRestorationCheck = new QCheckBox(tr("Требуется реставрация"));
    statusForm->addRow("", m_requiresRestorationCheck);
    
    statusLayout->addLayout(statusForm);
    
    QGroupBox *notesGroup = new QGroupBox(tr("Примечания"));
    QVBoxLayout *notesLayout = new QVBoxLayout(notesGroup);
    m_notesEdit = new QTextEdit();
    m_notesEdit->setMaximumHeight(100);
    notesLayout->addWidget(m_notesEdit);
    statusLayout->addWidget(notesGroup);
    
    statusLayout->addStretch();
    tabWidget->addTab(statusTab, tr("Статус"));
    
    mainLayout->addWidget(tabWidget);
    
    // Кнопки
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        // Валидация
        if (m_inventoryEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите инвентарный номер"));
            m_inventoryEdit->setFocus();
            return;
        }
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите название экспоната"));
            m_nameEdit->setFocus();
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void ExhibitDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    // Категории
    m_categoryCombo->clear();
    m_categoryCombo->addItem(tr("-- Не выбрана --"), QVariant());
    query.exec("SELECT id, name FROM categories ORDER BY name");
    while (query.next()) {
        m_categoryCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Коллекции
    m_collectionCombo->clear();
    m_collectionCombo->addItem(tr("-- Не выбрана --"), QVariant());
    query.exec("SELECT id, name FROM collections WHERE is_active = 1 ORDER BY name");
    while (query.next()) {
        m_collectionCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Хранилища
    m_storageCombo->clear();
    m_storageCombo->addItem(tr("-- Не выбрано --"), QVariant());
    query.exec("SELECT id, name FROM storages WHERE is_active = 1 ORDER BY name");
    while (query.next()) {
        m_storageCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void ExhibitDialog::setData(const QVariantMap& data)
{
    m_inventoryEdit->setText(data.value("inventory_number").toString());
    m_nameEdit->setText(data.value("name").toString());
    m_descriptionEdit->setPlainText(data.value("description").toString());
    
    // Устанавливаем комбобоксы
    int categoryIdx = m_categoryCombo->findData(data.value("category_id"));
    if (categoryIdx >= 0) m_categoryCombo->setCurrentIndex(categoryIdx);
    
    int collectionIdx = m_collectionCombo->findData(data.value("collection_id"));
    if (collectionIdx >= 0) m_collectionCombo->setCurrentIndex(collectionIdx);
    
    int storageIdx = m_storageCombo->findData(data.value("storage_id"));
    if (storageIdx >= 0) m_storageCombo->setCurrentIndex(storageIdx);
    
    m_authorEdit->setText(data.value("author").toString());
    m_creationDateEdit->setText(data.value("creation_date").toString());
    m_creationPlaceEdit->setText(data.value("creation_place").toString());
    
    QDate acqDate = QDate::fromString(data.value("acquisition_date").toString(), "yyyy-MM-dd");
    if (acqDate.isValid()) {
        m_acquisitionDateEdit->setDate(acqDate);
    }
    
    m_materialEdit->setText(data.value("material").toString());
    m_techniqueEdit->setText(data.value("technique").toString());
    m_dimensionsEdit->setText(data.value("dimensions").toString());
    m_weightSpin->setValue(data.value("weight").toDouble());
    m_insuranceValueSpin->setValue(data.value("insurance_value").toDouble());
    
    m_conditionCombo->setCurrentText(data.value("condition", "Хорошее").toString());
    m_statusCombo->setCurrentText(data.value("status", "В хранении").toString());
    m_onDisplayCheck->setChecked(data.value("is_on_display").toBool());
    m_requiresRestorationCheck->setChecked(data.value("requires_restoration").toBool());
    
    m_notesEdit->setPlainText(data.value("notes").toString());
}

QVariantMap ExhibitDialog::getData() const
{
    QVariantMap data;
    
    data["inventory_number"] = m_inventoryEdit->text().trimmed();
    data["name"] = m_nameEdit->text().trimmed();
    data["description"] = m_descriptionEdit->toPlainText().trimmed();
    data["category_id"] = m_categoryCombo->currentData();
    data["collection_id"] = m_collectionCombo->currentData();
    data["storage_id"] = m_storageCombo->currentData();
    data["author"] = m_authorEdit->text().trimmed();
    data["creation_date"] = m_creationDateEdit->text().trimmed();
    data["creation_place"] = m_creationPlaceEdit->text().trimmed();
    data["acquisition_date"] = m_acquisitionDateEdit->date().toString("yyyy-MM-dd");
    data["material"] = m_materialEdit->text().trimmed();
    data["technique"] = m_techniqueEdit->text().trimmed();
    data["dimensions"] = m_dimensionsEdit->text().trimmed();
    data["weight"] = m_weightSpin->value();
    data["insurance_value"] = m_insuranceValueSpin->value();
    data["condition"] = m_conditionCombo->currentText();
    data["status"] = m_statusCombo->currentText();
    data["is_on_display"] = m_onDisplayCheck->isChecked();
    data["requires_restoration"] = m_requiresRestorationCheck->isChecked();
    data["notes"] = m_notesEdit->toPlainText().trimmed();
    
    return data;
}

void ExhibitDialog::setReadOnly(bool readOnly)
{
    m_inventoryEdit->setReadOnly(readOnly);
    m_nameEdit->setReadOnly(readOnly);
    m_descriptionEdit->setReadOnly(readOnly);
    m_categoryCombo->setEnabled(!readOnly);
    m_collectionCombo->setEnabled(!readOnly);
    m_storageCombo->setEnabled(!readOnly);
    m_authorEdit->setReadOnly(readOnly);
    m_creationDateEdit->setReadOnly(readOnly);
    m_creationPlaceEdit->setReadOnly(readOnly);
    m_acquisitionDateEdit->setReadOnly(readOnly);
    m_materialEdit->setReadOnly(readOnly);
    m_techniqueEdit->setReadOnly(readOnly);
    m_dimensionsEdit->setReadOnly(readOnly);
    m_weightSpin->setReadOnly(readOnly);
    m_insuranceValueSpin->setReadOnly(readOnly);
    m_conditionCombo->setEnabled(!readOnly);
    m_statusCombo->setEnabled(!readOnly);
    m_onDisplayCheck->setEnabled(!readOnly);
    m_requiresRestorationCheck->setEnabled(!readOnly);
    m_notesEdit->setReadOnly(readOnly);
    
    if (readOnly) {
        m_buttonBox->setStandardButtons(QDialogButtonBox::Close);
    }
}
