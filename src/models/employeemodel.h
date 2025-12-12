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
        PositionName = 4,
        Phone = 5,
        Email = 6,
        HireDate = 7,
        DismissalDate = 8  // Только когда показываются уволенные
    };

    void refresh();

    bool addEmployee(const QVariantMap& data);
    bool updateEmployee(int id, const QVariantMap& data);
    bool deleteEmployee(int id);
    bool dismissEmployee(int id, const QDate& dismissalDate);
    bool reinstateEmployee(int id);  // Восстановление уволенного сотрудника

    QVariantMap getEmployeeById(int id);
    QString getEmployeeFullName(int id);
    QList<QPair<int, QString>> getActiveEmployees();
    QList<QPair<int, QString>> getEmployeesByPosition(int positionId);

    void setFilterByPosition(int positionId);
    void setShowInactive(bool show);

    bool isShowingInactive() const { return m_showInactive; }

signals:
    void dataChanged();

private:
    bool m_showInactive;
    int m_currentPositionFilter;
};

#endif // EMPLOYEEMODEL_H
