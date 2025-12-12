#include "exhibitionmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDate>
#include <QDebug>

ExhibitionModel::ExhibitionModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    refresh();
}

void ExhibitionModel::refresh()
{
    setQuery(R"(
        SELECT
            ex.id,
            ex.name,
            ex.description,
            ex.start_date,
            ex.end_date,
            ex.location,
            ex.curator_id,
            CONCAT(e.last_name, ' ', e.first_name) as curator_name,
            ex.status,
            ex.visitor_count,
            ex.budget,
            ex.is_permanent
        FROM exhibitions ex
        LEFT JOIN employees e ON ex.curator_id = e.id
        ORDER BY ex.start_date DESC
    )", DatabaseManager::instance().database());

    if (lastError().isValid()) {
        qWarning() << "ExhibitionModel query error:" << lastError().text();
    }

    setHeaderData(0, Qt::Horizontal, tr("ID"));
    setHeaderData(1, Qt::Horizontal, tr("Название"));
    setHeaderData(2, Qt::Horizontal, tr("Описание"));
    setHeaderData(3, Qt::Horizontal, tr("Начало"));
    setHeaderData(4, Qt::Horizontal, tr("Окончание"));
    setHeaderData(5, Qt::Horizontal, tr("Место"));
    setHeaderData(6, Qt::Horizontal, tr("ID куратора"));
    setHeaderData(7, Qt::Horizontal, tr("Куратор"));
    setHeaderData(8, Qt::Horizontal, tr("Статус"));
    setHeaderData(9, Qt::Horizontal, tr("Посетителей"));
    setHeaderData(10, Qt::Horizontal, tr("Бюджет"));
    setHeaderData(11, Qt::Horizontal, tr("Постоянная"));

    emit dataChanged();
}

bool ExhibitionModel::addExhibition(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        INSERT INTO exhibitions (name, description, start_date, end_date, location,
                                 curator_id, status, visitor_count, budget, is_permanent)
        VALUES (:name, :description, :start_date, :end_date, :location,
                :curator_id, :status, :visitor_count, :budget, :is_permanent)
    )");

    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":start_date", data.value("start_date"));
    query.bindValue(":end_date", data.value("end_date"));
    query.bindValue(":location", data.value("location"));
    query.bindValue(":curator_id", data.value("curator_id").isNull() || !data.value("curator_id").isValid() ? QVariant(QMetaType::fromType<int>()) : data.value("curator_id"));
    query.bindValue(":status", data.value("status", "Планируется"));
    query.bindValue(":visitor_count", data.value("visitor_count", 0));
    query.bindValue(":budget", data.value("budget", 0.0));
    query.bindValue(":is_permanent", data.value("is_permanent", false).toBool() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Ошибка добавления выставки:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool ExhibitionModel::updateExhibition(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        UPDATE exhibitions SET
            name = :name,
            description = :description,
            start_date = :start_date,
            end_date = :end_date,
            location = :location,
            curator_id = :curator_id,
            status = :status,
            visitor_count = :visitor_count,
            budget = :budget,
            is_permanent = :is_permanent
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":start_date", data.value("start_date"));
    query.bindValue(":end_date", data.value("end_date"));
    query.bindValue(":location", data.value("location"));
    query.bindValue(":curator_id", data.value("curator_id").isNull() || !data.value("curator_id").isValid() ? QVariant(QMetaType::fromType<int>()) : data.value("curator_id"));
    query.bindValue(":status", data.value("status"));
    query.bindValue(":visitor_count", data.value("visitor_count"));
    query.bindValue(":budget", data.value("budget"));
    query.bindValue(":is_permanent", data.value("is_permanent").toBool() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Ошибка обновления выставки:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool ExhibitionModel::deleteExhibition(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare("DELETE FROM exhibition_exhibits WHERE exhibition_id = :id");
    query.bindValue(":id", id);
    query.exec();

    query.prepare("DELETE FROM exhibitions WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "Ошибка удаления выставки:" << query.lastError().text();
        return false;
    }

    refresh();
    return true;
}

bool ExhibitionModel::addExhibitToExhibition(int exhibitionId, int exhibitId, const QString& location)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        INSERT IGNORE INTO exhibition_exhibits (exhibition_id, exhibit_id, display_location)
        VALUES (:exhibition_id, :exhibit_id, :location)
    )");

    query.bindValue(":exhibition_id", exhibitionId);
    query.bindValue(":exhibit_id", exhibitId);
    query.bindValue(":location", location);

    if (!query.exec()) {
        qWarning() << "Ошибка добавления экспоната на выставку:" << query.lastError().text();
        return false;
    }

    query.prepare("UPDATE exhibits SET is_on_display = 1, status = 'На выставке' WHERE id = :id");
    query.bindValue(":id", exhibitId);
    query.exec();

    return true;
}

