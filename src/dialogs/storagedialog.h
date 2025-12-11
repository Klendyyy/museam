#ifndef STORAGEDIALOG_H
#define STORAGEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class StorageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StorageDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;

private slots:
    void onClimateControlChanged(int state);

private:
    void setupUi();
    void loadComboData();
    
    QLineEdit *m_nameEdit;
    QLineEdit *m_locationEdit;
    QComboBox *m_typeCombo;
    QSpinBox *m_capacitySpin;
    QCheckBox *m_climateControlCheck;
    QDoubleSpinBox *m_tempMinSpin;
    QDoubleSpinBox *m_tempMaxSpin;
    QDoubleSpinBox *m_humidityMinSpin;
    QDoubleSpinBox *m_humidityMaxSpin;
    QComboBox *m_responsibleCombo;
    QTextEdit *m_descriptionEdit;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // STORAGEDIALOG_H
