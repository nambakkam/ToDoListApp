#include "logger.h"
#include "dbmanager.h"

Logger::Logger(QObject *parent) : QObject(parent) {}

/**
 * @brief Returns the singleton instance of the Logger class.
 *
 * This method ensures that only one instance of Logger exists throughout
 * the application by using a static local variable. Subsequent calls to
 * this function will return the same Logger instance.
 *
 * @return Reference to the singleton Logger instance.
 */
Logger &Logger::instance() {
  static Logger logger;
  return logger;
}

/**
 * @brief Converts an EventType enum value to its corresponding string representation.
 *
 * This function uses Qt's meta-object system to retrieve the string key associated
 * with the given EventType enum value.
 *
 * @param type The EventType enum value to convert.
 * @return QString The string representation of the specified EventType.
 */
QString Logger::eventTypeToString(Logger::EventType type) {
  const QMetaObject &metaObj = Logger::staticMetaObject;
  int enumIndex = metaObj.indexOfEnumerator("EventType");
  QMetaEnum metaEnum = metaObj.enumerator(enumIndex);
  return QString(metaEnum.valueToKey(type));
}

/**
 * @brief Logs an event to the database with the specified type and details.
 *
 * This function prepares a JSON object containing the note name and, if provided,
 * the task name. It then converts the event type to a string and calls the database
 * manager to add the event log. The result of the operation is output to the debug log.
 *
 * @param type The type of the event to log.
 * @param noteName The name of the note associated with the event.
 * @param taskName The name of the task associated with the event (optional).
 */
void Logger::logEvent(EventType type, const QString &noteName,
                      const QString &taskName) {
  // Prepare JSON description
  QJsonObject jsonObj;
  jsonObj["NoteName"] = noteName;
  if (!taskName.isEmpty())
    jsonObj["TaskName"] = taskName;

  QString jsonString =
      QString::fromUtf8(QJsonDocument(jsonObj).toJson(QJsonDocument::Compact));

  // Get event type as string
  QString eventTypeStr = eventTypeToString(type);

  // Call DB function
  int result = DBManager::instance()->addEventLog(eventTypeStr, jsonString);

  if (result == -1) {
    qDebug() << "Failed to log event:" << eventTypeStr << jsonString;
  } else {
    qDebug() << "Event logged:" << eventTypeStr << jsonString;
  }
}
