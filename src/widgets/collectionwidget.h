#ifndef COLLECTIONWIDGET_H
#define COLLECTIONWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>

class CollectionModel;

class CollectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CollectionWidget(QWidget *parent = nullptr);
    
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
    CollectionModel *m_model;
    
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // COLLECTIONWIDGET_H
