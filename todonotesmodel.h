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
 * This class inherits from QAbstractListModel and provides an interface for storing,
 * retrieving, and displaying TODO notes. It supports custom roles for note ID, item name,
 * and timestamp, and can reload from the database or drop a note by id. It does
 * not write to the database or the event log; mutations go through Controller.
 *
 * QML role names: `NoteID`, `NoteName`, `CreatedAt`.
 *
 * @see Controller
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
   * @brief Reloads all lists from the database (newest first).
   *
   * Resets the model. Called once from main() at start-up and after each add.
   */
  Q_INVOKABLE void fetchAllNotesFromDB();

  // In-memory helper used by Controller after it has written to the DB.
  bool removeById(int id);

private:
  /** @brief In-memory lists, in the order returned by the database. */
  QVector<notesElement> modelData;
};

#endif // TODONOTESMODEL_H
