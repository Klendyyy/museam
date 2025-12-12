#ifndef EMPLOYEEMODEL_H
#define EMPLOYEEMODEL_H

#include <QSqlQueryModel>
#include <QVariantMap>
#include <QDate>
#include <QList>
#include <QPair>

class EmployeeModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit EmployeeModel(QObject *parent = nullptr);

    enum Columns {
        Id = 0,
        LastName = 1,
        FirstName = 2,
        MiddleName = 3,
        PositionId = 4,
        PositionName = 5,
        Phone = 6,
        Email = 7,
        HireDate = 8,
        DismissalDate = 9,
        IsActive = 10
    };

    void refresh();

    bool addEmployee(const QVariantMap& data);
    bool updateEmployee(int id, const QVariantMap& data);
    bool deleteEmployee(int id);
    bool dismissEmployee(int id, const QDate& dismissalDate);

    QVariantMap getEmployeeById(int id);
    QString getEmployeeFullName(int id);
    QList<QPair<int, QString>> getActiveEmployees();
    QList<QPair<int, QString>> getEmployeesByPosition(int positionId);

    void setFilterByPosition(int positionId);
    void setShowInactive(bool show);

signals:
    void dataChanged();

private:
    bool m_showInactive;
};

#endif // EMPLOYEEMODEL_H
