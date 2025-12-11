#ifndef EXHIBITIONWIDGET_H
#define EXHIBITIONWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QComboBox>
#include <QPushButton>

class ExhibitionModel;

class ExhibitionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExhibitionWidget(QWidget *parent = nullptr);
    
    void refresh();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onManageExhibitsClicked();
    void onStatusFilterChanged(int index);
    void onTableDoubleClicked(const QModelIndex& index);

private:
    void setupUi();
    int getSelectedId();
    
    QTableView *m_tableView;
    ExhibitionModel *m_model;
    QComboBox *m_statusCombo;
    
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_manageExhibitsButton;
};

#endif // EXHIBITIONWIDGET_H
