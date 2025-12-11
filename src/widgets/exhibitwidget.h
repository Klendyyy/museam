#ifndef EXHIBITWIDGET_H
#define EXHIBITWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSortFilterProxyModel>

class ExhibitModel;

class ExhibitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExhibitWidget(QWidget *parent = nullptr);
    
    void refresh();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onViewClicked();
    void onSearchTextChanged(const QString& text);
    void onCategoryFilterChanged(int index);
    void onStatusFilterChanged(int index);
    void onTableDoubleClicked(const QModelIndex& index);

private:
    void setupUi();
    void loadFilters();
    int getSelectedId();
    
    QTableView *m_tableView;
    ExhibitModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    
    QLineEdit *m_searchEdit;
    QComboBox *m_categoryCombo;
    QComboBox *m_statusCombo;
    
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_viewButton;
};

#endif // EXHIBITWIDGET_H
