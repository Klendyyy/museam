#ifndef REPORTSWIDGET_H
#define REPORTSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTableView>

class ReportsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(QWidget *parent = nullptr);

private slots:
    void onGenerateReport();
    void onExportReport();
    void onReportTypeChanged(int index);

private:
    void setupUi();
    
    void generateExhibitReport();
    void generateCollectionReport();
    void generateExhibitionReport();
    void generateRestorationReport();
    void generateStorageReport();
    void generateEmployeeReport();
    void generateStatisticsReport();
    
    QComboBox *m_reportTypeCombo;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QPushButton *m_generateButton;
    QPushButton *m_exportButton;
    QTextEdit *m_reportView;
    QTableView *m_tableView;
    
    QString m_currentReport;
};

#endif // REPORTSWIDGET_H
