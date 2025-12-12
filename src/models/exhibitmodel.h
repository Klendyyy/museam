#ifndef EXHIBITMODEL_H
#define EXHIBITMODEL_H

#include <QSqlQueryModel>
#include <QVariantMap>

class ExhibitModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit ExhibitModel(QObject *parent = nullptr);

    enum Columns {
        Id = 0,
        InventoryNumber = 1,
        Name = 2,
        Description = 3,
        CategoryId = 4,
        CategoryName = 5,
        CollectionId = 6,
        CollectionName = 7,
        StorageId = 8,
        StorageName = 9,
        Author = 10,
        CreationDate = 11,
        CreationPlace = 12,
        AcquisitionDate = 13,
        Material = 14,
        Technique = 15,
        Dimensions = 16,
        Weight = 17,
        InsuranceValue = 18,
        Condition = 19,
        Status = 20,
        IsOnDisplay = 21,
        RequiresRestoration = 22,
        Notes = 23,
        CreatedAt = 24,
        UpdatedAt = 25
    };

    void refresh();

    bool addExhibit(const QVariantMap& data);
    bool updateExhibit(int id, const QVariantMap& data);
    bool deleteExhibit(int id);

    void setFilterByCategory(int categoryId);
    void setFilterByCollection(int collectionId);
    void setFilterByStorage(int storageId);
    void setFilterByStatus(const QString& status);
    void setSearchFilter(const QString& searchText);
    void clearFilters();

    QVariantMap getExhibitById(int id);
    QStringList getStatusList() const;
    QStringList getConditionList() const;

signals:
    void dataChanged();
};

#endif // EXHIBITMODEL_H
