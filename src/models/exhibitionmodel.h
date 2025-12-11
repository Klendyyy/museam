#ifndef EXHIBITIONMODEL_H
#define EXHIBITIONMODEL_H

#include <QSqlRelationalTableModel>

class ExhibitionModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit ExhibitionModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        Name,
        Description,
        StartDate,
        EndDate,
        Location,
        CuratorId,
        Status,
        VisitorCount,
        Budget,
        IsPermanent,
        CreatedAt,
        UpdatedAt
    };
    
    void setupModel();
    void refresh();
    
    bool addExhibition(const QVariantMap& data);
    bool updateExhibition(int id, const QVariantMap& data);
    bool deleteExhibition(int id);
    
    // Управление экспонатами на выставке
    bool addExhibitToExhibition(int exhibitionId, int exhibitId, const QString& location = "");
    bool removeExhibitFromExhibition(int exhibitionId, int exhibitId);
    QList<int> getExhibitionExhibits(int exhibitionId);
    
    QVariantMap getExhibitionById(int id);
    QStringList getStatusList() const;
    
    void setFilterByStatus(const QString& status);
    void setFilterByDateRange(const QDate& startDate, const QDate& endDate);
    
signals:
    void dataChanged();
};

#endif // EXHIBITIONMODEL_H
