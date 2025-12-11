#ifndef ACQUISITIONMODEL_H
#define ACQUISITIONMODEL_H

#include <QSqlRelationalTableModel>

class AcquisitionModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit AcquisitionModel(QObject *parent = nullptr);
    
    enum Columns {
        Id = 0,
        ExhibitId,
        AcquisitionDate,
        SourceId,
        SourceDetails,
        DocumentNumber,
        DocumentDate,
        Price,
        ResponsibleEmployeeId,
        Notes,
        CreatedAt
    };
    
    void setupModel();
    void refresh();
    
    bool addAcquisition(const QVariantMap& data);
    bool updateAcquisition(int id, const QVariantMap& data);
    bool deleteAcquisition(int id);
    
    QVariantMap getAcquisitionById(int id);
    QVariantMap getAcquisitionByExhibit(int exhibitId);
    
signals:
    void dataChanged();
};

#endif // ACQUISITIONMODEL_H
