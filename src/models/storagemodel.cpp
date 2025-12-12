#include "storagemodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

StorageModel::StorageModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    refresh();
}

void StorageModel::refresh()
{
    setQuery(R"(
        SELECT
            s.id,
            s.name,
            s.location,
            s.storage_type,
            s.capacity,
            s.climate_control,
            s.temperature_min,
            s.temperature_max,
            s.humidity_min,
            s.humidity_max,
            s.responsible_employee_id,
            CONCAT(e.last_name, ' ', e.first_name) as responsible_name,
            s.description,
            s.is_active
        FROM storages s
        LEFT JOIN employees e ON s.responsible_employee_id = e.id
        WHERE s.is_active = 1
        ORDER BY s.name
    )", DatabaseManager::instance().database());

    if (lastError().isValid()) {
        qWarning() << "StorageModel query error:" << lastError().text();
    }

    setHeaderData(0, Qt::Horizontal, tr("ID"));
    setHeaderData(1, Qt::Horizontal, tr("Название"));
    setHeaderData(2, Qt::Horizontal, tr("Расположение"));
    setHeaderData(3, Qt::Horizontal, tr("Тип"));
    setHeaderData(4, Qt::Horizontal, tr("Вместимость"));
    setHeaderData(5, Qt::Horizontal, tr("Климат"));
    setHeaderData(6, Qt::Horizontal, tr("Мин. темп."));
    setHeaderData(7, Qt::Horizontal, tr("Макс. темп."));
    setHeaderData(8, Qt::Horizontal, tr("Мин. влажн."));
    setHeaderData(9, Qt::Horizontal, tr("Макс. влажн."));
    setHeaderData(10, Qt::Horizontal, tr("ID ответств."));
    setHeaderData(11, Qt::Horizontal, tr("Ответственный"));
    setHeaderData(12, Qt::Horizontal, tr("Описание"));
    setHeaderData(13, Qt::Horizontal, tr("Активно"));

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
    query.bindValue(":responsible_employee_id", data.value("responsible_employee_id").isNull() || !data.value("responsible_employee_id").isValid()
                                                    ? QVariant(QMetaType::fromType<int>()) : data.value("responsible_employee_id"));
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
    query.bindValue(":responsible_employee_id", data.value("responsible_employee_id").isNull() || !data.value("responsible_employee_id").isValid()
                                                    ? QVariant(QMetaType::fromType<int>()) : data.value("responsible_employee_id"));
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
