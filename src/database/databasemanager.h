#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();

    bool initialize(const QString& host,
                    int port,
                    const QString& database,
                    const QString& user,
                    const QString& password);

    bool isOpen() const;
    void close();

    QSqlDatabase& database();

    bool executeQuery(const QString& query);
    QSqlQuery execSelect(const QString& query);

    int getExhibitCount();
    int getCollectionCount();
    int getExhibitionCount();
    int getEmployeeCount();
    int getExhibitsInRestoration();
    int getExhibitsOnDisplay();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_database;
    bool m_isInitialized;
};

#endif // DATABASEMANAGER_H
