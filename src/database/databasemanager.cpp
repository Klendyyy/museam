#include "databasemanager.h"
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_isInitialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString& host, int port,
                                 const QString& database,
                                 const QString& user,
                                 const QString& password)
{
    if (m_isInitialized) {
        return true;
    }

    // Проверяем доступные драйверы
    qDebug() << "Доступные SQL драйверы:" << QSqlDatabase::drivers();

    m_database = QSqlDatabase::addDatabase("QMYSQL");
    m_database.setHostName(host);
    m_database.setPort(port);
    m_database.setDatabaseName(database);
    m_database.setUserName(user);
    m_database.setPassword(password);
    m_database.setConnectOptions("MYSQL_OPT_RECONNECT=1;MYSQL_OPT_CONNECT_TIMEOUT=5");

    if (!m_database.open()) {
        qCritical() << "Ошибка подключения к БД:" << m_database.lastError().text();
        qCritical() << "Драйвер:" << m_database.driverName();
        return false;
    }

    // Устанавливаем кодировку
    QSqlQuery query(m_database);
    query.exec("SET NAMES 'utf8mb4'");
    query.exec("SET CHARACTER SET utf8mb4");
    query.exec("SET character_set_connection=utf8mb4");

    m_isInitialized = true;
    qDebug() << "Успешное подключение к MariaDB";

    // Тестовый запрос
    query.exec("SELECT VERSION()");
    if (query.next()) {
        qDebug() << "Версия сервера:" << query.value(0).toString();
    }

    return true;
}

bool DatabaseManager::isOpen() const
{
    return m_database.isOpen();
}

void DatabaseManager::close()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    m_isInitialized = false;
}

QSqlDatabase& DatabaseManager::database()
{
    return m_database;
}

bool DatabaseManager::executeQuery(const QString& queryStr)
{
    QSqlQuery query(m_database);
    if (!query.exec(queryStr)) {
        qWarning() << "Ошибка запроса:" << query.lastError().text();
        qWarning() << "SQL:" << queryStr;
        return false;
    }
    return true;
}

QSqlQuery DatabaseManager::execSelect(const QString& queryStr)
{
    QSqlQuery query(m_database);
    if (!query.exec(queryStr)) {
        qWarning() << "Ошибка SELECT:" << query.lastError().text();
        qWarning() << "SQL:" << queryStr;
    }
    return query;
}

int DatabaseManager::getExhibitCount()
{
    QSqlQuery query = execSelect("SELECT COUNT(*) FROM exhibits");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getCollectionCount()
{
    QSqlQuery query = execSelect("SELECT COUNT(*) FROM collections WHERE is_active = 1");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getExhibitionCount()
{
    QSqlQuery query = execSelect("SELECT COUNT(*) FROM exhibitions WHERE status = 'Активна'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getEmployeeCount()
{
    QSqlQuery query = execSelect("SELECT COUNT(*) FROM employees WHERE is_active = 1");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getExhibitsInRestoration()
{
    QSqlQuery query = execSelect("SELECT COUNT(*) FROM restorations WHERE status = 'В процессе'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getExhibitsOnDisplay()
{
    QSqlQuery query = execSelect("SELECT COUNT(*) FROM exhibits WHERE is_on_display = 1");
    return query.next() ? query.value(0).toInt() : 0;
}
