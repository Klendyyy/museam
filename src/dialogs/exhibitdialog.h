#ifndef EXHIBITDIALOG_H
#define EXHIBITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class ExhibitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExhibitDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;
    void setReadOnly(bool readOnly);

private:
    void setupUi();
    void loadComboData();
    
    // Основные поля
    QLineEdit *m_inventoryEdit;
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QComboBox *m_categoryCombo;
    QComboBox *m_collectionCombo;
    QComboBox *m_storageCombo;
    
    // Информация об экспонате
    QLineEdit *m_authorEdit;
    QLineEdit *m_creationDateEdit;
    QLineEdit *m_creationPlaceEdit;
    QDateEdit *m_acquisitionDateEdit;
    
    // Физические характеристики
    QLineEdit *m_materialEdit;
    QLineEdit *m_techniqueEdit;
    QLineEdit *m_dimensionsEdit;
    QDoubleSpinBox *m_weightSpin;
    QDoubleSpinBox *m_insuranceValueSpin;
    
    // Статус
    QComboBox *m_conditionCombo;
    QComboBox *m_statusCombo;
    QCheckBox *m_onDisplayCheck;
    QCheckBox *m_requiresRestorationCheck;
    
    // Примечания
    QTextEdit *m_notesEdit;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // EXHIBITDIALOG_H
