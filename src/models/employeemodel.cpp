#include "employeemodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

EmployeeModel::EmployeeModel(QObject *parent)
    : QSqlQueryModel(parent)
    , m_showInactive(false)
    , m_currentPositionFilter(0)
{
    refresh();
}

void EmployeeModel::refresh()
{
    QString sql;

    if (m_showInactive) {
        // Показываем ТОЛЬКО уволенных сотрудников
        sql = R"(
            SELECT
                e.id,
                e.last_name AS 'Фамилия',
                e.first_name AS 'Имя',
                IFNULL(e.middle_name, '') AS 'Отчество',
                IFNULL(p.name, '-') AS 'Должность',
                IFNULL(e.phone, '-') AS 'Телефон',
                IFNULL(e.email, '-') AS 'Email',
                DATE_FORMAT(e.hire_date, '%d.%m.%Y') AS 'Дата приёма',
                DATE_FORMAT(e.dismissal_date, '%d.%m.%Y') AS 'Дата увольнения'
            FROM employees e
            LEFT JOIN positions p ON e.position_id = p.id
            WHERE e.is_active = 0
        )";
    } else {
        // Показываем ТОЛЬКО активных сотрудников
        sql = R"(
            SELECT
                e.id,
                e.last_name AS 'Фамилия',
                e.first_name AS 'Имя',
                IFNULL(e.middle_name, '') AS 'Отчество',
                IFNULL(p.name, '-') AS 'Должность',
                IFNULL(e.phone, '-') AS 'Телефон',
                IFNULL(e.email, '-') AS 'Email',
                DATE_FORMAT(e.hire_date, '%d.%m.%Y') AS 'Дата приёма'
            FROM employees e
            LEFT JOIN positions p ON e.position_id = p.id
            WHERE e.is_active = 1
        )";
    }

    // Применяем фильтр по должности если установлен
    if (m_currentPositionFilter > 0) {
        sql += QString(" AND e.position_id = %1").arg(m_currentPositionFilter);
    }

    sql += " ORDER BY e.last_name, e.first_name";

    setQuery(sql, DatabaseManager::instance().database());

    if (lastError().isValid()) {
        qWarning() << "EmployeeModel error:" << lastError().text();
    } else {
        qDebug() << "EmployeeModel loaded rows:" << rowCount();
    }
}

bool EmployeeModel::addEmployee(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        INSERT INTO employees (last_name, first_name, middle_name, position_id,
                               phone, email, hire_date, is_active)
        VALUES (?, ?, ?, ?, ?, ?, ?, 1)
    )");

    query.addBindValue(data.value("last_name"));
    query.addBindValue(data.value("first_name"));
    query.addBindValue(data.value("middle_name"));
    query.addBindValue(data.value("position_id"));
    query.addBindValue(data.value("phone"));
    query.addBindValue(data.value("email"));
    query.addBindValue(data.value("hire_date"));

    if (!query.exec()) {
        qWarning() << "Add employee error:" << query.lastError().text();
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
            last_name = ?,
            first_name = ?,
            middle_name = ?,
            position_id = ?,
            phone = ?,
            email = ?,
            hire_date = ?
        WHERE id = ?
    )");

    query.addBindValue(data.value("last_name"));
    query.addBindValue(data.value("first_name"));
    query.addBindValue(data.value("middle_name"));
    query.addBindValue(data.value("position_id"));
    query.addBindValue(data.value("phone"));
    query.addBindValue(data.value("email"));
    query.addBindValue(data.value("hire_date"));
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Update employee error:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool EmployeeModel::deleteEmployee(int id)
{
    return dismissEmployee(id, QDate::currentDate());
}

bool EmployeeModel::dismissEmployee(int id, const QDate& dismissalDate)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare("UPDATE employees SET is_active = 0, dismissal_date = ? WHERE id = ?");
    query.addBindValue(dismissalDate.toString("yyyy-MM-dd"));
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Dismiss employee error:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool EmployeeModel::reinstateEmployee(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare("UPDATE employees SET is_active = 1, dismissal_date = NULL WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Reinstate employee error:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

QVariantMap EmployeeModel::getEmployeeById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM employees WHERE id = ?");
    query.addBindValue(id);

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
    query.prepare("SELECT last_name, first_name, middle_name FROM employees WHERE id = ?");
    query.addBindValue(id);

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

    query.exec("SELECT id, CONCAT(last_name, ' ', first_name) FROM employees WHERE is_active = 1 ORDER BY last_name");

    while (query.next()) {
        result.append({query.value(0).toInt(), query.value(1).toString()});
    }

    return result;
}

QList<QPair<int, QString>> EmployeeModel::getEmployeesByPosition(int positionId)
{
    QList<QPair<int, QString>> result;
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare("SELECT id, CONCAT(last_name, ' ', first_name) FROM employees WHERE is_active = 1 AND position_id = ? ORDER BY last_name");
    query.addBindValue(positionId);

    if (query.exec()) {
        while (query.next()) {
            result.append({query.value(0).toInt(), query.value(1).toString()});
        }
    }

    return result;
}

void EmployeeModel::setFilterByPosition(int positionId)
{
    m_currentPositionFilter = positionId;
    refresh();
}

void EmployeeModel::setShowInactive(bool show)
{
    m_showInactive = show;
    refresh();
}
