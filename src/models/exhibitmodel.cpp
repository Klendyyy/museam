#include "exhibitmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDateTime>
#include <QDebug>

ExhibitModel::ExhibitModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void ExhibitModel::setupModel()
{
    setTable("exhibits");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Устанавливаем связи с другими таблицами
    setRelation(CategoryId, QSqlRelation("categories", "id", "name"));
    setRelation(CollectionId, QSqlRelation("collections", "id", "name"));
    setRelation(StorageId, QSqlRelation("storages", "id", "name"));
    
    // Устанавливаем заголовки колонок
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(InventoryNumber, Qt::Horizontal, tr("Инв. номер"));
    setHeaderData(Name, Qt::Horizontal, tr("Название"));
    setHeaderData(Description, Qt::Horizontal, tr("Описание"));
    setHeaderData(CategoryId, Qt::Horizontal, tr("Категория"));
    setHeaderData(CollectionId, Qt::Horizontal, tr("Коллекция"));
    setHeaderData(StorageId, Qt::Horizontal, tr("Хранилище"));
    setHeaderData(Author, Qt::Horizontal, tr("Автор"));
    setHeaderData(CreationDate, Qt::Horizontal, tr("Дата создания"));
    setHeaderData(CreationPlace, Qt::Horizontal, tr("Место создания"));
    setHeaderData(AcquisitionDate, Qt::Horizontal, tr("Дата поступления"));
    setHeaderData(Material, Qt::Horizontal, tr("Материал"));
    setHeaderData(Technique, Qt::Horizontal, tr("Техника"));
    setHeaderData(Dimensions, Qt::Horizontal, tr("Размеры"));
    setHeaderData(Weight, Qt::Horizontal, tr("Вес (кг)"));
    setHeaderData(InsuranceValue, Qt::Horizontal, tr("Страховая стоимость"));
    setHeaderData(Condition, Qt::Horizontal, tr("Состояние"));
    setHeaderData(Status, Qt::Horizontal, tr("Статус"));
    setHeaderData(IsOnDisplay, Qt::Horizontal, tr("На выставке"));
    setHeaderData(RequiresRestoration, Qt::Horizontal, tr("Требует реставрации"));
    setHeaderData(Notes, Qt::Horizontal, tr("Примечания"));
    
    select();
}

void ExhibitModel::refresh()
{
    select();
    emit dataChanged();
}

