#include "employeedialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QRegularExpressionValidator>

EmployeeDialog::EmployeeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadComboData();
    resize(450, 350);
}

void EmployeeDialog::setupUi()
{
    setWindowTitle(tr("Сотрудник"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    
    m_lastNameEdit = new QLineEdit();
    m_lastNameEdit->setPlaceholderText(tr("Фамилия"));
    formLayout->addRow(tr("Фамилия*:"), m_lastNameEdit);
    
    m_firstNameEdit = new QLineEdit();
    m_firstNameEdit->setPlaceholderText(tr("Имя"));
    formLayout->addRow(tr("Имя*:"), m_firstNameEdit);
    
    m_middleNameEdit = new QLineEdit();
    m_middleNameEdit->setPlaceholderText(tr("Отчество (если есть)"));
    formLayout->addRow(tr("Отчество:"), m_middleNameEdit);
    
    m_positionCombo = new QComboBox();
    formLayout->addRow(tr("Должность*:"), m_positionCombo);
    
    m_phoneEdit = new QLineEdit();
    m_phoneEdit->setPlaceholderText(tr("+7 (XXX) XXX-XX-XX"));
    formLayout->addRow(tr("Телефон:"), m_phoneEdit);
    
    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText(tr("example@museum.ru"));
    formLayout->addRow(tr("Email:"), m_emailEdit);
    
    m_hireDateEdit = new QDateEdit();
    m_hireDateEdit->setCalendarPopup(true);
    m_hireDateEdit->setDate(QDate::currentDate());
    m_hireDateEdit->setDisplayFormat("dd.MM.yyyy");
    formLayout->addRow(tr("Дата приёма*:"), m_hireDateEdit);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    
    // Кнопки
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_lastNameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите фамилию"));
            m_lastNameEdit->setFocus();
            return;
        }
        if (m_firstNameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Введите имя"));
            m_firstNameEdit->setFocus();
            return;
        }
        if (m_positionCombo->currentData().isNull()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Выберите должность"));
            m_positionCombo->setFocus();
            return;
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void EmployeeDialog::loadComboData()
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    m_positionCombo->clear();
    m_positionCombo->addItem(tr("-- Выберите должность --"), QVariant());
    
    query.exec("SELECT id, name FROM positions ORDER BY name");
    while (query.next()) {
        m_positionCombo->addItem(query.value(1).toString(), query.value(0).toInt());
    }
}

void EmployeeDialog::setData(const QVariantMap& data)
{
    m_lastNameEdit->setText(data.value("last_name").toString());
    m_firstNameEdit->setText(data.value("first_name").toString());
    m_middleNameEdit->setText(data.value("middle_name").toString());
    
    int posIdx = m_positionCombo->findData(data.value("position_id"));
    if (posIdx >= 0) m_positionCombo->setCurrentIndex(posIdx);
    
    m_phoneEdit->setText(data.value("phone").toString());
    m_emailEdit->setText(data.value("email").toString());
    
    QDate hireDate = QDate::fromString(data.value("hire_date").toString(), "yyyy-MM-dd");
    if (hireDate.isValid()) m_hireDateEdit->setDate(hireDate);
}

QVariantMap EmployeeDialog::getData() const
{
    QVariantMap data;
    
    data["last_name"] = m_lastNameEdit->text().trimmed();
    data["first_name"] = m_firstNameEdit->text().trimmed();
    data["middle_name"] = m_middleNameEdit->text().trimmed();
    data["position_id"] = m_positionCombo->currentData();
    data["phone"] = m_phoneEdit->text().trimmed();
    data["email"] = m_emailEdit->text().trimmed();
    data["hire_date"] = m_hireDateEdit->date().toString("yyyy-MM-dd");
    
    return data;
}
