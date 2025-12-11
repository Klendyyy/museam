#ifndef COLLECTIONMODEL_H
#define COLLECTIONMODEL_H

#include <QSqlRelationalTableModel>

class CollectionModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit CollectionModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        Name,
        Description,
        CuratorId,
        CreationDate,
        IsPermanent,
        IsActive,
        CreatedAt,
        UpdatedAt
    };
    
    void setupModel();
    void refresh();
    
    bool addCollection(const QVariantMap& data);
    bool updateCollection(int id, const QVariantMap& data);
    bool deleteCollection(int id);
    
    QVariantMap getCollectionById(int id);
    int getExhibitCount(int collectionId);
    
signals:
    void dataChanged();
};

#endif // COLLECTIONMODEL_H
