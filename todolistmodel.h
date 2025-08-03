#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
/**
 * @struct listElement
 * @brief Represents a single item in the to-do list.
 *
 * This structure holds the information for a to-do list element,
 * including its unique identifier, name, and completion status.
 *
 * @var listElement::id
 * Unique identifier for the to-do list item.
 *
 * @var listElement::itemName
 * Name or description of the to-do list item.
 *
 * @var listElement::completionStatus
 * Indicates whether the item has been completed (true) or not (false).
 */
struct listElement {
  int id;
  QString itemName;
  bool completionStatus;
};

/**
 * @class ToDoListModel
 * @brief Model class for managing a list of to-do items in a Qt MVC application.
 *
 * Inherits from QAbstractListModel and provides an interface for storing,
 * retrieving, and manipulating to-do list items. Supports custom roles for
 * item ID, name, and status. Exposes methods for adding, removing, toggling
 * task status, and fetching data from a database. Integrates with Qt's
 * meta-object system for use in QML and signal-slot communication.
 *
 * @note This class is intended to be used as the model in a Model-View-Controller
 * (MVC) pattern, typically with a QListView or similar view component.
 *
 * @see QAbstractListModel
 */
class ToDoListModel : public QAbstractListModel {
  Q_OBJECT
public:
  explicit ToDoListModel(QObject *parent = nullptr);
  virtual ~ToDoListModel();
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

  Q_INVOKABLE void fetchListFromDB();

public slots:
  void setNoteID(const int &val);
  const int &getNoteID();

signals:
  void noteIDChanged();

private:
  QVector<listElement> modelData;
  int m_noteID;
};

#endif // TODOLISTMODEL_H