bool ExhibitionModel::removeExhibitFromExhibition(int exhibitionId, int exhibitId)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        UPDATE exhibition_exhibits SET removed_date = CURDATE()
        WHERE exhibition_id = :exhibition_id AND exhibit_id = :exhibit_id
    )");

    query.bindValue(":exhibition_id", exhibitionId);
    query.bindValue(":exhibit_id", exhibitId);

    if (!query.exec()) {
        qWarning() << "Ошибка удаления экспоната с выставки:" << query.lastError().text();
        return false;
    }

    query.prepare(R"(
        SELECT COUNT(*) FROM exhibition_exhibits
        WHERE exhibit_id = :exhibit_id AND removed_date IS NULL
    )");
    query.bindValue(":exhibit_id", exhibitId);

    if (query.exec() && query.next() && query.value(0).toInt() == 0) {
        query.prepare("UPDATE exhibits SET is_on_display = 0, status = 'В хранении' WHERE id = :id");
        query.bindValue(":id", exhibitId);
        query.exec();
    }

    return true;
}

QList<int> ExhibitionModel::getExhibitionExhibits(int exhibitionId)
{
    QList<int> result;
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        SELECT exhibit_id FROM exhibition_exhibits
        WHERE exhibition_id = :id AND removed_date IS NULL
    )");
    query.bindValue(":id", exhibitionId);

    if (query.exec()) {
        while (query.next()) {
            result.append(query.value(0).toInt());
        }
    }

    return result;
}

QVariantMap ExhibitionModel::getExhibitionById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM exhibitions WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }

    return result;
}

QStringList ExhibitionModel::getStatusList() const
{
    return QStringList{
        "Планируется",
        "Активна",
        "Приостановлена",
        "Завершена",
        "Отменена"
    };
}

void ExhibitionModel::setFilterByStatus(const QString& status)
{
    QString sql = R"(
        SELECT
            ex.id, ex.name, ex.description, ex.start_date, ex.end_date,
            ex.location, ex.curator_id,
            CONCAT(e.last_name, ' ', e.first_name) as curator_name,
            ex.status, ex.visitor_count, ex.budget, ex.is_permanent
        FROM exhibitions ex
        LEFT JOIN employees e ON ex.curator_id = e.id
    )";

    if (!status.isEmpty() && status != "Все") {
        sql += QString(" WHERE ex.status = '%1'").arg(status);
    }
    sql += " ORDER BY ex.start_date DESC";

    setQuery(sql, DatabaseManager::instance().database());
}

void ExhibitionModel::setFilterByDateRange(const QDate& startDate, const QDate& endDate)
{
    QString sql = R"(
        SELECT
            ex.id, ex.name, ex.description, ex.start_date, ex.end_date,
            ex.location, ex.curator_id,
            CONCAT(e.last_name, ' ', e.first_name) as curator_name,
            ex.status, ex.visitor_count, ex.budget, ex.is_permanent
        FROM exhibitions ex
        LEFT JOIN employees e ON ex.curator_id = e.id
    )";

    if (startDate.isValid() && endDate.isValid()) {
        sql += QString(" WHERE ex.start_date >= '%1' AND ex.start_date <= '%2'")
        .arg(startDate.toString("yyyy-MM-dd"))
            .arg(endDate.toString("yyyy-MM-dd"));
    }
    sql += " ORDER BY ex.start_date DESC";

    setQuery(sql, DatabaseManager::instance().database());
}
