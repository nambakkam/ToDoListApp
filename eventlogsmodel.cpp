#include "eventlogsmodel.h"
#include "dbmanager.h"
#include <QJsonDocument>
#include <QJsonObject>

EventLogsModel::EventLogsModel(QObject *parent) : QAbstractListModel(parent) {
  refresh();
}

int EventLogsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_logs.size();
}

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

QHash<int, QByteArray> EventLogsModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[IdRole] = "id";
  roles[EventTypeRole] = "eventType";
  roles[NoteNameRole] = "noteName";
  roles[TaskNameRole] = "taskName";
  roles[TimestampRole] = "timestamp";
  return roles;
}

void EventLogsModel::refresh() {
  beginResetModel();
  m_logs = DBManager::instance()->getEventLogs(); // Uses your DB function
  endResetModel();
}
