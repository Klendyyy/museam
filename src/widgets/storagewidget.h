#ifndef STORAGEWIDGET_H
#define STORAGEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>

class StorageModel;

class StorageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StorageWidget(QWidget *parent = nullptr);
    
    void refresh();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onTableDoubleClicked(const QModelIndex& index);

private:
    void setupUi();
    int getSelectedId();
    
    QTableView *m_tableView;
    StorageModel *m_model;
    
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // STORAGEWIDGET_H
