#ifndef EXHIBITMODEL_H
#define EXHIBITMODEL_H

#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

/**
 * @brief Модель данных для экспонатов
 * 
 * Использует QSqlRelationalTableModel для отображения связанных данных
 * из таблиц categories, collections и storages
 */
class ExhibitModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit ExhibitModel(QObject *parent = nullptr);
    
    // Перечисление колонок для удобства доступа
    enum Columns {
        Id = 0,
        InventoryNumber,
        Name,
        Description,
        CategoryId,
        CollectionId,
        StorageId,
        Author,
        CreationDate,
        CreationPlace,
        AcquisitionDate,
        Material,
        Technique,
        Dimensions,
        Weight,
        InsuranceValue,
        Condition,
        Status,
        IsOnDisplay,
        RequiresRestoration,
        Notes,
        CreatedAt,
        UpdatedAt
    };
    
    void setupModel();
    void refresh();
    
    // CRUD операции
    bool addExhibit(const QVariantMap& data);
    bool updateExhibit(int id, const QVariantMap& data);
    bool deleteExhibit(int id);
    
    // Поиск и фильтрация
    void setFilterByCategory(int categoryId);
    void setFilterByCollection(int collectionId);
    void setFilterByStorage(int storageId);
    void setFilterByStatus(const QString& status);
    void setSearchFilter(const QString& searchText);
    void clearFilters();
    
    // Получение данных
    QVariantMap getExhibitById(int id);
    QStringList getStatusList() const;
    QStringList getConditionList() const;
    
signals:
    void dataChanged();
    
private:
    QString m_baseFilter;
    void applyFilter(const QString& filter);
};

#endif // EXHIBITMODEL_H
