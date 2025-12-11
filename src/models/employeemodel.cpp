#include "employeemodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDate>
#include <QDebug>

EmployeeModel::EmployeeModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
    , m_showInactive(false)
{
    setupModel();
}

void EmployeeModel::setupModel()
{
    setTable("employees");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    setRelation(PositionId, QSqlRelation("positions", "id", "name"));
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(LastName, Qt::Horizontal, tr("Фамилия"));
    setHeaderData(FirstName, Qt::Horizontal, tr("Имя"));
    setHeaderData(MiddleName, Qt::Horizontal, tr("Отчество"));
    setHeaderData(PositionId, Qt::Horizontal, tr("Должность"));
    setHeaderData(Phone, Qt::Horizontal, tr("Телефон"));
    setHeaderData(Email, Qt::Horizontal, tr("Email"));
    setHeaderData(HireDate, Qt::Horizontal, tr("Дата приёма"));
    setHeaderData(DismissalDate, Qt::Horizontal, tr("Дата увольнения"));
    setHeaderData(IsActive, Qt::Horizontal, tr("Активен"));
    
    setFilter("is_active = 1");
    select();
}

void EmployeeModel::refresh()
{
    select();
    emit dataChanged();
}

bool EmployeeModel::addEmployee(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        INSERT INTO employees (last_name, first_name, middle_name, position_id, 
                               phone, email, hire_date, is_active)
        VALUES (:last_name, :first_name, :middle_name, :position_id,
                :phone, :email, :hire_date, 1)
    )");
    
    query.bindValue(":last_name", data.value("last_name"));
    query.bindValue(":first_name", data.value("first_name"));
    query.bindValue(":middle_name", data.value("middle_name"));
    query.bindValue(":position_id", data.value("position_id"));
    query.bindValue(":phone", data.value("phone"));
    query.bindValue(":email", data.value("email"));
    query.bindValue(":hire_date", data.value("hire_date"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления сотрудника:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool EmployeeModel::updateEmployee(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE employees SET
            last_name = :last_name,
            first_name = :first_name,
            middle_name = :middle_name,
            position_id = :position_id,
            phone = :phone,
            email = :email,
            hire_date = :hire_date,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":last_name", data.value("last_name"));
    query.bindValue(":first_name", data.value("first_name"));
    query.bindValue(":middle_name", data.value("middle_name"));
    query.bindValue(":position_id", data.value("position_id"));
    query.bindValue(":phone", data.value("phone"));
    query.bindValue(":email", data.value("email"));
    query.bindValue(":hire_date", data.value("hire_date"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления сотрудника:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool EmployeeModel::deleteEmployee(int id)
{
    // Мягкое удаление
    return dismissEmployee(id, QDate::currentDate());
}

bool EmployeeModel::dismissEmployee(int id, const QDate& dismissalDate)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE employees SET 
            is_active = 0, 
            dismissal_date = :dismissal_date,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":dismissal_date", dismissalDate.toString("yyyy-MM-dd"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка увольнения сотрудника:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

QVariantMap EmployeeModel::getEmployeeById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM employees WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

QString EmployeeModel::getEmployeeFullName(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT last_name, first_name, middle_name FROM employees WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QString fullName = query.value(0).toString() + " " + query.value(1).toString();
        if (!query.value(2).toString().isEmpty()) {
            fullName += " " + query.value(2).toString();
        }
        return fullName;
    }
    
    return QString();
}

QList<QPair<int, QString>> EmployeeModel::getActiveEmployees()
{
    QList<QPair<int, QString>> result;
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.exec(R"(
        SELECT id, last_name || ' ' || first_name || 
               CASE WHEN middle_name IS NOT NULL AND middle_name != '' 
                    THEN ' ' || middle_name ELSE '' END as full_name
        FROM employees WHERE is_active = 1
        ORDER BY last_name, first_name
    )");
    
    while (query.next()) {
        result.append({query.value(0).toInt(), query.value(1).toString()});
    }
    
    return result;
}

QList<QPair<int, QString>> EmployeeModel::getEmployeesByPosition(int positionId)
{
    QList<QPair<int, QString>> result;
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        SELECT id, last_name || ' ' || first_name || 
               CASE WHEN middle_name IS NOT NULL AND middle_name != '' 
                    THEN ' ' || middle_name ELSE '' END as full_name
        FROM employees 
        WHERE is_active = 1 AND position_id = :position_id
        ORDER BY last_name, first_name
    )");
    query.bindValue(":position_id", positionId);
    
    if (query.exec()) {
        while (query.next()) {
            result.append({query.value(0).toInt(), query.value(1).toString()});
        }
    }
    
    return result;
}

void EmployeeModel::setFilterByPosition(int positionId)
{
    QString filter = m_showInactive ? "" : "is_active = 1";
    
    if (positionId > 0) {
        if (!filter.isEmpty()) {
            filter += " AND ";
        }
        filter += QString("employees.position_id = %1").arg(positionId);
    }
    
    setFilter(filter);
    select();
}

void EmployeeModel::setShowInactive(bool show)
{
    m_showInactive = show;
    setFilter(show ? "" : "is_active = 1");
    select();
}
