#ifndef COLLECTIONWIDGET_H
#define COLLECTIONWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

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
    void updateStatistics();
    int getSelectedId();
    QFrame* createMiniStatCard(const QString& title, const QString& value, const QString& color);

    QTableView *m_tableView;
    CollectionModel *m_model;

    QLabel *m_countLabel;
    QLabel *m_totalExhibitsLabel;
    QLabel *m_permanentLabel;
    QLabel *m_temporaryLabel;

    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // COLLECTIONWIDGET_H




