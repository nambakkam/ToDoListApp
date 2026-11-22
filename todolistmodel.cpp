#include "todolistmodel.h"
#include "dbmanager.h"
ToDoListModel::ToDoListModel(QObject *parent)
    : QAbstractListModel{parent}, m_noteID{-1} {
  Q_UNUSED(parent);
  QObject::connect(this, &ToDoListModel::noteIDChanged, this,
                   &ToDoListModel::fetchListFromDB);
}

ToDoListModel::~ToDoListModel() {}

/**
 * @brief Returns the number of rows in the model.
 *
 * This function returns the number of items in the to-do list model.
 * If the provided parent index is valid, it returns 0, indicating that
 * this model does not support hierarchical data.
 *
 * @param parent The parent index. Used to support hierarchical models.
 * @return The number of rows in the model.
 */
int ToDoListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return modelData.count();
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 *
 * This function retrieves the data for a specific item in the model based on the provided index and role.
 * It supports custom roles such as IdRole, ItemNameRole, and StatusRole, returning the corresponding
 * item properties. If the index is invalid or out of bounds, it returns 0. For unsupported roles, it returns
 * an empty QVariant.
 *
 * @param index The QModelIndex identifying the item in the model.
 * @param role The role for which the data is requested.
 * @return QVariant containing the requested data, or an empty QVariant if the role is not supported.
 */
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

/**
 * @brief Returns the data for the given role and section in the header with the specified orientation.
 *
 * This function provides header data for the model, such as column names or row numbers.
 * For horizontal orientation and display role, it returns "Task" for the first column.
 * For vertical orientation and display role, it returns the row number (1-based).
 * For other roles or sections, it returns an invalid QVariant.
 *
 * @param section The section of the header (column or row index).
 * @param orientation The orientation of the header (horizontal or vertical).
 * @param role The role for which the data is requested (e.g., Qt::DisplayRole).
 * @return QVariant containing the header data, or an invalid QVariant if not applicable.
 */
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

/**
 * @brief Returns a hash map of custom role names used in the model.
 *
 * This function provides a mapping between integer role identifiers and their corresponding
 * string names. These roles are used to access specific data fields in the model, such as
 * the item's ID, name, and status.
 *
 * @return QHash<int, QByteArray> A hash map where the key is the role identifier and the value is the role name.
 */
QHash<int, QByteArray> ToDoListModel::roleNames() const {
  QHash<int, QByteArray> hashMap;
  hashMap[IdRole] = "id";
  hashMap[ItemNameRole] = "ItemName";
  hashMap[StatusRole] = "StatusRole";
  return hashMap;
}

/**
 * @brief Returns the name of the task with the given database id.
 *
 * @param id The task (note content) id.
 * @return The task name, or an empty string if no such task is loaded.
 */
QString ToDoListModel::itemNameById(int id) const {
  for (const auto &item : modelData) {
    if (item.id == id)
      return item.itemName;
  }
  return QString();
}

/**
 * @brief Removes the task with the given database id from the in-memory list.
 *
 * Only updates the model; the caller is responsible for deleting the row
 * from the database.
 *
 * @param id The task (note content) id.
 * @return true if a row was removed, false if no such task is loaded.
 */
bool ToDoListModel::removeById(int id) {
  for (int row = 0; row < modelData.size(); ++row) {
    if (modelData.at(row).id == id) {
      beginRemoveRows(QModelIndex(), row, row);
      modelData.removeAt(row);
      endRemoveRows();
      return true;
    }
  }
  return false;
}

/**
 * @brief Sets the completion status of the task with the given id in memory.
 *
 * Only updates the model and notifies views; the caller is responsible for
 * persisting the change.
 *
 * @param id The task (note content) id.
 * @param status The new completion status.
 * @return true if the task was found, false otherwise.
 */
bool ToDoListModel::setStatusById(int id, bool status) {
  for (int row = 0; row < modelData.size(); ++row) {
    if (modelData.at(row).id == id) {
      modelData[row].completionStatus = status;
      emit dataChanged(index(row), index(row), {StatusRole});
      return true;
    }
  }
  return false;
}

/**
 * @brief Fetches the to-do list items from the database and updates the model.
 *
 * This function retrieves the list of to-do items associated with the current note ID
 * from the database using DBManager. It resets the model, clears the existing data,
 * and populates the model with the fetched items. Each item includes its ID, content,
 * and completion status.
 *
 * The model is reset before and after updating to ensure proper notification of views.
 */
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

/**
 * @brief Sets the note ID for the ToDoListModel.
 *
 * If the provided value is different from the current note ID,
 * updates the note ID and emits the noteIDChanged() signal.
 *
 * @param val The new note ID to set.
 */
void ToDoListModel::setNoteID(const int &val) {
  if (val != m_noteID) {
    m_noteID = val;
    emit noteIDChanged();
  }
}

/**
 * @brief Returns the ID of the current note.
 * 
 * @return Reference to the integer representing the note ID.
 */
const int &ToDoListModel::getNoteID() { return m_noteID; }
