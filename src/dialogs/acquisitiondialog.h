#ifndef ACQUISITIONDIALOG_H
#define ACQUISITIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class AcquisitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AcquisitionDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;
    void setExhibitId(int exhibitId);

private:
    void setupUi();
    void loadComboData();
    
    QComboBox *m_exhibitCombo;
    QDateEdit *m_acquisitionDateEdit;
    QComboBox *m_sourceCombo;
    QTextEdit *m_sourceDetailsEdit;
    QLineEdit *m_documentNumberEdit;
    QDateEdit *m_documentDateEdit;
    QDoubleSpinBox *m_priceSpin;
    QComboBox *m_responsibleCombo;
    QTextEdit *m_notesEdit;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // ACQUISITIONDIALOG_H
