#include "collectiondialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

CollectionDialog::CollectionDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(500, 350);
}

void CollectionDialog::setupUi()
{
    setWindowTitle(tr("Коллекция"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Название коллекции"));
    formLayout->addRow(tr("Название*:"), m_nameEdit);
    
    m_curatorCombo = new QComboBox();
    formLayout->addRow(tr("Куратор:"), m_curatorCombo);
    
    m_creationDateEdit = new QDateEdit();
    m_creationDateEdit->setCalendarPopup(true);
    m_creationDateEdit->setDate(QDate::currentDate());
    m_creationDateEdit->setDisplayFormat("dd.MM.yyyy");
    formLayout->addRow(tr("Дата создания:"), m_creationDateEdit);
    
    m_isPermanentCheck = new QCheckBox(tr("Постоянная коллекция"));
    m_isPermanentCheck->setChecked(true);
    formLayout->addRow("", m_isPermanentCheck);
    
    mainLayout->addLayout(formLayout);
    
    QLabel *descLabel = new QLabel(tr("Описание:"));
    mainLayout->addWidget(descLabel);
    
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(100);
    mainLayout->addWidget(m_descriptionEdit);
    
    mainLayout->addStretch();
    
    // Кнопки
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите название коллекции"));
            m_nameEdit->setFocus();
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void CollectionDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    // Кураторы (сотрудники)
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

void CollectionDialog::setData(const QVariantMap& data)
{
    m_nameEdit->setText(data.value("name").toString());
    m_descriptionEdit->setPlainText(data.value("description").toString());
    
    int curatorIdx = m_curatorCombo->findData(data.value("curator_id"));
    if (curatorIdx >= 0) m_curatorCombo->setCurrentIndex(curatorIdx);
    
    QDate date = QDate::fromString(data.value("creation_date").toString(), "yyyy-MM-dd");
    if (date.isValid()) {
        m_creationDateEdit->setDate(date);
    }
    
    m_isPermanentCheck->setChecked(data.value("is_permanent", true).toBool());
}

QVariantMap CollectionDialog::getData() const
{
    QVariantMap data;
    
    data["name"] = m_nameEdit->text().trimmed();
    data["description"] = m_descriptionEdit->toPlainText().trimmed();
    data["curator_id"] = m_curatorCombo->currentData();
    data["creation_date"] = m_creationDateEdit->date().toString("yyyy-MM-dd");
    data["is_permanent"] = m_isPermanentCheck->isChecked();
    
    return data;
}
