/**
 * @file dbmanager.h
 * @brief Declares DBManager, the singleton SQLite access layer.
 */
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QDebug>
#include <QMutex>
#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QtSql/QSqlDatabase>

/**
 * @class DBManager
 * @brief Singleton wrapper around the SQLite database used by the app.
 *
 * Provides CRUD operations for the three tables. In UI terms a "note" is a
 * list (`Notes` table) and a "note content" is a task (`NotesContents`
 * table); `eventLogs` holds the audit trail written by Logger.
 *
 * The constructor opens `./database.db` and runs
 * `./schema/databaseTemplate.sql`, both relative to the current working
 * directory. Only creation of the instance is guarded by a mutex; queries
 * use the default QSqlDatabase connection with no locking. Query failures
 * are mostly reported through qDebug() and a return value.
 *
 * @note Use DBManager::instance() to access the object.
 */
class DBManager : public QObject {
  Q_OBJECT
public:
  /**
   * @brief Returns the singleton, creating it on first use.
   * @return Pointer to the shared DBManager (never null).
   */
  static DBManager *instance();

  /**
   * @brief Opens the SQLite database at @p path.
   * @param path Path of the database file.
   * @return true if opened or already open, false on failure.
   */
  bool openDB(const QString &path);

  /** @brief Closes the database connection if it is open. */
  void closeDB();

  // Notes operations (lists)

  /**
   * @brief Inserts a list into the `Notes` table.
   * @param title Name of the list.
   * @return Id of the new row, or -1 on failure.
   */
  int addNote(const QString &title);

  /**
   * @brief Renames a list.
   * @param noteId Id of the list (`note_id`).
   * @param newTitle New name.
   * @return true if the statement executed.
   * @note Not called anywhere in the app at present.
   */
  bool updateNoteTitle(int noteId, const QString &newTitle);

  /**
   * @brief Reads all lists, newest first (`created_at DESC`).
   * @return One map per list with keys `note_id`, `title`, `created_at`.
   */
  QList<QVariantMap> getAllNotes();

  /**
   * @brief Deletes a list row from `Notes`.
   *
   * Its tasks in `NotesContents` are not touched.
   *
   * @param noteId Id of the list.
   * @return true if the statement executed (also when no row matched).
   */
  bool deleteNote(int noteId);

  // NotesContents operations (tasks)

  /**
   * @brief Inserts a task into the `NotesContents` table.
   * @param noteId Id of the list the task belongs to.
   * @param content Task text.
   * @return Id of the new row, or -1 on failure.
   */
  int addNoteContent(int noteId, const QString &content);

  /**
   * @brief Sets the completion flag of a task.
   * @param contentId Id of the task (`id` column, not a row index).
   * @param completed New completion state.
   * @return true if the statement executed.
   */
  bool updateNoteContent(int contentId, bool completed);

  /**
   * @brief Reads the tasks of a list, oldest first (`created_at ASC`).
   * @param noteId Id of the list.
   * @return One map per task with keys `id`, `note_id`, `content`,
   *         `completed`, `created_at`. Empty if none (or on query error,
   *         which is not reported).
   */
  QList<QVariantMap> getNoteContents(int noteId);

  /**
   * @brief Deletes one task.
   * @param contentId Id of the task (`id` column).
   * @return true if the statement executed.
   */
  bool deleteNoteContent(int contentId);

  // Event logs

  /**
   * @brief Inserts a row into `eventLogs`.
   * @param eventType Logger::EventType enumerator name as text.
   * @param eventDescription Compact JSON string built by Logger.
   * @return Id of the new row, or -1 on failure.
   */
  int addEventLog(const QString &eventType, const QString &eventDescription);

  /**
   * @brief Reads all audit log rows, newest first (`created_at DESC`).
   * @return One map per row with keys `id`, `event_type`,
   *         `event_description`, `created_at`.
   */
  QList<QVariantMap> getEventLogs();

  /** @brief Destructor; closes the database. */
  ~DBManager();

  /**
   * @brief Deletes every task of a list.
   *
   * Intended for list deletion, but currently never called, so deleting a
   * list leaves its tasks behind (known issue, see
   * TODONotesModel::removeNoteFromList()).
   *
   * @param noteID Id of the list.
   * @return true if the statement executed.
   */
  bool deleteAllNoteContents(int noteID);
public slots:
  /**
   * @brief Looks up the name of a list.
   * @param noteId Id of the list.
   * @return The title, or an empty string if no such list exists.
   */
  QString getNoteName(int noteId);
private slots:
  /**
   * @brief Executes the statements of an SQL file.
   *
   * The file is split on ';' and each piece is run in order; stops at the
   * first failure.
   *
   * @param sqlFilePath Path of the schema file.
   * @return true if every statement succeeded.
   */
  bool createTablesFromFile(const QString &sqlFilePath);

private:
  /**
   * @brief Private constructor (singleton).
   *
   * Opens `./database.db` and runs `./schema/databaseTemplate.sql`.
   *
   * @param parent Optional QObject parent.
   */
  explicit DBManager(QObject *parent = nullptr);

  /** @brief The singleton instance; null until first instance() call. */
  static DBManager *m_instance;

  /** @brief Guards creation of #m_instance only. */
  static QMutex m_mutex;

  /** @brief The SQLite connection (default connection name). */
  QSqlDatabase m_db;
};

#endif // DBMANAGER_H
