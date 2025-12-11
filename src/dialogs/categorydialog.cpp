#include "categorydialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

CategoryDialog::CategoryDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(400, 300);
}

void CategoryDialog::setupUi()
{
    setWindowTitle(tr("Категория"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Название категории"));
    formLayout->addRow(tr("Название*:"), m_nameEdit);
    
    m_parentCombo = new QComboBox();
    formLayout->addRow(tr("Родительская:"), m_parentCombo);
    
    mainLayout->addLayout(formLayout);
    
    QLabel *descLabel = new QLabel(tr("Описание:"));
    mainLayout->addWidget(descLabel);
    
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(80);
    mainLayout->addWidget(m_descriptionEdit);
    
    mainLayout->addStretch();
    
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите название категории"));
            m_nameEdit->setFocus();
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void CategoryDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    m_parentCombo->clear();
    m_parentCombo->addItem(tr("-- Нет (корневая) --"), QVariant());
    
    query.exec("SELECT id, name FROM categories ORDER BY name");
    while (query.next()) {
        m_parentCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void CategoryDialog::setData(const QVariantMap& data)
{
    m_nameEdit->setText(data.value("name").toString());
    m_descriptionEdit->setPlainText(data.value("description").toString());
    
    int parentIdx = m_parentCombo->findData(data.value("parent_id"));
    if (parentIdx >= 0) m_parentCombo->setCurrentIndex(parentIdx);
}

QVariantMap CategoryDialog::getData() const
{
    QVariantMap data;
    
    data["name"] = m_nameEdit->text().trimmed();
    data["description"] = m_descriptionEdit->toPlainText().trimmed();
    data["parent_id"] = m_parentCombo->currentData();
    
    return data;
}
