#include "databasemanager.h"
#include <QSqlDriver>
#include <QStringList>

DatabaseManager::DatabaseManager()
    : inTransaction(false)
{
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString& host,
                                 int port,
                                 const QString& dbname,
                                 const QString& user,
                                 const QString& password)
{
    if (db.isOpen()) {
        disconnect();
    }

     
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(password);

    if (!db.open()) {
        setLastError(QString("Connection failed: %1").arg(db.lastError().text()));
        return false;
    }

    qDebug() << "Successfully connected to database:" << dbname;

     
    if (!checkDatabaseStructure()) {
        qWarning() << "Warning: Database structure check failed or incomplete";
    }

    return true;
}

bool DatabaseManager::isConnected() const
{
    return db.isOpen();
}

void DatabaseManager::disconnect()
{
    if (db.isOpen()) {
        if (inTransaction) {
            rollbackTransaction();
        }

        QString connectionName = db.connectionName();
        db.close();
        QSqlDatabase::removeDatabase(connectionName);

        qDebug() << "Database connection closed";
    }
}

bool DatabaseManager::execute(const QString& query)
{
    if (!checkConnection()) {
        setLastError("Not connected to database");
        return false;
    }

    QSqlQuery q(db);
    if (!q.exec(query)) {
        setLastError(QString("Query execution failed: %1\nQuery: %2")
                         .arg(q.lastError().text())
                         .arg(query));
        qDebug() << "Failed query:" << query;
        qDebug() << "Error:" << lastError;
        return false;
    }

    return true;
}

QSqlQuery DatabaseManager::executeQuery(const QString& query)
{
    QSqlQuery q(db);

    if (!checkConnection()) {
        setLastError("Not connected to database");
        return q;
    }

    if (!q.exec(query)) {
        setLastError(QString("Query execution failed: %1\nQuery: %2")
                         .arg(q.lastError().text())
                         .arg(query));
        qDebug() << "Failed query:" << query;
        qDebug() << "Error:" << lastError;
    }

    return q;
}

QString DatabaseManager::getLastError() const
{
    return lastError;
}

bool DatabaseManager::beginTransaction()
{
    if (!checkConnection()) {
        setLastError("Not connected to database");
        return false;
    }

    if (inTransaction) {
        qWarning() << "Transaction already in progress";
        return false;
    }

    if (db.transaction()) {
        inTransaction = true;
        qDebug() << "Transaction started";
        return true;
    }

    setLastError(db.lastError().text());
    return false;
}

bool DatabaseManager::commitTransaction()
{
    if (!checkConnection()) {
        setLastError("Not connected to database");
        return false;
    }

    if (!inTransaction) {
        qWarning() << "No transaction in progress";
        return false;
    }

    if (db.commit()) {
        inTransaction = false;
        qDebug() << "Transaction committed";
        return true;
    }

    setLastError(db.lastError().text());
    rollbackTransaction();
    return false;
}

bool DatabaseManager::rollbackTransaction()
{
    if (!checkConnection()) {
        setLastError("Not connected to database");
        return false;
    }

    if (!inTransaction) {
        qWarning() << "No transaction in progress";
        return false;
    }

    if (db.rollback()) {
        inTransaction = false;
        qDebug() << "Transaction rolled back";
        return true;
    }

    inTransaction = false;
    setLastError(db.lastError().text());
    return false;
}

int64_t DatabaseManager::getLastInsertedId()
{
    if (!checkConnection()) {
        return -1;
    }

    QSqlQuery q(db);
    if (q.exec("SELECT lastval()") && q.next()) {
        return q.value(0).toLongLong();
    }

    return -1;
}

QString DatabaseManager::escapeString(const QString& str) const
{
     
     
    QString escaped = str;
    escaped.replace("'", "''");
    return escaped;
}

bool DatabaseManager::checkDatabaseStructure()
{
    if (!checkConnection()) {
        return false;
    }

     
    QStringList tables = {"tube", "section", "segment", "point", "edge"};

    for (const QString& table : tables) {
        QString query = QString(
                            "SELECT EXISTS ("
                            "   SELECT FROM information_schema.tables "
                            "   WHERE table_schema = 'public' "
                            "   AND table_name = '%1'"
                            ")").arg(table);

        QSqlQuery q = executeQuery(query);
        if (!q.next() || !q.value(0).toBool()) {
            qWarning() << "Table does not exist:" << table;
            return false;
        }
    }

    qDebug() << "Database structure check passed";
    return true;
}

void DatabaseManager::setLastError(const QString& error)
{
    lastError = error;
    qDebug() << "Database error:" << error;
}

bool DatabaseManager::checkConnection()
{
    if (!db.isOpen()) {
        setLastError("Database connection is not open");
        return false;
    }

    return true;
}

bool DatabaseManager::clearAllTables()
{
    if (!checkConnection()) {
        setLastError("Not connected to database");
        return false;
    }

    qDebug() << "Starting database cleanup...";

    if (!beginTransaction()) {
        qDebug() << "Failed to start transaction for cleanup";
        return false;
    }

    QStringList deleteQueries = {
        "DELETE FROM point",
        "DELETE FROM section",
        "DELETE FROM edge",
        "DELETE FROM segment",
        "DELETE FROM tube"
    };

    for (const QString& query : deleteQueries) {
        if (!execute(query)) {
            qWarning() << "Failed to execute:" << query;
            rollbackTransaction();
            return false;
        }
        qDebug() << "Executed:" << query;
    }

    if (!commitTransaction()) {
        qDebug() << "Failed to commit cleanup transaction";
        return false;
    }

    qDebug() << "Database cleanup completed successfully";
    return true;
}
