#ifndef STORAGEMODEL_H
#define STORAGEMODEL_H

#include <QSqlRelationalTableModel>

class StorageModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit StorageModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        Name,
        Location,
        StorageType,
        Capacity,
        ClimateControl,
        TemperatureMin,
        TemperatureMax,
        HumidityMin,
        HumidityMax,
        ResponsibleEmployeeId,
        Description,
        IsActive,
        CreatedAt
    };
    
    void setupModel();
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
