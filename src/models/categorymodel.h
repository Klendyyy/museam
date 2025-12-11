#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QSqlTableModel>
#include <QList>
#include <QPair>

class CategoryModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit CategoryModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        Name,
        Description,
        ParentId,
        CreatedAt
    };
    
    void setupModel();
    void refresh();
    
    bool addCategory(const QString& name, const QString& description, int parentId = 0);
    bool updateCategory(int id, const QString& name, const QString& description, int parentId = 0);
    bool deleteCategory(int id);
    
    QVariantMap getCategoryById(int id);
    QList<QPair<int, QString>> getAllCategories();
    QList<QPair<int, QString>> getSubcategories(int parentId);
    int getExhibitCount(int categoryId);
    
signals:
    void dataChanged();
};

#endif // CATEGORYMODEL_H
