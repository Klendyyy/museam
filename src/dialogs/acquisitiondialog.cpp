#include "acquisitiondialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

AcquisitionDialog::AcquisitionDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(500, 500);
}

void AcquisitionDialog::setupUi()
{
    setWindowTitle(tr("Поступление экспоната"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    
    m_exhibitCombo = new QComboBox();
    formLayout->addRow(tr("Экспонат*:"), m_exhibitCombo);
    
    m_acquisitionDateEdit = new QDateEdit();
    m_acquisitionDateEdit->setCalendarPopup(true);
    m_acquisitionDateEdit->setDate(QDate::currentDate());
    m_acquisitionDateEdit->setDisplayFormat("dd.MM.yyyy");
    formLayout->addRow(tr("Дата поступления*:"), m_acquisitionDateEdit);
    
    m_sourceCombo = new QComboBox();
    formLayout->addRow(tr("Источник:"), m_sourceCombo);
    
    m_priceSpin = new QDoubleSpinBox();
    m_priceSpin->setRange(0, 999999999);
    m_priceSpin->setDecimals(2);
    m_priceSpin->setSuffix(tr(" ₽"));
    formLayout->addRow(tr("Стоимость:"), m_priceSpin);
    
    m_documentNumberEdit = new QLineEdit();
    m_documentNumberEdit->setPlaceholderText(tr("Номер акта приёма"));
    formLayout->addRow(tr("№ документа:"), m_documentNumberEdit);
    
    m_documentDateEdit = new QDateEdit();
    m_documentDateEdit->setCalendarPopup(true);
    m_documentDateEdit->setDate(QDate::currentDate());
    m_documentDateEdit->setDisplayFormat("dd.MM.yyyy");
    formLayout->addRow(tr("Дата документа:"), m_documentDateEdit);
    
    m_responsibleCombo = new QComboBox();
    formLayout->addRow(tr("Ответственный:"), m_responsibleCombo);
    
    mainLayout->addLayout(formLayout);
    
    QLabel *sourceLabel = new QLabel(tr("Детали источника:"));
    mainLayout->addWidget(sourceLabel);
    m_sourceDetailsEdit = new QTextEdit();
    m_sourceDetailsEdit->setMaximumHeight(60);
    m_sourceDetailsEdit->setPlaceholderText(tr("ФИО дарителя, название организации..."));
    mainLayout->addWidget(m_sourceDetailsEdit);
    
    QLabel *notesLabel = new QLabel(tr("Примечания:"));
    mainLayout->addWidget(notesLabel);
    m_notesEdit = new QTextEdit();
    m_notesEdit->setMaximumHeight(60);
    mainLayout->addWidget(m_notesEdit);
    
    mainLayout->addStretch();
    
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

void AcquisitionDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    // Экспонаты
    m_exhibitCombo->clear();
    m_exhibitCombo->addItem(tr("-- Выберите экспонат --"), QVariant());
    query.exec("SELECT id, inventory_number || ' - ' || name FROM exhibits ORDER BY inventory_number");
    while (query.next()) {
        m_exhibitCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Источники
    m_sourceCombo->clear();
    m_sourceCombo->addItem(tr("-- Не указан --"), QVariant());
    query.exec("SELECT id, name FROM acquisition_sources ORDER BY name");
    while (query.next()) {
        m_sourceCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
    
    // Ответственные
    m_responsibleCombo->clear();
    m_responsibleCombo->addItem(tr("-- Не назначен --"), QVariant());
    query.exec(R"(
        SELECT id, last_name || ' ' || first_name 
        FROM employees WHERE is_active = 1 
        ORDER BY last_name
    )");
    while (query.next()) {
        m_responsibleCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void AcquisitionDialog::setExhibitId(int exhibitId)
{
    int idx = m_exhibitCombo->findData(exhibitId);
    if (idx >= 0) {
        m_exhibitCombo->setCurrentIndex(idx);
        m_exhibitCombo->setEnabled(false);
    }
}

void AcquisitionDialog::setData(const QVariantMap& data)
{
    int exhibitIdx = m_exhibitCombo->findData(data.value("exhibit_id"));
    if (exhibitIdx >= 0) m_exhibitCombo->setCurrentIndex(exhibitIdx);
    
    QDate acqDate = QDate::fromString(data.value("acquisition_date").toString(), "yyyy-MM-dd");
    if (acqDate.isValid()) m_acquisitionDateEdit->setDate(acqDate);
    
    int sourceIdx = m_sourceCombo->findData(data.value("source_id"));
    if (sourceIdx >= 0) m_sourceCombo->setCurrentIndex(sourceIdx);
    
    m_sourceDetailsEdit->setPlainText(data.value("source_details").toString());
    m_documentNumberEdit->setText(data.value("document_number").toString());
    
    QDate docDate = QDate::fromString(data.value("document_date").toString(), "yyyy-MM-dd");
    if (docDate.isValid()) m_documentDateEdit->setDate(docDate);
    
    m_priceSpin->setValue(data.value("price", 0).toDouble());
    
    int respIdx = m_responsibleCombo->findData(data.value("responsible_employee_id"));
    if (respIdx >= 0) m_responsibleCombo->setCurrentIndex(respIdx);
    
    m_notesEdit->setPlainText(data.value("notes").toString());
}

QVariantMap AcquisitionDialog::getData() const
{
    QVariantMap data;
    
    data["exhibit_id"] = m_exhibitCombo->currentData();
    data["acquisition_date"] = m_acquisitionDateEdit->date().toString("yyyy-MM-dd");
    data["source_id"] = m_sourceCombo->currentData();
    data["source_details"] = m_sourceDetailsEdit->toPlainText().trimmed();
    data["document_number"] = m_documentNumberEdit->text().trimmed();
    data["document_date"] = m_documentDateEdit->date().toString("yyyy-MM-dd");
    data["price"] = m_priceSpin->value();
    data["responsible_employee_id"] = m_responsibleCombo->currentData();
    data["notes"] = m_notesEdit->toPlainText().trimmed();
    
    return data;
}
