/**
 * @file eventlogsmodel.h
 * @brief Declares EventLogsModel, the read-only model behind the Logs page.
 */
#ifndef EVENTLOGSMODEL_H
#define EVENTLOGSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariantMap>

/**
 * @class EventLogsModel
 * @brief Read-only model of the audit log (exposed to QML as
 *        `eventLogsModel`).
 *
 * Holds the rows of the `eventLogs` table (newest first) as QVariantMaps.
 * It loads once at construction and afterwards only on refresh(); it does not
 * observe Logger, so main.qml calls refresh() when the Logs page is opened.
 * `event_type` is the Logger::EventType enumerator name; the note and task
 * names are extracted from the JSON in `event_description` every time they
 * are requested.
 *
 * QML role names: `id`, `eventType`, `noteName`, `taskName`, `timestamp`.
 *
 * @see QAbstractListModel
 */
class EventLogsModel : public QAbstractListModel {
  Q_OBJECT
public:
  /** @brief Custom data roles of this model. */
  enum Roles {
    IdRole = Qt::UserRole + 1, /**< `eventLogs.id` (QML `id`). */
    EventTypeRole,             /**< Enumerator name (QML `eventType`). */
    NoteNameRole,              /**< "NoteName" from the JSON (`noteName`). */
    TaskNameRole,              /**< "TaskName" from the JSON (`taskName`). */
    TimestampRole              /**< `created_at` (QML `timestamp`). */
  };
  Q_ENUM(Roles)

  /**
   * @brief Constructs the model and loads the logs once via refresh().
   * @param parent Optional QObject parent.
   */
  explicit EventLogsModel(QObject *parent = nullptr);

  // Basic overrides

  /**
   * @brief Number of log entries.
   * @param parent Must be invalid; a valid parent yields 0 (flat model).
   * @return Number of rows.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  /**
   * @brief Returns one field of a log entry.
   * @param index Row to read.
   * @param role One of Roles. NoteNameRole and TaskNameRole parse
   *        `event_description` as JSON on every call; a missing key (e.g.
   *        "TaskName" for list events) gives an empty string.
   * @return The value, or an invalid QVariant for an invalid index or
   *         unknown role.
   */
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  /**
   * @brief Maps roles to the names used in QML delegates.
   * @return `id`, `eventType`, `noteName`, `taskName` and `timestamp`.
   */
  QHash<int, QByteArray> roleNames() const override;

  /**
   * @brief Reloads all log rows from the database and resets the model.
   */
  Q_INVOKABLE void refresh(); // To reload logs

private:
  /** @brief Rows of `eventLogs` (keys id, event_type, event_description,
   *  created_at), newest first. */
  QList<QVariantMap> m_logs;
};

#endif // EVENTLOGSMODEL_H
