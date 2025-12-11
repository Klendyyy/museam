#ifndef EMPLOYEEWIDGET_H
#define EMPLOYEEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

class EmployeeModel;

class EmployeeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmployeeWidget(QWidget *parent = nullptr);
    
    void refresh();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onPositionFilterChanged(int index);
    void onShowInactiveChanged(int state);
    void onTableDoubleClicked(const QModelIndex& index);

private:
    void setupUi();
    void loadFilters();
    int getSelectedId();
    
    QTableView *m_tableView;
    EmployeeModel *m_model;
    
    QComboBox *m_positionCombo;
    QCheckBox *m_showInactiveCheck;
    
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // EMPLOYEEWIDGET_H
