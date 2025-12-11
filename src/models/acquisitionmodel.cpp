#include "acquisitionmodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QDebug>

AcquisitionModel::AcquisitionModel(QObject *parent)
    : QSqlRelationalTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void AcquisitionModel::setupModel()
{
    setTable("acquisitions");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    setRelation(ExhibitId, QSqlRelation("exhibits", "id", "name"));
    setRelation(SourceId, QSqlRelation("acquisition_sources", "id", "name"));
    setRelation(ResponsibleEmployeeId, QSqlRelation("employees", "id", "last_name"));
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(ExhibitId, Qt::Horizontal, tr("Экспонат"));
    setHeaderData(AcquisitionDate, Qt::Horizontal, tr("Дата поступления"));
    setHeaderData(SourceId, Qt::Horizontal, tr("Источник"));
    setHeaderData(SourceDetails, Qt::Horizontal, tr("Детали источника"));
    setHeaderData(DocumentNumber, Qt::Horizontal, tr("Номер документа"));
    setHeaderData(DocumentDate, Qt::Horizontal, tr("Дата документа"));
    setHeaderData(Price, Qt::Horizontal, tr("Стоимость"));
    setHeaderData(ResponsibleEmployeeId, Qt::Horizontal, tr("Ответственный"));
    setHeaderData(Notes, Qt::Horizontal, tr("Примечания"));
    
    select();
}

void AcquisitionModel::refresh()
{
    select();
    emit dataChanged();
}

bool AcquisitionModel::addAcquisition(const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        INSERT INTO acquisitions (exhibit_id, acquisition_date, source_id, source_details,
                                  document_number, document_date, price, responsible_employee_id, notes)
        VALUES (:exhibit_id, :acquisition_date, :source_id, :source_details,
                :document_number, :document_date, :price, :responsible_employee_id, :notes)
    )");
    
    query.bindValue(":exhibit_id", data.value("exhibit_id"));
    query.bindValue(":acquisition_date", data.value("acquisition_date"));
    query.bindValue(":source_id", data.value("source_id").isNull() ? QVariant() : data.value("source_id"));
    query.bindValue(":source_details", data.value("source_details"));
    query.bindValue(":document_number", data.value("document_number"));
    query.bindValue(":document_date", data.value("document_date"));
    query.bindValue(":price", data.value("price", 0.0));
    query.bindValue(":responsible_employee_id", data.value("responsible_employee_id").isNull() 
                    ? QVariant() : data.value("responsible_employee_id"));
    query.bindValue(":notes", data.value("notes"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления поступления:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool AcquisitionModel::updateAcquisition(int id, const QVariantMap& data)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE acquisitions SET
            acquisition_date = :acquisition_date,
            source_id = :source_id,
            source_details = :source_details,
            document_number = :document_number,
            document_date = :document_date,
            price = :price,
            responsible_employee_id = :responsible_employee_id,
            notes = :notes
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":acquisition_date", data.value("acquisition_date"));
    query.bindValue(":source_id", data.value("source_id").isNull() ? QVariant() : data.value("source_id"));
    query.bindValue(":source_details", data.value("source_details"));
    query.bindValue(":document_number", data.value("document_number"));
    query.bindValue(":document_date", data.value("document_date"));
    query.bindValue(":price", data.value("price"));
    query.bindValue(":responsible_employee_id", data.value("responsible_employee_id").isNull() 
                    ? QVariant() : data.value("responsible_employee_id"));
    query.bindValue(":notes", data.value("notes"));
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления поступления:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool AcquisitionModel::deleteAcquisition(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM acquisitions WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления поступления:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

QVariantMap AcquisitionModel::getAcquisitionById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM acquisitions WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

QVariantMap AcquisitionModel::getAcquisitionByExhibit(int exhibitId)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM acquisitions WHERE exhibit_id = :exhibit_id");
    query.bindValue(":exhibit_id", exhibitId);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}
