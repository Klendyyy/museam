#ifndef RESTORATIONDIALOG_H
#define RESTORATIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class RestorationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RestorationDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;
    void setExhibitId(int exhibitId);

private:
    void setupUi();
    void loadComboData();
    
    QComboBox *m_exhibitCombo;
    QComboBox *m_typeCombo;
    QComboBox *m_restorerCombo;
    QComboBox *m_statusCombo;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QTextEdit *m_conditionBeforeEdit;
    QTextEdit *m_conditionAfterEdit;
    QTextEdit *m_workDescriptionEdit;
    QTextEdit *m_materialsEdit;
    QDoubleSpinBox *m_costSpin;
    QTextEdit *m_notesEdit;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // RESTORATIONDIALOG_H
