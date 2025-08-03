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
 * @brief Singleton class for managing database operations related to notes, note contents, and event logs.
 *
 * This class provides an interface to interact with the application's database, including opening/closing the database,
 * performing CRUD operations on notes and their contents, and logging events. It ensures thread safety and uses Qt's
 * SQL module for database access.
 *
 * @note This class is implemented as a singleton. Use DBManager::instance() to access the instance.
 */
class DBManager : public QObject {
  Q_OBJECT
public:
  static DBManager *instance();
  bool openDB(const QString &path);
  void closeDB();

  // Notes operations
  int addNote(const QString &title);
  bool updateNoteTitle(int noteId, const QString &newTitle);
  QList<QVariantMap> getAllNotes();
  bool deleteNote(int noteId);

  // NotesContents operations
  int addNoteContent(int noteId, const QString &content);
  bool updateNoteContent(int contentId, bool completed);
  QList<QVariantMap> getNoteContents(int noteId);
  bool deleteNoteContent(int contentId);

  // Event logs
  int addEventLog(const QString &eventType, const QString &eventDescription);
  QList<QVariantMap> getEventLogs();
  ~DBManager();

  bool deleteAllNoteContents(int noteID);
public slots:
  QString getNoteName(int noteId);
private slots:
  bool createTablesFromFile(const QString &sqlFilePath);

private:
  explicit DBManager(QObject *parent = nullptr);
  static DBManager *m_instance;
  static QMutex m_mutex;

  QSqlDatabase m_db;
};

#endif // DBMANAGER_H
