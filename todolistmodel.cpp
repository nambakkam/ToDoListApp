#include "todolistmodel.h"

ToDoListModel::ToDoListModel(QObject *parent) : modelId{1} { Q_UNUSED(parent); }

ToDoListModel::~ToDoListModel() {}

int ToDoListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return modelData.count();
}

QVariant ToDoListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= modelData.count()) {
    return 0;
  }
  listElement item = modelData.at(index.row());
  switch (role) {
  case IdRole:
    return item.id;
  case ItemNameRole:
    return item.itemName;
  case StatusRole:
    return item.completionStatus;
  default:
    return QVariant();
    break;
  }
}

QVariant ToDoListModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    if (section == 0)
      return QString("Task");
  } else if (orientation == Qt::Vertical) {
    // Optional: show row numbers
    return QString::number(section + 1);
  }

  return QVariant();
}

QHash<int, QByteArray> ToDoListModel::roleNames() const {
  QHash<int, QByteArray> hashMap;
  hashMap[IdRole] = "id";
  hashMap[ItemNameRole] = "ItemName";
  hashMap[StatusRole] = "StatusRole";
  return hashMap;
}

void ToDoListModel::addItemToList(const QString &data) {
  listElement item;
  item.id = modelId++;
  item.itemName = data;
  item.completionStatus = false;
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  modelData.append(item);
  endInsertRows();
}

void ToDoListModel::removeItemFromList(const int &index) {
  if (index < 0 || index >= modelData.size())
    return;
  beginRemoveRows(QModelIndex(), index, index);
  modelData.removeAt(index);
  endRemoveRows();
}

void ToDoListModel::toggleTaskStatus(const int &index, const bool &status) {
  if (index < 0 || index >= modelData.size())
    return;
  modelData[index].completionStatus = status;
  emit dataChanged(this->index(index), this->index(index), {StatusRole});
}
