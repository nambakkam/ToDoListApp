#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QObject>

class Logger : public QObject {
  Q_OBJECT
public:
  enum EventType {
    NOTE_CREATED,
    NOTE_DELETED,
    NOTE_UPDATED,
    TASK_ADDED,
    TASK_DELETED,
    TASK_STATUS_TOGGLED
  };
  Q_ENUM(EventType)

  static Logger &instance(); // Singleton accessor

  void logEvent(EventType type, const QString &noteName,
                const QString &taskName = QString());

private:
  explicit Logger(QObject *parent = nullptr);
  QString eventTypeToString(EventType type);
};

#endif // LOGGER_H
