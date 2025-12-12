#ifndef STORAGEMODEL_H
#define STORAGEMODEL_H

#include <QSqlQueryModel>
#include <QVariantMap>
#include <QStringList>

class StorageModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit StorageModel(QObject *parent = nullptr);

    enum Columns {
        Id = 0,
        Name = 1,
        Location = 2,
        StorageType = 3,
        Capacity = 4,
        ClimateControl = 5,
        TemperatureMin = 6,
        TemperatureMax = 7,
        HumidityMin = 8,
        HumidityMax = 9,
        ResponsibleEmployeeId = 10,
        ResponsibleName = 11,
        Description = 12,
        IsActive = 13
    };

    void refresh();

    bool addStorage(const QVariantMap& data);
    bool updateStorage(int id, const QVariantMap& data);
    bool deleteStorage(int id);

    QVariantMap getStorageById(int id);
    int getExhibitCount(int storageId);
    int getAvailableCapacity(int storageId);
    QStringList getStorageTypes() const;

signals:
    void dataChanged();
};

#endif // STORAGEMODEL_H