bool ExhibitModel::addExhibit(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        INSERT INTO exhibits (
            inventory_number, name, description, category_id, collection_id,
            storage_id, author, creation_date, creation_place, acquisition_date,
            material, technique, dimensions, weight, insurance_value,
            condition, status, is_on_display, requires_restoration, notes
        ) VALUES (
            :inventory_number, :name, :description, :category_id, :collection_id,
            :storage_id, :author, :creation_date, :creation_place, :acquisition_date,
            :material, :technique, :dimensions, :weight, :insurance_value,
            :condition, :status, :is_on_display, :requires_restoration, :notes
        )
    )");
    
    query.bindValue(":inventory_number", data.value("inventory_number"));
    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":category_id", data.value("category_id").isNull() ? QVariant() : data.value("category_id"));
    query.bindValue(":collection_id", data.value("collection_id").isNull() ? QVariant() : data.value("collection_id"));
    query.bindValue(":storage_id", data.value("storage_id").isNull() ? QVariant() : data.value("storage_id"));
    query.bindValue(":author", data.value("author"));
    query.bindValue(":creation_date", data.value("creation_date"));
    query.bindValue(":creation_place", data.value("creation_place"));
    query.bindValue(":acquisition_date", data.value("acquisition_date"));
    query.bindValue(":material", data.value("material"));
    query.bindValue(":technique", data.value("technique"));
    query.bindValue(":dimensions", data.value("dimensions"));
    query.bindValue(":weight", data.value("weight").toDouble());
    query.bindValue(":insurance_value", data.value("insurance_value").toDouble());
    query.bindValue(":condition", data.value("condition", "Хорошее"));
    query.bindValue(":status", data.value("status", "В хранении"));
    query.bindValue(":is_on_display", data.value("is_on_display", false).toBool() ? 1 : 0);
    query.bindValue(":requires_restoration", data.value("requires_restoration", false).toBool() ? 1 : 0);
    query.bindValue(":notes", data.value("notes"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления экспоната:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool ExhibitModel::updateExhibit(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE exhibits SET
            inventory_number = :inventory_number,
            name = :name,
            description = :description,
            category_id = :category_id,
            collection_id = :collection_id,
            storage_id = :storage_id,
            author = :author,
            creation_date = :creation_date,
            creation_place = :creation_place,
            acquisition_date = :acquisition_date,
            material = :material,
            technique = :technique,
            dimensions = :dimensions,
            weight = :weight,
            insurance_value = :insurance_value,
            condition = :condition,
            status = :status,
            is_on_display = :is_on_display,
            requires_restoration = :requires_restoration,
            notes = :notes,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":inventory_number", data.value("inventory_number"));
    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":category_id", data.value("category_id").isNull() ? QVariant() : data.value("category_id"));
    query.bindValue(":collection_id", data.value("collection_id").isNull() ? QVariant() : data.value("collection_id"));
    query.bindValue(":storage_id", data.value("storage_id").isNull() ? QVariant() : data.value("storage_id"));
    query.bindValue(":author", data.value("author"));
    query.bindValue(":creation_date", data.value("creation_date"));
    query.bindValue(":creation_place", data.value("creation_place"));
    query.bindValue(":acquisition_date", data.value("acquisition_date"));
    query.bindValue(":material", data.value("material"));
    query.bindValue(":technique", data.value("technique"));
    query.bindValue(":dimensions", data.value("dimensions"));
    query.bindValue(":weight", data.value("weight").toDouble());
    query.bindValue(":insurance_value", data.value("insurance_value").toDouble());
    query.bindValue(":condition", data.value("condition"));
    query.bindValue(":status", data.value("status"));
    query.bindValue(":is_on_display", data.value("is_on_display").toBool() ? 1 : 0);
    query.bindValue(":requires_restoration", data.value("requires_restoration").toBool() ? 1 : 0);
    query.bindValue(":notes", data.value("notes"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления экспоната:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool ExhibitModel::deleteExhibit(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM exhibits WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления экспоната:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

void ExhibitModel::setFilterByCategory(int categoryId)
{
    if (categoryId > 0) {
        setFilter(QString("exhibits.category_id = %1").arg(categoryId));
    } else {
        setFilter("");
    }
    select();
}

void ExhibitModel::setFilterByCollection(int collectionId)
{
    if (collectionId > 0) {
        setFilter(QString("exhibits.collection_id = %1").arg(collectionId));
    } else {
        setFilter("");
    }
    select();
}

void ExhibitModel::setFilterByStorage(int storageId)
{
    if (storageId > 0) {
        setFilter(QString("exhibits.storage_id = %1").arg(storageId));
    } else {
        setFilter("");
    }
    select();
}

void ExhibitModel::setFilterByStatus(const QString& status)
{
    if (!status.isEmpty() && status != "Все") {
        setFilter(QString("exhibits.status = '%1'").arg(status));
    } else {
        setFilter("");
    }
    select();
}

void ExhibitModel::setSearchFilter(const QString& searchText)
{
    if (!searchText.isEmpty()) {
        QString filter = QString(
            "exhibits.name LIKE '%%1%' OR "
            "exhibits.inventory_number LIKE '%%1%' OR "
            "exhibits.author LIKE '%%1%' OR "
            "exhibits.description LIKE '%%1%'"
        ).arg(searchText);
        setFilter(filter);
    } else {
        setFilter("");
    }
    select();
}

void ExhibitModel::clearFilters()
{
    setFilter("");
    select();
}

QVariantMap ExhibitModel::getExhibitById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM exhibits WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

QStringList ExhibitModel::getStatusList() const
{
    return QStringList{
        "В хранении",
        "На выставке",
        "На реставрации",
        "Во временном хранении",
        "На экспертизе",
        "Утрачен",
        "Списан"
    };
}

QStringList ExhibitModel::getConditionList() const
{
    return QStringList{
        "Отличное",
        "Хорошее",
        "Удовлетворительное",
        "Требует реставрации",
        "Аварийное"
    };
}
