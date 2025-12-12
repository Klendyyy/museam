#ifndef COLLECTIONMODEL_H
#define COLLECTIONMODEL_H

#include <QSqlQueryModel>
#include <QVariantMap>

class CollectionModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit CollectionModel(QObject *parent = nullptr);

    enum Columns {
        Id = 0,
        Name = 1,
        Description = 2,
        CuratorId = 3,
        CuratorName = 4,
        CreationDate = 5,
        IsPermanent = 6,
        IsActive = 7
    };

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
