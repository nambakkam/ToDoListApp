#ifndef TODONOTESMODEL_H
#define TODONOTESMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
/**
 * @struct notesElement
 * @brief Represents a single note item with an identifier, name, and creation timestamp.
 *
 * This structure holds the essential information for a note, including its unique ID,
 * descriptive name, and the time it was created.
 *
 * @var notesElement::id
 *   Unique identifier for the note.
 * @var notesElement::itemName
 *   Name or description of the note item.
 * @var notesElement::creationTime
 *   Timestamp indicating when the note was created.
 */
struct notesElement {
  int id;
  QString itemName;
  QDateTime creationTime;
};
/**
 * @class TODONotesModel
 * @brief Model class for managing a list of TODO notes in a Qt MVC application.
 *
 * This class inherits from QAbstractListModel and provides an interface for storing,
 * retrieving, and displaying TODO notes. It supports custom roles for note ID, item name,
 * and timestamp, and can reload from the database or drop a note by id. It does
 * not write to the database or the event log; mutations go through Controller.
 *
 * @note The model uses a QVector of notesElement to store its data.
 *
 * @see Controller
 *
 * @see QAbstractListModel
 */
class TODONotesModel : public QAbstractListModel {
  Q_OBJECT
public:
  explicit TODONotesModel(QAbstractListModel *parent = nullptr);
  virtual ~TODONotesModel();
  enum roleEnums { noteIDRole = Qt::UserRole + 1, ItemNameRole, TimeStampRole };
  Q_ENUM(roleEnums);
  Q_INVOKABLE virtual int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;
  Q_INVOKABLE virtual QVariant data(const QModelIndex &index,
                                    int role = Qt::DisplayRole) const override;
  Q_INVOKABLE virtual QVariant
  headerData(int section, Qt::Orientation orientation,
             int role = Qt::DisplayRole) const override;
  Q_INVOKABLE virtual QHash<int, QByteArray> roleNames() const override;
  Q_INVOKABLE void fetchAllNotesFromDB();

  // In-memory helper used by Controller after it has written to the DB.
  bool removeById(int id);

private:
  QVector<notesElement> modelData;
};

#endif // TODONOTESMODEL_H
