#include "todonotesmodel.h"
#include "dbmanager.h"
#include "logger.h"
#include <QDateTime>
TODONotesModel::TODONotesModel(QAbstractListModel *parent)
    : QAbstractListModel{parent} {
  Q_UNUSED(parent)
}

TODONotesModel::~TODONotesModel() {}

/**
 * @brief Returns the number of rows in the model.
 *
 * This function provides the number of items in the model, which is used by views
 * to determine how many rows to display. If the given parent index is valid,
 * indicating a request for child items, the function returns 0, as this model
 * does not support hierarchical data.
 *
 * @param parent The parent index. If valid, indicates a request for child items.
 * @return The number of rows in the model, or 0 if the parent index is valid.
 */
int TODONotesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return modelData.count();
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 *
 * This function retrieves the data for a specific row and role from the model.
 * It checks if the index is valid and within bounds, then returns the corresponding
 * value from the notesElement item. Supported roles include noteIDRole, ItemNameRole,
 * and TimeStampRole.
 *
 * @param index The QModelIndex specifying the row and column of the item.
 * @param role The role for which the data is requested.
 * @return QVariant containing the requested data, or an invalid QVariant if the index is not valid or the role is not recognized.
 */
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

/**
 * @brief Returns the data for the given role and section in the header with the specified orientation.
 *
 * This function provides header data for the model, used by views to display column or row headers.
 * For horizontal orientation and display role, it returns "Note" for the first column.
 * For vertical orientation and display role, it returns the row number (starting from 1).
 * For other roles or sections, it returns an invalid QVariant.
 *
 * @param section The section of the header (column or row index).
 * @param orientation The orientation of the header (horizontal or vertical).
 * @param role The role for which the data is requested (typically Qt::DisplayRole).
 * @return QVariant containing the header data, or an invalid QVariant if not applicable.
 */
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

/**
 * @brief Returns a hash mapping custom role integers to their corresponding names for the model.
 *
 * This function defines the role names used in the TODONotesModel, which are utilized
 * for data retrieval in views and delegates. The roles include:
 * - noteIDRole: Maps to "NoteID"
 * - ItemNameRole: Maps to "NoteName"
 * - TimeStampRole: Maps to "CreatedAt"
 *
 * @return QHash<int, QByteArray> A hash containing the role integer as key and the role name as value.
 */
QHash<int, QByteArray> TODONotesModel::roleNames() const {
  QHash<int, QByteArray> roleNameHash;
  roleNameHash[noteIDRole] = "NoteID";
  roleNameHash[ItemNameRole] = "NoteName";
  roleNameHash[TimeStampRole] = "CreatedAt";
  return roleNameHash;
}

/**
 * @brief Adds a new note to the list and updates the model.
 *
 * This function creates a new note with the given data by inserting it into the database.
 * After insertion, it refreshes the model data from the database to reflect the changes.
 * Additionally, it logs the creation event for auditing purposes.
 *
 * @param data The content of the note to be added.
 */
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


/**
 * @brief Removes a note from the model and the database.
 *
 * This function deletes the note at the specified index from both the internal model data
 * and the persistent database. It also logs the deletion event. The function emits the necessary
 * signals to update any views associated with the model.
 *
 * @param index The index of the note to be removed.
 */
void TODONotesModel::removeNoteFromList(const int &index) {
  int eventID = modelData.at(index).id;
  Logger::instance().logEvent(Logger::NOTE_DELETED,
                              modelData.at(index).itemName);
  DBManager::instance()->deleteNote(eventID);
  DBManager::instance()->getNoteContents(eventID);
  beginRemoveRows(QModelIndex(), index, index);
  modelData.removeAt(index);
  endRemoveRows();
}

/**
 * @brief Fetches all notes from the database and updates the model.
 *
 * This method retrieves all notes from the database using DBManager,
 * clears the current model data, and repopulates it with the fetched notes.
 * The model is reset before and after the update to ensure proper notification
 * of changes to any attached views.
 */
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
