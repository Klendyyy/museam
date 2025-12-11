#include "restorationmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDate>
#include <QDebug>

RestorationModel::RestorationModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void RestorationModel::setupModel()
{
    setTable("restorations");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    setRelation(ExhibitId, QSqlRelation("exhibits", "id", "name"));
    setRelation(RestorationTypeId, QSqlRelation("restoration_types", "id", "name"));
    setRelation(RestorerId, QSqlRelation("employees", "id", "last_name"));
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(ExhibitId, Qt::Horizontal, tr("Экспонат"));
    setHeaderData(RestorationTypeId, Qt::Horizontal, tr("Тип реставрации"));
    setHeaderData(StartDate, Qt::Horizontal, tr("Дата начала"));
    setHeaderData(EndDate, Qt::Horizontal, tr("Дата окончания"));
    setHeaderData(RestorerId, Qt::Horizontal, tr("Реставратор"));
    setHeaderData(Status, Qt::Horizontal, tr("Статус"));
    setHeaderData(ConditionBefore, Qt::Horizontal, tr("Состояние до"));
    setHeaderData(ConditionAfter, Qt::Horizontal, tr("Состояние после"));
    setHeaderData(WorkDescription, Qt::Horizontal, tr("Описание работ"));
    setHeaderData(Cost, Qt::Horizontal, tr("Стоимость"));
    
    select();
}

void RestorationModel::refresh()
{
    select();
    emit dataChanged();
}

bool RestorationModel::addRestoration(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        INSERT INTO restorations (exhibit_id, restoration_type_id, start_date, restorer_id,
                                  status, condition_before, work_description, materials_used, cost, notes)
        VALUES (:exhibit_id, :restoration_type_id, :start_date, :restorer_id,
                :status, :condition_before, :work_description, :materials_used, :cost, :notes)
    )");
    
    query.bindValue(":exhibit_id", data.value("exhibit_id"));
    query.bindValue(":restoration_type_id", data.value("restoration_type_id").isNull() 
                    ? QVariant() : data.value("restoration_type_id"));
    query.bindValue(":start_date", data.value("start_date", QDate::currentDate().toString("yyyy-MM-dd")));
    query.bindValue(":restorer_id", data.value("restorer_id").isNull() 
                    ? QVariant() : data.value("restorer_id"));
    query.bindValue(":status", data.value("status", "В процессе"));
    query.bindValue(":condition_before", data.value("condition_before"));
    query.bindValue(":work_description", data.value("work_description"));
    query.bindValue(":materials_used", data.value("materials_used"));
    query.bindValue(":cost", data.value("cost", 0.0));
    query.bindValue(":notes", data.value("notes"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления реставрации:" << query.lastError().text();
        return false;
    }
    
    // Обновляем статус экспоната
    int exhibitId = data.value("exhibit_id").toInt();
    query.prepare("UPDATE exhibits SET status = 'На реставрации', requires_restoration = 1 WHERE id = :id");
    query.bindValue(":id", exhibitId);
    query.exec();
    
    refresh();
    return true;
}

bool RestorationModel::updateRestoration(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE restorations SET
            restoration_type_id = :restoration_type_id,
            start_date = :start_date,
            end_date = :end_date,
            restorer_id = :restorer_id,
            status = :status,
            condition_before = :condition_before,
            condition_after = :condition_after,
            work_description = :work_description,
            materials_used = :materials_used,
            cost = :cost,
            notes = :notes,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":restoration_type_id", data.value("restoration_type_id").isNull() 
                    ? QVariant() : data.value("restoration_type_id"));
    query.bindValue(":start_date", data.value("start_date"));
    query.bindValue(":end_date", data.value("end_date"));
    query.bindValue(":restorer_id", data.value("restorer_id").isNull() 
                    ? QVariant() : data.value("restorer_id"));
    query.bindValue(":status", data.value("status"));
    query.bindValue(":condition_before", data.value("condition_before"));
    query.bindValue(":condition_after", data.value("condition_after"));
    query.bindValue(":work_description", data.value("work_description"));
    query.bindValue(":materials_used", data.value("materials_used"));
    query.bindValue(":cost", data.value("cost"));
    query.bindValue(":notes", data.value("notes"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления реставрации:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool RestorationModel::completeRestoration(int id, const QString& conditionAfter)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    // Получаем exhibit_id
    query.prepare("SELECT exhibit_id FROM restorations WHERE id = :id");
    query.bindValue(":id", id);
    
    int exhibitId = 0;
    if (query.exec() && query.next()) {
        exhibitId = query.value(0).toInt();
    }
    
    // Обновляем реставрацию
    query.prepare(R"(
        UPDATE restorations SET
            status = 'Завершена',
            end_date = CURRENT_DATE,
            condition_after = :condition_after,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":condition_after", conditionAfter);
    
    if (!query.exec()) {
        qWarning() << "Ошибка завершения реставрации:" << query.lastError().text();
        return false;
    }
    
    // Обновляем статус экспоната
    if (exhibitId > 0) {
        query.prepare("UPDATE exhibits SET status = 'В хранении', requires_restoration = 0 WHERE id = :id");
        query.bindValue(":id", exhibitId);
        query.exec();
    }
    
    refresh();
    return true;
}

bool RestorationModel::deleteRestoration(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM restorations WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления реставрации:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

QVariantMap RestorationModel::getRestorationById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM restorations WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

QList<QVariantMap> RestorationModel::getExhibitRestorations(int exhibitId)
{
    QList<QVariantMap> result;
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        SELECT r.*, rt.name as type_name, 
               e.last_name || ' ' || e.first_name as restorer_name
        FROM restorations r
        LEFT JOIN restoration_types rt ON r.restoration_type_id = rt.id
        LEFT JOIN employees e ON r.restorer_id = e.id
        WHERE r.exhibit_id = :exhibit_id
        ORDER BY r.start_date DESC
    )");
    query.bindValue(":exhibit_id", exhibitId);
    
    if (query.exec()) {
        while (query.next()) {
            QVariantMap item;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); ++i) {
                item[record.fieldName(i)] = query.value(i);
            }
            result.append(item);
        }
    }
    
    return result;
}

QStringList RestorationModel::getStatusList() const
{
    return QStringList{
        "В процессе",
        "Приостановлена",
        "Завершена",
        "Отменена"
    };
}

void RestorationModel::setFilterByStatus(const QString& status)
{
    if (!status.isEmpty() && status != "Все") {
        setFilter(QString("restorations.status = '%1'").arg(status));
    } else {
        setFilter("");
    }
    select();
}

void RestorationModel::setFilterByExhibit(int exhibitId)
{
    if (exhibitId > 0) {
        setFilter(QString("restorations.exhibit_id = %1").arg(exhibitId));
    } else {
        setFilter("");
    }
    select();
}
