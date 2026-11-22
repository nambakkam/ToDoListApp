#ifndef TESTENV_H
#define TESTENV_H

#include "dbmanager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QtTest>

#ifndef TEST_SCHEMA_DIR
#error "TEST_SCHEMA_DIR must point at the repository schema/ folder"
#endif

/**
 * @class TestEnv
 * @brief Sandbox for DBManager: temp working directory plus schema copy.
 *
 * DBManager::instance() opens ./database.db and reads
 * ./schema/databaseTemplate.sql relative to the current directory and the
 * singleton cannot be reset. TestEnv therefore creates a QTemporaryDir,
 * copies the schema into it and makes it the current directory before the
 * first instance() call. Use one TestEnv per test process.
 */
class TestEnv {
public:
  /** @brief Enters the sandbox; returns false if setup failed. */
  bool setUp() {
    if (!m_dir.isValid())
      return false;
    m_previous = QDir::currentPath();
    const QString schemaDir = m_dir.path() + "/schema";
    if (!QDir().mkpath(schemaDir))
      return false;
    const QString src =
        QString::fromUtf8(TEST_SCHEMA_DIR) + "/databaseTemplate.sql";
    if (!QFile::copy(src, schemaDir + "/databaseTemplate.sql"))
      return false;
    if (!QDir::setCurrent(m_dir.path()))
      return false;
    DBManager::instance();
    return QFileInfo::exists(m_dir.path() + "/database.db");
  }

  /** @brief Closes the database and leaves the sandbox. */
  void tearDown() {
    DBManager::instance()->closeDB();
    QDir::setCurrent(m_previous);
  }

  /** @brief Absolute path of the sandbox directory. */
  QString path() const { return m_dir.path(); }

  /** @brief Empties all three tables so each test starts clean. */
  static void clearTables() {
    QSqlQuery q;
    q.exec("DELETE FROM Notes");
    q.exec("DELETE FROM NotesContents");
    q.exec("DELETE FROM eventLogs");
  }

  /** @brief All event log rows whose event_type equals @p type. */
  static QList<QVariantMap> logsOfType(const QString &type) {
    QList<QVariantMap> result;
    const auto all = DBManager::instance()->getEventLogs();
    for (const QVariantMap &row : all) {
      if (row.value("event_type").toString() == type)
        result.append(row);
    }
    return result;
  }

private:
  QTemporaryDir m_dir;
  QString m_previous;
};

#endif // TESTENV_H
