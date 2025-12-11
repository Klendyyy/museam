#include "collectionmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDebug>

CollectionModel::CollectionModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void CollectionModel::setupModel()
{
    setTable("collections");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Связь с таблицей сотрудников для куратора
    setRelation(CuratorId, QSqlRelation("employees", "id", "last_name"));
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(Name, Qt::Horizontal, tr("Название"));
    setHeaderData(Description, Qt::Horizontal, tr("Описание"));
    setHeaderData(CuratorId, Qt::Horizontal, tr("Куратор"));
    setHeaderData(CreationDate, Qt::Horizontal, tr("Дата создания"));
    setHeaderData(IsPermanent, Qt::Horizontal, tr("Постоянная"));
    setHeaderData(IsActive, Qt::Horizontal, tr("Активна"));
    
    setFilter("is_active = 1");
    select();
}

void CollectionModel::refresh()
{
    select();
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
    query.bindValue(":curator_id", data.value("curator_id").isNull() ? QVariant() : data.value("curator_id"));
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
            is_permanent = :is_permanent,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":curator_id", data.value("curator_id").isNull() ? QVariant() : data.value("curator_id"));
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
    // Мягкое удаление
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE collections SET is_active = 0, updated_at = CURRENT_TIMESTAMP WHERE id = :id");
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
