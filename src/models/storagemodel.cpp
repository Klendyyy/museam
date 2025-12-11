#include "storagemodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDebug>

StorageModel::StorageModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void StorageModel::setupModel()
{
    setTable("storages");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    setRelation(ResponsibleEmployeeId, QSqlRelation("employees", "id", "last_name"));
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(Name, Qt::Horizontal, tr("Название"));
    setHeaderData(Location, Qt::Horizontal, tr("Расположение"));
    setHeaderData(StorageType, Qt::Horizontal, tr("Тип хранилища"));
    setHeaderData(Capacity, Qt::Horizontal, tr("Вместимость"));
    setHeaderData(ClimateControl, Qt::Horizontal, tr("Климат-контроль"));
    setHeaderData(TemperatureMin, Qt::Horizontal, tr("Мин. темп."));
    setHeaderData(TemperatureMax, Qt::Horizontal, tr("Макс. темп."));
    setHeaderData(HumidityMin, Qt::Horizontal, tr("Мин. влажность"));
    setHeaderData(HumidityMax, Qt::Horizontal, tr("Макс. влажность"));
    setHeaderData(ResponsibleEmployeeId, Qt::Horizontal, tr("Ответственный"));
    setHeaderData(Description, Qt::Horizontal, tr("Описание"));
    setHeaderData(IsActive, Qt::Horizontal, tr("Активно"));
    
    setFilter("is_active = 1");
    select();
}

void StorageModel::refresh()
{
    select();
    emit dataChanged();
}

bool StorageModel::addStorage(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        INSERT INTO storages (name, location, storage_type, capacity, climate_control,
                              temperature_min, temperature_max, humidity_min, humidity_max,
                              responsible_employee_id, description, is_active)
        VALUES (:name, :location, :storage_type, :capacity, :climate_control,
                :temperature_min, :temperature_max, :humidity_min, :humidity_max,
                :responsible_employee_id, :description, 1)
    )");
    
    query.bindValue(":name", data.value("name"));
    query.bindValue(":location", data.value("location"));
    query.bindValue(":storage_type", data.value("storage_type"));
    query.bindValue(":capacity", data.value("capacity", 0));
    query.bindValue(":climate_control", data.value("climate_control", false).toBool() ? 1 : 0);
    query.bindValue(":temperature_min", data.value("temperature_min"));
    query.bindValue(":temperature_max", data.value("temperature_max"));
    query.bindValue(":humidity_min", data.value("humidity_min"));
    query.bindValue(":humidity_max", data.value("humidity_max"));
    query.bindValue(":responsible_employee_id", data.value("responsible_employee_id").isNull() 
                    ? QVariant() : data.value("responsible_employee_id"));
    query.bindValue(":description", data.value("description"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления хранилища:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool StorageModel::updateStorage(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE storages SET
            name = :name,
            location = :location,
            storage_type = :storage_type,
            capacity = :capacity,
            climate_control = :climate_control,
            temperature_min = :temperature_min,
            temperature_max = :temperature_max,
            humidity_min = :humidity_min,
            humidity_max = :humidity_max,
            responsible_employee_id = :responsible_employee_id,
            description = :description
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":name", data.value("name"));
    query.bindValue(":location", data.value("location"));
    query.bindValue(":storage_type", data.value("storage_type"));
    query.bindValue(":capacity", data.value("capacity"));
    query.bindValue(":climate_control", data.value("climate_control").toBool() ? 1 : 0);
    query.bindValue(":temperature_min", data.value("temperature_min"));
    query.bindValue(":temperature_max", data.value("temperature_max"));
    query.bindValue(":humidity_min", data.value("humidity_min"));
    query.bindValue(":humidity_max", data.value("humidity_max"));
    query.bindValue(":responsible_employee_id", data.value("responsible_employee_id").isNull() 
                    ? QVariant() : data.value("responsible_employee_id"));
    query.bindValue(":description", data.value("description"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления хранилища:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool StorageModel::deleteStorage(int id)
{
    // Проверяем, есть ли экспонаты в хранилище
    if (getExhibitCount(id) > 0) {
        qWarning() << "Невозможно удалить хранилище: в нём есть экспонаты";
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE storages SET is_active = 0 WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления хранилища:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

QVariantMap StorageModel::getStorageById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM storages WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

int StorageModel::getExhibitCount(int storageId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM exhibits WHERE storage_id = :id");
    query.bindValue(":id", storageId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int StorageModel::getAvailableCapacity(int storageId)
{
    QVariantMap storage = getStorageById(storageId);
    int capacity = storage.value("capacity", 0).toInt();
    int used = getExhibitCount(storageId);
    return capacity - used;
}

QStringList StorageModel::getStorageTypes() const
{
    return QStringList{
        "Основное хранилище",
        "Запасник",
        "Выставочный зал",
        "Реставрационная мастерская",
        "Временное хранилище",
        "Депозитарий"
    };
}
