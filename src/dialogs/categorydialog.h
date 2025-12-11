#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class CategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;

private:
    void setupUi();
    void loadComboData();
    
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QComboBox *m_parentCombo;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // CATEGORYDIALOG_H
