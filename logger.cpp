#include "logger.h"
#include "dbmanager.h"

Logger::Logger(QObject *parent) : QObject(parent) {}

Logger &Logger::instance() {
  static Logger logger;
  return logger;
}

QString Logger::eventTypeToString(Logger::EventType type) {
  const QMetaObject &metaObj = Logger::staticMetaObject;
  int enumIndex = metaObj.indexOfEnumerator("EventType");
  QMetaEnum metaEnum = metaObj.enumerator(enumIndex);
  return QString(metaEnum.valueToKey(type));
}

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
