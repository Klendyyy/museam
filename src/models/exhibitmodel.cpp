#include "exhibitmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

ExhibitModel::ExhibitModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    refresh();
}

void ExhibitModel::refresh()
{
    QString sql = R"(
        SELECT
            e.id,
            e.inventory_number AS 'Инв. номер',
            e.name AS 'Название',
            IFNULL(c.name, '-') AS 'Категория',
            IFNULL(col.name, '-') AS 'Коллекция',
            IFNULL(s.name, '-') AS 'Хранилище',
            IFNULL(e.author, '-') AS 'Автор',
            IFNULL(e.creation_date, '-') AS 'Дата создания',
            e.`condition` AS 'Состояние',
            e.status AS 'Статус'
        FROM exhibits e
        LEFT JOIN categories c ON e.category_id = c.id
        LEFT JOIN collections col ON e.collection_id = col.id
        LEFT JOIN storages s ON e.storage_id = s.id
        ORDER BY e.id DESC
    )";

    setQuery(sql, DatabaseManager::instance().database());

    if (lastError().isValid()) {
        qWarning() << "ExhibitModel error:" << lastError().text();
    } else {
        qDebug() << "ExhibitModel loaded rows:" << rowCount();
    }
}

bool ExhibitModel::addExhibit(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        INSERT INTO exhibits (
            inventory_number, name, description, category_id, collection_id,
            storage_id, author, creation_date, creation_place, acquisition_date,
            material, technique, dimensions, weight, insurance_value,
            `condition`, status, is_on_display, requires_restoration, notes
        ) VALUES (
            ?, ?, ?, ?, ?,
            ?, ?, ?, ?, ?,
            ?, ?, ?, ?, ?,
            ?, ?, ?, ?, ?
        )
    )");

    query.addBindValue(data.value("inventory_number"));
    query.addBindValue(data.value("name"));
    query.addBindValue(data.value("description"));
    query.addBindValue(data.value("category_id").toInt() > 0 ? data.value("category_id") : QVariant());
    query.addBindValue(data.value("collection_id").toInt() > 0 ? data.value("collection_id") : QVariant());
    query.addBindValue(data.value("storage_id").toInt() > 0 ? data.value("storage_id") : QVariant());
    query.addBindValue(data.value("author"));
    query.addBindValue(data.value("creation_date"));
    query.addBindValue(data.value("creation_place"));
    query.addBindValue(data.value("acquisition_date"));
    query.addBindValue(data.value("material"));
    query.addBindValue(data.value("technique"));
    query.addBindValue(data.value("dimensions"));
    query.addBindValue(data.value("weight").toDouble());
    query.addBindValue(data.value("insurance_value").toDouble());
    query.addBindValue(data.value("condition", "Хорошее"));
    query.addBindValue(data.value("status", "В хранении"));
    query.addBindValue(data.value("is_on_display", false).toBool() ? 1 : 0);
    query.addBindValue(data.value("requires_restoration", false).toBool() ? 1 : 0);
    query.addBindValue(data.value("notes"));

    if (!query.exec()) {
        qWarning() << "Add exhibit error:" << query.lastError().text();
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
            inventory_number = ?,
            name = ?,
            description = ?,
            category_id = ?,
            collection_id = ?,
            storage_id = ?,
            author = ?,
            creation_date = ?,
            creation_place = ?,
            acquisition_date = ?,
            material = ?,
            technique = ?,
            dimensions = ?,
            weight = ?,
            insurance_value = ?,
            `condition` = ?,
            status = ?,
            is_on_display = ?,
            requires_restoration = ?,
            notes = ?
        WHERE id = ?
    )");

    query.addBindValue(data.value("inventory_number"));
    query.addBindValue(data.value("name"));
    query.addBindValue(data.value("description"));
    query.addBindValue(data.value("category_id").toInt() > 0 ? data.value("category_id") : QVariant());
    query.addBindValue(data.value("collection_id").toInt() > 0 ? data.value("collection_id") : QVariant());
    query.addBindValue(data.value("storage_id").toInt() > 0 ? data.value("storage_id") : QVariant());
    query.addBindValue(data.value("author"));
    query.addBindValue(data.value("creation_date"));
    query.addBindValue(data.value("creation_place"));
    query.addBindValue(data.value("acquisition_date"));
    query.addBindValue(data.value("material"));
    query.addBindValue(data.value("technique"));
    query.addBindValue(data.value("dimensions"));
    query.addBindValue(data.value("weight").toDouble());
    query.addBindValue(data.value("insurance_value").toDouble());
    query.addBindValue(data.value("condition"));
    query.addBindValue(data.value("status"));
    query.addBindValue(data.value("is_on_display").toBool() ? 1 : 0);
    query.addBindValue(data.value("requires_restoration").toBool() ? 1 : 0);
    query.addBindValue(data.value("notes"));
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Update exhibit error:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool ExhibitModel::deleteExhibit(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM exhibits WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Delete exhibit error:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

void ExhibitModel::setFilterByCategory(int categoryId)
{
    QString sql = R"(
        SELECT
            e.id,
            e.inventory_number AS 'Инв. номер',
            e.name AS 'Название',
            IFNULL(c.name, '-') AS 'Категория',
            IFNULL(col.name, '-') AS 'Коллекция',
            IFNULL(s.name, '-') AS 'Хранилище',
            IFNULL(e.author, '-') AS 'Автор',
            IFNULL(e.creation_date, '-') AS 'Дата создания',
            e.`condition` AS 'Состояние',
            e.status AS 'Статус'
        FROM exhibits e
        LEFT JOIN categories c ON e.category_id = c.id
        LEFT JOIN collections col ON e.collection_id = col.id
        LEFT JOIN storages s ON e.storage_id = s.id
    )";

    if (categoryId > 0) {
        sql += QString(" WHERE e.category_id = %1").arg(categoryId);
    }
    sql += " ORDER BY e.id DESC";

    setQuery(sql, DatabaseManager::instance().database());
}

