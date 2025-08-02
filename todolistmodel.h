#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
struct listElement {
  int id;
  QString itemName;
  bool completionStatus;
};

class ToDoListModel : public QAbstractListModel {
  Q_OBJECT
public:
  explicit ToDoListModel(QObject *parent = nullptr);
  ~ToDoListModel();
  enum roleEnums { IdRole = Qt::UserRole + 1, ItemNameRole, StatusRole };
  Q_ENUM(roleEnums);
  Q_INVOKABLE virtual int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;
  Q_INVOKABLE virtual QVariant data(const QModelIndex &index,
                                    int role = Qt::DisplayRole) const override;
  Q_INVOKABLE virtual QVariant
  headerData(int section, Qt::Orientation orientation,
             int role = Qt::DisplayRole) const override;
  Q_INVOKABLE virtual QHash<int, QByteArray> roleNames() const override;
  Q_INVOKABLE void addItemToList(const QString &data);
  Q_INVOKABLE void removeItemFromList(const int &index);

  Q_INVOKABLE void toggleTaskStatus(const int &index, const bool &status);

private:
  QVector<listElement> modelData;
  int modelId;
};

#endif // TODOLISTMODEL_H
