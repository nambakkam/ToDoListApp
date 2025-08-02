#include "todonotesmodel.h"
#include "dbmanager.h"
#include "logger.h"
#include <QDateTime>
TODONotesModel::TODONotesModel(QAbstractListModel *parent)
    : QAbstractListModel{parent} {
  Q_UNUSED(parent)
}

TODONotesModel::~TODONotesModel() {}

int TODONotesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return modelData.count();
}

QVariant TODONotesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= modelData.count()) {
    return 0;
  }
  notesElement item = modelData.at(index.row());
  switch (role) {
  case noteIDRole:
    return item.id;
  case ItemNameRole:
    return item.itemName;
  case TimeStampRole:
    return item.creationTime;
  default:
    return QVariant();
    break;
  }
}

QVariant TODONotesModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    if (section == 0)
      return QString("Note");
  } else if (orientation == Qt::Vertical) {
    // Optional: show row numbers
    return QString::number(section + 1);
  }
  return QVariant();
}

QHash<int, QByteArray> TODONotesModel::roleNames() const {
  QHash<int, QByteArray> roleNameHash;
  roleNameHash[noteIDRole] = "NoteID";
  roleNameHash[ItemNameRole] = "NoteName";
  roleNameHash[TimeStampRole] = "CreatedAt";
  return roleNameHash;
}

void TODONotesModel::addNoteToList(const QString &data) {
  int eventID = DBManager::instance()->addNote(data);
  //  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  //  notesElement element;
  //  element.id = eventID;
  //  element.itemName = data;
  //  element.creationTime = QDateTime::currentDateTime();
  //  modelData.append(element);
  //  endInsertRows();
  fetchAllNotesFromDB();
  Logger::instance().logEvent(Logger::NOTE_CREATED, data);
}

void TODONotesModel::removeNoteFromList(const int &index) {
  int eventID = modelData.at(index).id;
  Logger::instance().logEvent(Logger::NOTE_DELETED,
                              modelData.at(index).itemName);
  DBManager::instance()->deleteNote(eventID);
  beginRemoveRows(QModelIndex(), index, index);
  modelData.removeAt(index);
  endRemoveRows();
}

void TODONotesModel::fetchAllNotesFromDB() {
  QList<QVariantMap> list;
  beginResetModel();
  modelData.clear();
  list = DBManager::instance()->getAllNotes();
  for (const auto &a : list) {
    notesElement element;
    element.id = a["note_id"].toInt();
    element.itemName = a["title"].toString();
    element.creationTime = QDateTime::fromString(a["created_at"].toString());
    modelData.append(element);
  }
  endResetModel();
}
