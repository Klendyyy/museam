#ifndef EMPLOYEEDIALOG_H
#define EMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QVariantMap>

class EmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;

private:
    void setupUi();
    void loadComboData();
    
    QLineEdit *m_lastNameEdit;
    QLineEdit *m_firstNameEdit;
    QLineEdit *m_middleNameEdit;
    QComboBox *m_positionCombo;
    QLineEdit *m_phoneEdit;
    QLineEdit *m_emailEdit;
    QDateEdit *m_hireDateEdit;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // EMPLOYEEDIALOG_H
