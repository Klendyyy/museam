#ifndef EXHIBITIONMODEL_H
#define EXHIBITIONMODEL_H

#include <QSqlQueryModel>
#include <QVariantMap>
#include <QStringList>
#include <QDate>

class ExhibitionModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit ExhibitionModel(QObject *parent = nullptr);

    enum Columns {
        Id = 0,
        Name = 1,
        Description = 2,
        StartDate = 3,
        EndDate = 4,
        Location = 5,
        CuratorId = 6,
        CuratorName = 7,
        Status = 8,
        VisitorCount = 9,
        Budget = 10,
        IsPermanent = 11
    };

    void refresh();

    bool addExhibition(const QVariantMap& data);
    bool updateExhibition(int id, const QVariantMap& data);
    bool deleteExhibition(int id);

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
