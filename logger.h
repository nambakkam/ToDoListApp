#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QObject>

/**
 * @class Logger
 * @brief Singleton class for logging note and task events within the application.
 *
 * The Logger class provides functionality to log various events related to notes and tasks,
 * such as creation, deletion, updates, and status changes. It uses Qt's QObject for signal-slot
 * capabilities and supports event type enumeration for easy event identification.
 *
 * Usage:
 *   Logger::instance().logEvent(Logger::NOTE_CREATED, "NoteName");
 *
 * @note This class follows the singleton pattern. Use Logger::instance() to access the logger.
 */
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
