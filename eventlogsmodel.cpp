#include "eventlogsmodel.h"
#include "dbmanager.h"
#include <QJsonDocument>
#include <QJsonObject>

EventLogsModel::EventLogsModel(QObject *parent) : QAbstractListModel(parent) {
  refresh();
}

/**
 * @brief Returns the number of rows in the model.
 *
 * This function provides the number of event log entries stored in the model.
 * If the given parent index is valid, it returns 0, indicating that this model
 * does not support hierarchical data.
 *
 * @param parent The parent index. Used to support hierarchical models.
 * @return The number of rows (event logs) in the model.
 */
int EventLogsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_logs.size();
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 *
 * This method retrieves data from the internal logs list based on the specified model index and role.
 * It supports custom roles such as IdRole, EventTypeRole, NoteNameRole, TaskNameRole, and TimestampRole.
 * For NoteNameRole and TaskNameRole, it parses the "event_description" field as JSON and extracts the respective values.
 *
 * @param index The model index specifying the row of the data to retrieve.
 * @param role The role for which the data is requested.
 * @return QVariant containing the requested data, or an invalid QVariant if the index is invalid or out of range.
 */
QVariant EventLogsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_logs.size())
    return QVariant();

  const QVariantMap &log = m_logs[index.row()];

  switch (role) {
  case IdRole:
    return log.value("id");
  case EventTypeRole:
    return log.value("event_type");
  case NoteNameRole: {
    QJsonDocument doc = QJsonDocument::fromJson(
        log.value("event_description").toString().toUtf8());
    return doc.object().value("NoteName").toString();
  }
  case TaskNameRole: {
    QJsonDocument doc = QJsonDocument::fromJson(
        log.value("event_description").toString().toUtf8());
    return doc.object().value("TaskName").toString();
  }
  case TimestampRole:
    return log.value("created_at");
  }

  return QVariant();
}

/**
 * @brief Returns a hash mapping custom role identifiers to their names for the model.
 *
 * This function defines the roles used in the EventLogsModel, associating each
 * role's integer identifier with its corresponding name as a QByteArray.
 * These roles are used for data retrieval in views and delegates, especially in
 * QML or when using custom models with Qt's Model/View framework.
 *
 * @return QHash<int, QByteArray> A hash containing role identifiers and their names.
 */
QHash<int, QByteArray> EventLogsModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[IdRole] = "id";
  roles[EventTypeRole] = "eventType";
  roles[NoteNameRole] = "noteName";
  roles[TaskNameRole] = "taskName";
  roles[TimestampRole] = "timestamp";
  return roles;
}

/**
 * @brief Refreshes the event logs model by reloading data from the database.
 *
 * This function resets the model and retrieves the latest event logs
 * from the database using DBManager. It ensures that the model's data
 * is up-to-date and notifies any attached views of the change.
 */
void EventLogsModel::refresh() {
  beginResetModel();
  m_logs = DBManager::instance()->getEventLogs(); // Uses your DB function
  endResetModel();
}
