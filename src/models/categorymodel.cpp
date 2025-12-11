#include "categorymodel.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

CategoryModel::CategoryModel(QObject *parent)
    : QSqlTableModel(parent, DatabaseManager::instance().database())
{
    setupModel();
}

void CategoryModel::setupModel()
{
    setTable("categories");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    setHeaderData(Id, Qt::Horizontal, tr("ID"));
    setHeaderData(Name, Qt::Horizontal, tr("Название"));
    setHeaderData(Description, Qt::Horizontal, tr("Описание"));
    setHeaderData(ParentId, Qt::Horizontal, tr("Родительская категория"));
    
    select();
}

void CategoryModel::refresh()
{
    select();
    emit dataChanged();
}

bool CategoryModel::addCategory(const QString& name, const QString& description, int parentId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        INSERT INTO categories (name, description, parent_id)
        VALUES (:name, :description, :parent_id)
    )");
    
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":parent_id", parentId > 0 ? parentId : QVariant());
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления категории:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool CategoryModel::updateCategory(int id, const QString& name, const QString& description, int parentId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.prepare(R"(
        UPDATE categories SET
            name = :name,
            description = :description,
            parent_id = :parent_id
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":parent_id", parentId > 0 ? parentId : QVariant());
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления категории:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool CategoryModel::deleteCategory(int id)
{
    // Проверяем, есть ли экспонаты в категории
    if (getExhibitCount(id) > 0) {
        qWarning() << "Невозможно удалить категорию: в ней есть экспонаты";
        return false;
    }
    
    // Проверяем подкатегории
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM categories WHERE parent_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        qWarning() << "Невозможно удалить категорию: есть подкатегории";
        return false;
    }
    
    query.prepare("DELETE FROM categories WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления категории:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

QVariantMap CategoryModel::getCategoryById(int id)
{
    QVariantMap result;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM categories WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = query.value(i);
        }
    }
    
    return result;
}

QList<QPair<int, QString>> CategoryModel::getAllCategories()
{
    QList<QPair<int, QString>> result;
    QSqlQuery query(DatabaseManager::instance().database());
    
    query.exec("SELECT id, name FROM categories ORDER BY name");
    
    while (query.next()) {
        result.append({query.value(0).toInt(), query.value(1).toString()});
    }
    
    return result;
}

QList<QPair<int, QString>> CategoryModel::getSubcategories(int parentId)
{
    QList<QPair<int, QString>> result;
    QSqlQuery query(DatabaseManager::instance().database());
    
    if (parentId > 0) {
        query.prepare("SELECT id, name FROM categories WHERE parent_id = :parent_id ORDER BY name");
        query.bindValue(":parent_id", parentId);
    } else {
        query.prepare("SELECT id, name FROM categories WHERE parent_id IS NULL ORDER BY name");
    }
    
    if (query.exec()) {
        while (query.next()) {
            result.append({query.value(0).toInt(), query.value(1).toString()});
        }
    }
    
    return result;
}

int CategoryModel::getExhibitCount(int categoryId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM exhibits WHERE category_id = :id");
    query.bindValue(":id", categoryId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
