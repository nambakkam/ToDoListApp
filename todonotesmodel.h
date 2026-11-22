/**
 * @file todonotesmodel.h
 * @brief Declares TODONotesModel, the model of all lists on the home page.
 */
#ifndef TODONOTESMODEL_H
#define TODONOTESMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
/**
 * @struct notesElement
 * @brief One list shown on the home page (a row of the `Notes` table).
 *
 * This structure holds the essential information for a list ("note" in the
 * code), including its unique ID, name, and the time it was created.
 */
struct notesElement {
  int id;                  /**< `Notes.note_id` of the list. */
  QString itemName;        /**< List name (`title` column). */
  QDateTime creationTime;  /**< Creation time parsed from `created_at`. */
};
/**
 * @class TODONotesModel
 * @brief Model of all lists (exposed to QML as `todoNotesModel`).
 *
 * "Note" in the code means "list" in the UI; the tasks of a list are handled
 * by ToDoListModel. Adding re-fetches the lists from the database and resets
 * the model; removing updates the in-memory vector in place. Each mutating
 * method also records an event through Logger.
 *
 * QML role names: `NoteID`, `NoteName`, `CreatedAt`.
 *
 * @see QAbstractListModel
 */
class TODONotesModel : public QAbstractListModel {
  Q_OBJECT
public:
  /**
   * @brief Constructs an empty model; call fetchAllNotesFromDB() to fill it.
   * @param parent Parent model (the parameter is typed QAbstractListModel*
   *        and is only passed to the base class).
   */
  explicit TODONotesModel(QAbstractListModel *parent = nullptr);

  /** @brief Destructor. */
  virtual ~TODONotesModel();

  /** @brief Custom data roles of this model. */
  enum roleEnums {
    noteIDRole = Qt::UserRole + 1, /**< List id (QML name `NoteID`). */
    ItemNameRole,                  /**< List name (QML name `NoteName`). */
    TimeStampRole                  /**< Creation time (QML `CreatedAt`). */
  };
  Q_ENUM(roleEnums);

  /**
   * @brief Number of lists.
   * @param parent Must be invalid; a valid parent yields 0 (flat model).
   * @return Number of rows.
   */
  Q_INVOKABLE virtual int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;

  /**
   * @brief Returns the value of one list for a role.
   * @param index Row to read.
   * @param role noteIDRole, ItemNameRole or TimeStampRole.
   * @return The value; an invalid QVariant for other roles. For an invalid or
   *         out-of-range index it returns `QVariant(0)` (an integer zero),
   *         not an invalid QVariant.
   */
  Q_INVOKABLE virtual QVariant data(const QModelIndex &index,
                                    int role = Qt::DisplayRole) const override;

  /**
   * @brief Header text for views that show headers.
   * @param section Section number.
   * @param orientation Horizontal gives "Note" for section 0; vertical gives
   *        the 1-based row number.
   * @param role Only Qt::DisplayRole is handled.
   * @return The text, or an invalid QVariant otherwise.
   */
  Q_INVOKABLE virtual QVariant
  headerData(int section, Qt::Orientation orientation,
             int role = Qt::DisplayRole) const override;

  /**
   * @brief Maps roles to the names used in QML delegates.
   * @return `NoteID`, `NoteName` and `CreatedAt`.
   */
  Q_INVOKABLE virtual QHash<int, QByteArray> roleNames() const override;

  /**
   * @brief Creates a list.
   *
   * Inserts it with DBManager::addNote(), re-fetches all lists (model reset)
   * and logs NOTE_CREATED. The id returned by the insert is not checked.
   *
   * @param data Name of the new list.
   */
  Q_INVOKABLE void addNoteToList(const QString &data);

  /**
   * @brief Deletes a list.
   *
   * Logs NOTE_DELETED, deletes the `Notes` row and removes the row from the
   * model. @warning Known bug: the tasks of the list are NOT deleted. The
   * code calls getNoteContents() and discards the result instead of
   * DBManager::deleteAllNoteContents(), so the tasks stay in `NotesContents`.
   * Also, modelData.at(index) is read without a bounds check.
   *
   * @param index Row of the list in the model (not its database id).
   */
  Q_INVOKABLE void removeNoteFromList(const int &index);

  /**
   * @brief Reloads all lists from the database (newest first).
   *
   * Resets the model. Called once from main() at start-up and after each add.
   */
  Q_INVOKABLE void fetchAllNotesFromDB();

private:
  /** @brief In-memory lists, in the order returned by the database. */
  QVector<notesElement> modelData;
};

#endif // TODONOTESMODEL_H
