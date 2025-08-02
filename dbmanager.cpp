#include "dbmanager.h"
#include <QFile>
DBManager *DBManager::m_instance = nullptr;
QMutex DBManager::m_mutex;

DBManager::DBManager(QObject *parent) : QObject(parent) {
  openDB("./database.db");
  createTablesFromFile("./schema/databaseTemplate.sql");
}

DBManager *DBManager::instance() {
  if (!m_instance) {
    QMutexLocker locker(&m_mutex);
    if (!m_instance) {
      m_instance = new DBManager();
    }
  }
  return m_instance;
}
bool DBManager::createTablesFromFile(const QString &sqlFilePath) {
  if (!m_db.isOpen()) {
    qDebug() << "Database is not open!";
    return false;
  }

  QFile file(sqlFilePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open SQL file:" << sqlFilePath;
    return false;
  }

  QTextStream in(&file);
  QString sqlContent = in.readAll();
  file.close();

  // Split by semicolons to execute individual statements
  QStringList queries = sqlContent.split(';', Qt::SkipEmptyParts);

  QSqlQuery query;
  for (const QString &q : queries) {
    QString trimmed = q.trimmed();
    if (trimmed.isEmpty())
      continue;
    if (!query.exec(trimmed)) {
      qDebug() << "SQL execution error:" << query.lastError().text()
               << "\nQuery:" << trimmed;
      return false;
    }
  }

  return true;
}

bool DBManager::openDB(const QString &path) {
  if (m_db.isOpen())
    return true;

  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(path);

  if (!m_db.open()) {
    qDebug() << "DB open error:" << m_db.lastError().text();
    return false;
  }
  return true;
}

void DBManager::closeDB() {
  if (m_db.isOpen())
    m_db.close();
}

/* ================== NOTES ================== */
int DBManager::addNote(const QString &title) {
  QSqlQuery query;
  query.prepare("INSERT INTO Notes (title) VALUES (:title)");
  query.bindValue(":title", title);
  if (!query.exec()) {
    qDebug() << "Add note error:" << query.lastError().text();
    return -1;
  }
  return query.lastInsertId().toInt();
}

bool DBManager::updateNoteTitle(int noteId, const QString &newTitle) {
  QSqlQuery query;
  query.prepare("UPDATE Notes SET title = :title WHERE note_id = :id");
  query.bindValue(":title", newTitle);
  query.bindValue(":id", noteId);
  return query.exec();
}

QList<QVariantMap> DBManager::getAllNotes() {
  QList<QVariantMap> notes;
  QSqlQuery query("SELECT * FROM Notes ORDER BY created_at DESC");
  while (query.next()) {
    QVariantMap note;
    note["note_id"] = query.value("note_id");
    note["title"] = query.value("title");
    note["created_at"] = query.value("created_at");
    notes.append(note);
  }
  return notes;
}

bool DBManager::deleteNote(int noteId) {
  QSqlQuery query;
  query.prepare("DELETE FROM Notes WHERE note_id = :id");
  query.bindValue(":id", noteId);
  return query.exec();
}

/* ================== NOTES CONTENT ================== */
int DBManager::addNoteContent(int noteId, const QString &content) {
  QSqlQuery query;
  query.prepare("INSERT INTO NotesContents (note_id, content) VALUES "
                "(:note_id, :content)");
  query.bindValue(":note_id", noteId);
  query.bindValue(":content", content);
  if (!query.exec()) {
    qDebug() << "Add note content error:" << query.lastError().text();
    return -1;
  }
  return query.lastInsertId().toInt();
}

bool DBManager::updateNoteContent(int contentId, bool completed) {
  QSqlQuery query;
  query.prepare("UPDATE NotesContents SET completed = "
                ":completed WHERE id = :id");
  query.bindValue(":completed", completed);
  query.bindValue(":id", contentId);
  return query.exec();
}

QList<QVariantMap> DBManager::getNoteContents(int noteId) {
  QList<QVariantMap> contents;
  QSqlQuery query;
  query.prepare("SELECT * FROM NotesContents WHERE note_id = :note_id ORDER BY "
                "created_at ASC");
  query.bindValue(":note_id", noteId);
  query.exec();
  while (query.next()) {
    QVariantMap content;
    content["id"] = query.value("id");
    content["note_id"] = query.value("note_id");
    content["content"] = query.value("content");
    content["completed"] = query.value("completed");
    content["created_at"] = query.value("created_at");
    contents.append(content);
  }
  return contents;
}

QString DBManager::getNoteName(int noteId) {
  QString name;
  QSqlQuery query;
  query.prepare("SELECT title FROM Notes WHERE note_id = :note_id");
  query.bindValue(":note_id", noteId);
  query.exec();
  while (query.next()) {
    name = query.value("title").toString();
  }
  return name;
}

bool DBManager::deleteNoteContent(int contentId) {
  QSqlQuery query;
  query.prepare("DELETE FROM NotesContents WHERE id = :id");
  query.bindValue(":id", contentId);
  return query.exec();
}

bool DBManager::deleteAllNoteContents(int noteID) {
  QSqlQuery query;
  query.prepare("DELETE FROM NotesContents WHERE note_id = :id");
  query.bindValue(":id", noteID);
  return query.exec();
}
/* ================== EVENT LOGS ================== */
int DBManager::addEventLog(const QString &eventType,
                           const QString &eventDescription) {
  QSqlQuery query;
  query.prepare("INSERT INTO eventLogs (event_type, event_description) VALUES "
                "(:type, :desc)");
  query.bindValue(":type", eventType);
  query.bindValue(":desc", eventDescription);
  if (!query.exec()) {
    qDebug() << "Add log error:" << query.lastError().text();
    return -1;
  }
  return query.lastInsertId().toInt();
}

QList<QVariantMap> DBManager::getEventLogs() {
  QList<QVariantMap> logs;
  QSqlQuery query("SELECT * FROM eventLogs ORDER BY created_at DESC");
  while (query.next()) {
    QVariantMap log;
    log["id"] = query.value("id");
    log["event_type"] = query.value("event_type");
    log["event_description"] = query.value("event_description");
    log["created_at"] = query.value("created_at");
    logs.append(log);
  }
  return logs;
}

DBManager::~DBManager() { closeDB(); }
