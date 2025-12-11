#ifndef EMPLOYEEMODEL_H
#define EMPLOYEEMODEL_H

#include <QSqlRelationalTableModel>

class EmployeeModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit EmployeeModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        LastName,
        FirstName,
        MiddleName,
        PositionId,
        Phone,
        Email,
        HireDate,
        DismissalDate,
        IsActive,
        CreatedAt,
        UpdatedAt
    };
    
    void setupModel();
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
