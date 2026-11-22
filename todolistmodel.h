/**
 * @file todolistmodel.h
 * @brief Declares ToDoListModel, the model of the tasks of the open list.
 */
#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
/**
 * @struct listElement
 * @brief One task of the open list (a row of the `NotesContents` table).
 *
 * This structure holds the information for a task,
 * including its unique identifier, name, and completion status.
 */
struct listElement {
  int id;                /**< `NotesContents.id` of the task (database id). */
  QString itemName;      /**< Task text (`content` column). */
  bool completionStatus; /**< true if the task is completed. */
};

/**
 * @class ToDoListModel
 * @brief Model of the tasks of the currently open list (exposed to QML as
 *        `todoModel`).
 *
 * Inherits from QAbstractListModel and provides an interface for storing,
 * retrieving, and manipulating to-do list items. Supports custom roles for
 * item ID, name, and status. Reads its data from the database and exposes
 * id-based helpers to update the in-memory list. It does not write to the
 * database or the event log; mutations go through Controller. Integrates with
 * Qt's meta-object system for use in QML and signal-slot communication.
 *
 * @note This class is the read-only model in the Model-View-Controller (MVC)
 * pattern; Controller performs the writes.
 *
 * @see Controller
 *
 * @see QAbstractListModel
 */
class ToDoListModel : public QAbstractListModel {
  Q_OBJECT
public:
  /**
   * @brief Constructs the model with no list open (note id -1).
   *
   * Connects noteIDChanged() to fetchListFromDB().
   *
   * @param parent Optional QObject parent.
   */
  explicit ToDoListModel(QObject *parent = nullptr);

  /** @brief Destructor. */
  virtual ~ToDoListModel();

  /** @brief Custom data roles of this model. */
  enum roleEnums {
    IdRole = Qt::UserRole + 1, /**< Task database id (QML name `id`). */
    ItemNameRole,              /**< Task text (QML name `ItemName`). */
    StatusRole                 /**< Completed flag (QML name `StatusRole`). */
  };
  Q_ENUM(roleEnums);

  /**
   * @brief Number of tasks.
   * @param parent Must be invalid; a valid parent yields 0 (flat model).
   * @return Number of rows.
   */
  Q_INVOKABLE virtual int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;

  /**
   * @brief Returns the value of one task for a role.
   * @param index Row to read.
   * @param role IdRole, ItemNameRole or StatusRole.
   * @return The value; an invalid QVariant for other roles. For an invalid or
   *         out-of-range index it returns `QVariant(0)` (an integer zero),
   *         not an invalid QVariant.
   */
  Q_INVOKABLE virtual QVariant data(const QModelIndex &index,
                                    int role = Qt::DisplayRole) const override;

  /**
   * @brief Header text for views that show headers.
   * @param section Section number.
   * @param orientation Horizontal gives "Task" for section 0; vertical gives
   *        the 1-based row number.
   * @param role Only Qt::DisplayRole is handled.
   * @return The text, or an invalid QVariant otherwise.
   */
  Q_INVOKABLE virtual QVariant
  headerData(int section, Qt::Orientation orientation,
             int role = Qt::DisplayRole) const override;

  /**
   * @brief Maps roles to the names used in QML delegates.
   * @return `id`, `ItemName` and `StatusRole`.
   */
  Q_INVOKABLE virtual QHash<int, QByteArray> roleNames() const override;

  /**
   * @brief Reloads the tasks of the current list from the database.
   *
   * Resets the model with the rows of getNoteContents(m_noteID).
   */
  Q_INVOKABLE void fetchListFromDB();

  // In-memory helpers used by Controller after it has written to the DB.
  QString itemNameById(int id) const;
  bool removeById(int id);
  bool setStatusById(int id, bool status);

public slots:
  /**
   * @brief Selects the open list.
   *
   * If @p val differs from the current id, stores it and emits
   * noteIDChanged() (which reloads the tasks). Setting the same id again
   * does nothing.
   *
   * @param val `Notes.note_id` of the list to open.
   */
  void setNoteID(const int &val);

  /**
   * @brief Returns the id of the open list.
   * @return Reference to the stored id; -1 until a list has been opened.
   */
  const int &getNoteID();

signals:
  /** @brief Emitted when setNoteID() changes the open list. */
  void noteIDChanged();

private:
  /** @brief In-memory tasks of the open list, in database order. */
  QVector<listElement> modelData;

  /** @brief Id of the open list; -1 until set. */
  int m_noteID;
};

#endif // TODOLISTMODEL_H
