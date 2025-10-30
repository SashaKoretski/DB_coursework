#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager& getInstance();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool initialize(const QString& host = "localhost",
                    int port = 5432,
                    const QString& dbname = "tube_deformation",
                    const QString& user = "postgres",
                    const QString& password = "12345");

    bool isConnected() const;
    void disconnect();

    bool execute(const QString& query);
    QSqlQuery executeQuery(const QString& query);

    QString getLastError() const;

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    int64_t getLastInsertedId();

    QString escapeString(const QString& str) const;
    bool checkDatabaseStructure();

    ~DatabaseManager();

    bool clearAllTables();

private:
    DatabaseManager();

    QSqlDatabase db;
    QString lastError;
    bool inTransaction;

    void setLastError(const QString& error);
    bool checkConnection();
};

#endif