void ExhibitModel::setFilterByStatus(const QString& status)
{
    QString sql = R"(
        SELECT
            e.id,
            e.inventory_number AS 'Инв. номер',
            e.name AS 'Название',
            IFNULL(c.name, '-') AS 'Категория',
            IFNULL(col.name, '-') AS 'Коллекция',
            IFNULL(s.name, '-') AS 'Хранилище',
            IFNULL(e.author, '-') AS 'Автор',
            IFNULL(e.creation_date, '-') AS 'Дата создания',
            e.`condition` AS 'Состояние',
            e.status AS 'Статус'
        FROM exhibits e
        LEFT JOIN categories c ON e.category_id = c.id
        LEFT JOIN collections col ON e.collection_id = col.id
        LEFT JOIN storages s ON e.storage_id = s.id
    )";

    if (!status.isEmpty()) {
        QString escaped = status;
        escaped.replace("'", "''");
        sql += QString(" WHERE e.status = '%1'").arg(escaped);
    }
    sql += " ORDER BY e.id DESC";

    setQuery(sql, DatabaseManager::instance().database());
}

void ExhibitModel::setSearchFilter(const QString& searchText)
{
    QString sql = R"(
        SELECT
            e.id,
            e.inventory_number AS 'Инв. номер',
            e.name AS 'Название',
            IFNULL(c.name, '-') AS 'Категория',
            IFNULL(col.name, '-') AS 'Коллекция',
            IFNULL(s.name, '-') AS 'Хранилище',
            IFNULL(e.author, '-') AS 'Автор',
            IFNULL(e.creation_date, '-') AS 'Дата создания',
            e.`condition` AS 'Состояние',
            e.status AS 'Статус'
        FROM exhibits e
        LEFT JOIN categories c ON e.category_id = c.id
        LEFT JOIN collections col ON e.collection_id = col.id
        LEFT JOIN storages s ON e.storage_id = s.id
    )";

    if (!searchText.isEmpty()) {
        QString escaped = searchText;
        escaped.replace("'", "''");
        sql += QString(" WHERE e.name LIKE '%%1%' OR e.inventory_number LIKE '%%1%' OR e.author LIKE '%%1%'").arg(escaped);
    }
    sql += " ORDER BY e.id DESC";

    setQuery(sql, DatabaseManager::instance().database());
}

void ExhibitModel::setFilterByCollection(int collectionId)
{
    Q_UNUSED(collectionId)
    refresh();
}

void ExhibitModel::setFilterByStorage(int storageId)
{
    Q_UNUSED(storageId)
    refresh();
}

void ExhibitModel::clearFilters()
{
    refresh();
}

QVariantMap ExhibitModel::getExhibitById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM exhibits WHERE id = ?");
    query.addBindValue(id);

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
