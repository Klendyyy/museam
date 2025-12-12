#include "collectionmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

CollectionModel::CollectionModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    refresh();
}

void CollectionModel::refresh()
{
    setQuery(R"(
        SELECT
            c.id,
            c.name,
            c.description,
            c.curator_id,
            CONCAT(e.last_name, ' ', e.first_name) as curator_name,
            c.creation_date,
            c.is_permanent,
            c.is_active
        FROM collections c
        LEFT JOIN employees e ON c.curator_id = e.id
        WHERE c.is_active = 1
        ORDER BY c.name
    )", DatabaseManager::instance().database());

    if (lastError().isValid()) {
        qWarning() << "CollectionModel query error:" << lastError().text();
    }

    setHeaderData(0, Qt::Horizontal, tr("ID"));
    setHeaderData(1, Qt::Horizontal, tr("Название"));
    setHeaderData(2, Qt::Horizontal, tr("Описание"));
    setHeaderData(3, Qt::Horizontal, tr("ID куратора"));
    setHeaderData(4, Qt::Horizontal, tr("Куратор"));
    setHeaderData(5, Qt::Horizontal, tr("Дата создания"));
    setHeaderData(6, Qt::Horizontal, tr("Постоянная"));
    setHeaderData(7, Qt::Horizontal, tr("Активна"));

    emit dataChanged();
}

bool CollectionModel::addCollection(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        INSERT INTO collections (name, description, curator_id, creation_date, is_permanent, is_active)
        VALUES (:name, :description, :curator_id, :creation_date, :is_permanent, 1)
    )");

    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":curator_id", data.value("curator_id").isNull() || !data.value("curator_id").isValid() ? QVariant(QMetaType::fromType<int>()) : data.value("curator_id"));
    query.bindValue(":creation_date", data.value("creation_date"));
    query.bindValue(":is_permanent", data.value("is_permanent", true).toBool() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Ошибка добавления коллекции:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool CollectionModel::updateCollection(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        UPDATE collections SET
            name = :name,
            description = :description,
            curator_id = :curator_id,
            creation_date = :creation_date,
            is_permanent = :is_permanent
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":curator_id", data.value("curator_id").isNull() || !data.value("curator_id").isValid() ? QVariant(QMetaType::fromType<int>()) : data.value("curator_id"));
    query.bindValue(":creation_date", data.value("creation_date"));
    query.bindValue(":is_permanent", data.value("is_permanent").toBool() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Ошибка обновления коллекции:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool CollectionModel::deleteCollection(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE collections SET is_active = 0 WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "Ошибка удаления коллекции:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

QVariantMap CollectionModel::getCollectionById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM collections WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }

    return result;
}

int CollectionModel::getExhibitCount(int collectionId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM exhibits WHERE collection_id = :id");
    query.bindValue(":id", collectionId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
