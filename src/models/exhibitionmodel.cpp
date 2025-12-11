#include "exhibitionmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDate>
#include <QDebug>

ExhibitionModel::ExhibitionModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void ExhibitionModel::setupModel()
{
    setTable("exhibitions");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    setRelation(CuratorId, QSqlRelation("employees", "id", "last_name"));
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(Name, Qt::Horizontal, tr("Название"));
    setHeaderData(Description, Qt::Horizontal, tr("Описание"));
    setHeaderData(StartDate, Qt::Horizontal, tr("Дата начала"));
    setHeaderData(EndDate, Qt::Horizontal, tr("Дата окончания"));
    setHeaderData(Location, Qt::Horizontal, tr("Место проведения"));
    setHeaderData(CuratorId, Qt::Horizontal, tr("Куратор"));
    setHeaderData(Status, Qt::Horizontal, tr("Статус"));
    setHeaderData(VisitorCount, Qt::Horizontal, tr("Посетителей"));
    setHeaderData(Budget, Qt::Horizontal, tr("Бюджет"));
    setHeaderData(IsPermanent, Qt::Horizontal, tr("Постоянная"));
    
    select();
}

void ExhibitionModel::refresh()
{
    select();
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
    query.bindValue(":curator_id", data.value("curator_id").isNull() ? QVariant() : data.value("curator_id"));
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
            is_permanent = :is_permanent,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":name", data.value("name"));
    query.bindValue(":description", data.value("description"));
    query.bindValue(":start_date", data.value("start_date"));
    query.bindValue(":end_date", data.value("end_date"));
    query.bindValue(":location", data.value("location"));
    query.bindValue(":curator_id", data.value("curator_id").isNull() ? QVariant() : data.value("curator_id"));
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
    
    // Сначала удаляем связи с экспонатами
    query.prepare("DELETE FROM exhibition_exhibits WHERE exhibition_id = :id");
    query.bindValue(":id", id);
    query.exec();
    
    // Затем удаляем саму выставку
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
        INSERT OR IGNORE INTO exhibition_exhibits (exhibition_id, exhibit_id, display_location)
        VALUES (:exhibition_id, :exhibit_id, :location)
    )");
    
    query.bindValue(":exhibition_id", exhibitionId);
    query.bindValue(":exhibit_id", exhibitId);
    query.bindValue(":location", location);
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления экспоната на выставку:" << query.lastError().text();
        return false;
    }
    
    // Обновляем статус экспоната
    query.prepare("UPDATE exhibits SET is_on_display = 1, status = 'На выставке' WHERE id = :id");
    query.bindValue(":id", exhibitId);
    query.exec();
    
    return true;
}

bool ExhibitionModel::removeExhibitFromExhibition(int exhibitionId, int exhibitId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE exhibition_exhibits SET removed_date = CURRENT_DATE
        WHERE exhibition_id = :exhibition_id AND exhibit_id = :exhibit_id
    )");
    
    query.bindValue(":exhibition_id", exhibitionId);
    query.bindValue(":exhibit_id", exhibitId);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления экспоната с выставки:" << query.lastError().text();
        return false;
    }
    
    // Проверяем, есть ли экспонат на других выставках
    query.prepare(R"(
        SELECT COUNT(*) FROM exhibition_exhibits 
        WHERE exhibit_id = :exhibit_id AND removed_date IS NULL
    )");
    query.bindValue(":exhibit_id", exhibitId);
    
    if (query.exec() && query.next() && query.value(0).toInt() == 0) {
        // Если экспонат больше ни на одной выставке, меняем его статус
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
    if (!status.isEmpty() && status != "Все") {
        setFilter(QString("exhibitions.status = '%1'").arg(status));
    } else {
        setFilter("");
    }
    select();
}

void ExhibitionModel::setFilterByDateRange(const QDate& startDate, const QDate& endDate)
{
    QString filter;
    if (startDate.isValid() && endDate.isValid()) {
        filter = QString("exhibitions.start_date >= '%1' AND exhibitions.start_date <= '%2'")
                     .arg(startDate.toString("yyyy-MM-dd"))
                     .arg(endDate.toString("yyyy-MM-dd"));
    }
    setFilter(filter);
    select();
}
