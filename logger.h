/**
 * @file logger.h
 * @brief Declares Logger, the singleton that writes audit-log events.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QObject>

/**
 * @class Logger
 * @brief Singleton that records list/task events in the `eventLogs` table.
 *
 * Each call to logEvent() stores the enumerator name of the event type in
 * `event_type` and a compact JSON object (`{"NoteName":...,"TaskName":...}`)
 * in `event_description` through DBManager::addEventLog(). In UI terms a
 * "note" is a list and a "task" is a task. The model methods that mutate data
 * call this class; EventLogsModel reads the rows back.
 *
 * Usage:
 *   Logger::instance().logEvent(Logger::NOTE_CREATED, "NoteName");
 *
 * @note Use Logger::instance() to access the logger.
 */
class Logger : public QObject {
  Q_OBJECT
public:
  /**
   * @brief Kinds of events that can be logged.
   *
   * The enumerator name (not the numeric value) is what gets stored and
   * shown on the Logs page, so renaming an enumerator changes stored data.
   */
  enum EventType {
    NOTE_CREATED,        /**< A list was created. */
    NOTE_DELETED,        /**< A list was deleted. */
    NOTE_UPDATED,        /**< A list was renamed; defined but never logged. */
    TASK_ADDED,          /**< A task was added to a list. */
    TASK_DELETED,        /**< A task was removed from a list. */
    TASK_STATUS_TOGGLED  /**< A task was (un)checked; TaskName gets ":0"/":1"
                              appended by the caller. */
  };
  Q_ENUM(EventType)

  /**
   * @brief Returns the singleton (function-local static, thread-safe init).
   * @return Reference to the shared Logger.
   */
  static Logger &instance(); // Singleton accessor

  /**
   * @brief Writes an event to the `eventLogs` table and echoes it to qDebug().
   * @param type Event kind; stored as its enumerator name.
   * @param noteName Name of the list involved (stored as "NoteName").
   * @param taskName Name of the task involved; stored as "TaskName" only
   *        when not empty.
   */
  void logEvent(EventType type, const QString &noteName,
                const QString &taskName = QString());

private:
  /**
   * @brief Private constructor (singleton).
   * @param parent Optional QObject parent.
   */
  explicit Logger(QObject *parent = nullptr);

  /**
   * @brief Converts an EventType to its enumerator name.
   * @param type Event kind.
   * @return The name, e.g. "TASK_ADDED".
   */
  QString eventTypeToString(EventType type);
};

#endif // LOGGER_H
