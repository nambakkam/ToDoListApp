#ifndef EVENTLOGSMODEL_H
#define EVENTLOGSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariantMap>

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
