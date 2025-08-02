#include "todolistmodel.h"
#include "dbmanager.h"
#include "logger.h"
ToDoListModel::ToDoListModel(QObject *parent)
    : QAbstractListModel{parent}, m_noteID{-1} {
  Q_UNUSED(parent);
  QObject::connect(this, &ToDoListModel::noteIDChanged, this,
                   &ToDoListModel::fetchListFromDB);
}

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
  DBManager::instance()->addNoteContent(m_noteID, data);
  QString noteName = DBManager::instance()->getNoteName(m_noteID);
  Logger::instance().logEvent(Logger::TASK_ADDED, noteName, data);
  fetchListFromDB();
}

void ToDoListModel::removeItemFromList(const int &index) {
  int eventID = modelData.at(index).id;
  DBManager::instance()->deleteNoteContent(eventID);
  QString noteName = DBManager::instance()->getNoteName(m_noteID);
  Logger::instance().logEvent(Logger::TASK_DELETED, noteName,
                              modelData.at(index).itemName);
  if (index < 0 || index >= modelData.size())
    return;
  beginRemoveRows(QModelIndex(), index, index);
  modelData.removeAt(index);
  endRemoveRows();
}

void ToDoListModel::toggleTaskStatus(const int &index, const bool &status) {
  int eventID = modelData.at(index).id;
  DBManager::instance()->updateNoteContent(eventID, status);
  QString noteName = DBManager::instance()->getNoteName(m_noteID);
  Logger::instance().logEvent(Logger::TASK_STATUS_TOGGLED, noteName,
                              modelData.at(index).itemName +
                                  QString(":%1").arg(status));
  if (index < 0 || index >= modelData.size())
    return;
  modelData[index].completionStatus = status;
  emit dataChanged(this->index(index), this->index(index), {StatusRole});
}
void ToDoListModel::fetchListFromDB() {
  QList<QVariantMap> list = DBManager::instance()->getNoteContents(m_noteID);
  beginResetModel();
  modelData.clear();
  for (const auto &a : list) {
    listElement element;
    element.id = a["id"].toInt();
    element.itemName = a["content"].toString();
    element.completionStatus = a["completed"].toBool();
    modelData.append(element);
  }
  endResetModel();
}

void ToDoListModel::setNoteID(const int &val) {
  if (val != m_noteID) {
    m_noteID = val;
    emit noteIDChanged();
  }
}

const int &ToDoListModel::getNoteID() { return m_noteID; }
