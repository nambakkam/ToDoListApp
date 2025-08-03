#include "dbmanager.h"
#include <QFile>
DBManager *DBManager::m_instance = nullptr;
QMutex DBManager::m_mutex;

DBManager::DBManager(QObject *parent) : QObject(parent) {
  openDB("./database.db");
  createTablesFromFile("./schema/databaseTemplate.sql");
}

/**
 * @brief Returns the singleton instance of DBManager.
 *
 * This method implements the double-checked locking pattern to ensure that
 * only one instance of DBManager is created in a thread-safe manner.
 * If the instance does not exist, it acquires a mutex lock and creates
 * the instance.
 *
 * @return Pointer to the singleton DBManager instance.
 */
DBManager *DBManager::instance() {
  if (!m_instance) {
    QMutexLocker locker(&m_mutex);
    if (!m_instance) {
      m_instance = new DBManager();
    }
  }
  return m_instance;
}
/**
 * @brief Creates database tables by executing SQL statements from a file.
 *
 * This function reads the specified SQL file, splits its contents into individual
 * SQL statements using semicolons as delimiters, and executes each statement
 * sequentially on the currently open database connection.
 *
 * @param sqlFilePath The path to the SQL file containing table creation statements.
 * @return true if all SQL statements are executed successfully; false otherwise.
 *
 * @note The database connection must be open before calling this function.
 *       If any statement fails to execute, the function returns false and logs the error.
 */
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

/**
 * @brief Opens a SQLite database at the specified path.
 *
 * This function attempts to open a SQLite database using the provided file path.
 * If the database is already open, it returns true immediately.
 * If the database cannot be opened, it logs the error and returns false.
 *
 * @param path The file path to the SQLite database.
 * @return true if the database is successfully opened or already open, false otherwise.
 */
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

/**
 * @brief Closes the database connection if it is currently open.
 *
 * This function checks if the database is open and closes it to release resources.
 */
void DBManager::closeDB() {
  if (m_db.isOpen())
    m_db.close();
}

/* ================== NOTES ================== */
/**
 * @brief Adds a new note to the database.
 *
 * Inserts a note with the specified title into the Notes table.
 * If the insertion is successful, returns the ID of the newly inserted note.
 * If an error occurs during insertion, logs the error and returns -1.
 *
 * @param title The title of the note to be added.
 * @return int The ID of the newly inserted note, or -1 if an error occurred.
 */
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

/**
 * @brief Updates the title of a note in the database.
 *
 * This method updates the title of the note identified by the given noteId
 * in the Notes table. The new title is provided as a QString.
 *
 * @param noteId The unique identifier of the note to update.
 * @param newTitle The new title to set for the note.
 * @return true if the update was successful, false otherwise.
 */
bool DBManager::updateNoteTitle(int noteId, const QString &newTitle) {
  QSqlQuery query;
  query.prepare("UPDATE Notes SET title = :title WHERE note_id = :id");
  query.bindValue(":title", newTitle);
  query.bindValue(":id", noteId);
  return query.exec();
}

/**
 * @brief Retrieves all notes from the database.
 *
 * Executes a SQL query to select all records from the Notes table,
 * ordered by the creation date in descending order. Each note is
 * represented as a QVariantMap containing the note's ID, title,
 * and creation timestamp.
 *
 * @return QList<QVariantMap> A list of notes, where each note is a QVariantMap
 *         with keys "note_id", "title", and "created_at".
 */
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

/**
 * @brief Deletes a note from the database by its ID.
 *
 * This function prepares and executes a SQL DELETE statement to remove
 * the note with the specified noteId from the Notes table.
 *
 * @param noteId The unique identifier of the note to be deleted.
 * @return true if the note was successfully deleted; false otherwise.
 */
bool DBManager::deleteNote(int noteId) {
  QSqlQuery query;
  query.prepare("DELETE FROM Notes WHERE note_id = :id");
  query.bindValue(":id", noteId);
  return query.exec();
}

/* ================== NOTES CONTENT ================== */
/**
 * @brief Adds content to a note in the database.
 *
 * Inserts a new entry into the NotesContents table with the specified note ID and content.
 *
 * @param noteId The ID of the note to which the content will be added.
 * @param content The content to be added to the note.
 * @return The ID of the newly inserted note content on success, or -1 if the operation fails.
 */
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

/**
 * @brief Updates the completion status of a note content in the database.
 *
 * This function sets the 'completed' field for the note content with the specified ID.
 *
 * @param contentId The unique identifier of the note content to update.
 * @param completed The new completion status to set (true for completed, false for not completed).
 * @return true if the update was successful, false otherwise.
 */
bool DBManager::updateNoteContent(int contentId, bool completed) {
  QSqlQuery query;
  query.prepare("UPDATE NotesContents SET completed = "
                ":completed WHERE id = :id");
  query.bindValue(":completed", completed);
  query.bindValue(":id", contentId);
  return query.exec();
}

/**
 * @brief Retrieves the contents of a specific note from the database.
 *
 * This function queries the NotesContents table for all entries associated with the given
 * note ID, ordered by their creation time in ascending order. Each entry is returned as a
 * QVariantMap containing the fields: id, note_id, content, completed, and created_at.
 *
 * @param noteId The ID of the note whose contents are to be retrieved.
 * @return QList<QVariantMap> A list of QVariantMap objects, each representing a content entry.
 */
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

/**
 * @brief Retrieves the title of a note from the database by its ID.
 *
 * This method executes a SQL query to fetch the title of the note
 * with the specified noteId from the Notes table.
 *
 * @param noteId The unique identifier of the note.
 * @return The title of the note as a QString. Returns an empty string if no note is found.
 */
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

/**
 * @brief Deletes a note content from the database by its ID.
 *
 * This function prepares and executes a SQL DELETE statement to remove
 * the note content with the specified ID from the NotesContents table.
 *
 * @param contentId The unique identifier of the note content to delete.
 * @return true if the deletion was successful, false otherwise.
 */
bool DBManager::deleteNoteContent(int contentId) {
  QSqlQuery query;
  query.prepare("DELETE FROM NotesContents WHERE id = :id");
  query.bindValue(":id", contentId);
  return query.exec();
}

/**
 * @brief Deletes all contents associated with a specific note from the NotesContents table.
 *
 * This function executes a SQL DELETE statement to remove all rows in the NotesContents table
 * that are linked to the provided note ID.
 *
 * @param noteID The ID of the note whose contents should be deleted.
 * @return true if the deletion was successful, false otherwise.
 */
bool DBManager::deleteAllNoteContents(int noteID) {
  QSqlQuery query;
  query.prepare("DELETE FROM NotesContents WHERE note_id = :id");
  query.bindValue(":id", noteID);
  return query.exec();
}
/* ================== EVENT LOGS ================== */
/**
 * @brief Adds a new event log entry to the database.
 *
 * Inserts a record into the `eventLogs` table with the specified event type and description.
 *
 * @param eventType The type of the event to log.
 * @param eventDescription A description of the event.
 * @return The ID of the newly inserted event log entry on success, or -1 if the operation fails.
 */
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

/**
 * @brief Retrieves all event logs from the database.
 *
 * Executes a SQL query to select all records from the eventLogs table,
 * ordered by the creation timestamp in descending order. Each record is
 * mapped to a QVariantMap containing the following keys: "id", "event_type",
 * "event_description", and "created_at". The method returns a QList of these
 * QVariantMap objects.
 *
 * @return QList<QVariantMap> List of event logs, each represented as a QVariantMap.
 */
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
