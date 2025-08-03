#ifndef EVENTLOGSMODEL_H
#define EVENTLOGSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariantMap>

/**
 * @class EventLogsModel
 * @brief Model for representing event logs in a Qt view.
 *
 * This class inherits from QAbstractListModel and provides a model for storing and displaying
 * event log entries. Each log entry is represented as a QVariantMap and contains fields such as
 * ID, event type, note name, task name, and timestamp. The model exposes custom roles for
 * accessing these fields and provides methods for refreshing the log data.
 *
 * @note This model is intended for use with Qt's Model/View framework.
 *
 * @see QAbstractListModel
 */
class EventLogsModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum Roles {
    IdRole = Qt::UserRole + 1,
    EventTypeRole,
    NoteNameRole,
    TaskNameRole,
    TimestampRole
  };
  Q_ENUM(Roles)

  explicit EventLogsModel(QObject *parent = nullptr);

  // Basic overrides
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void refresh(); // To reload logs

private:
  QList<QVariantMap> m_logs;
};

#endif // EVENTLOGSMODEL_H
