#ifndef RESTORATIONMODEL_H
#define RESTORATIONMODEL_H

#include <QSqlRelationalTableModel>

class RestorationModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit RestorationModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        ExhibitId,
        RestorationTypeId,
        StartDate,
        EndDate,
        RestorerId,
        Status,
        ConditionBefore,
        ConditionAfter,
        WorkDescription,
        MaterialsUsed,
        Cost,
        Notes,
        CreatedAt,
        UpdatedAt
    };
    
    void setupModel();
    void refresh();
    
    bool addRestoration(const QVariantMap& data);
    bool updateRestoration(int id, const QVariantMap& data);
    bool completeRestoration(int id, const QString& conditionAfter);
    bool deleteRestoration(int id);
    
    QVariantMap getRestorationById(int id);
    QList<QVariantMap> getExhibitRestorations(int exhibitId);
    QStringList getStatusList() const;
    
    void setFilterByStatus(const QString& status);
    void setFilterByExhibit(int exhibitId);
    
signals:
    void dataChanged();
};

#endif // RESTORATIONMODEL_H
