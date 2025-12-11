#ifndef EXHIBITIONDIALOG_H
#define EXHIBITIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class ExhibitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExhibitionDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;

private:
    void setupUi();
    void loadComboData();
    
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QLineEdit *m_locationEdit;
    QComboBox *m_curatorCombo;
    QComboBox *m_statusCombo;
    QSpinBox *m_visitorCountSpin;
    QDoubleSpinBox *m_budgetSpin;
    QCheckBox *m_isPermanentCheck;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // EXHIBITIONDIALOG_H
