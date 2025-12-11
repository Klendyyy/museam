#ifndef COLLECTIONDIALOG_H
#define COLLECTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVariantMap>

class CollectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CollectionDialog(QWidget *parent = nullptr);
    
    void setData(const QVariantMap& data);
    QVariantMap getData() const;

private:
    void setupUi();
    void loadComboData();
    
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QComboBox *m_curatorCombo;
    QDateEdit *m_creationDateEdit;
    QCheckBox *m_isPermanentCheck;
    
    QDialogButtonBox *m_buttonBox;
};

#endif // COLLECTIONDIALOG_H
