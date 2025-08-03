#ifndef TODONOTESMODEL_H
#define TODONOTESMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
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
 * retrieving, and manipulating TODO notes. It supports custom roles for note ID, item name,
 * and timestamp, and exposes methods for adding, removing, and fetching notes.
 *
 * @note The model uses a QVector of notesElement to store its data.
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
  Q_INVOKABLE void addNoteToList(const QString &data);
  Q_INVOKABLE void removeNoteFromList(const int &index);
  Q_INVOKABLE void fetchAllNotesFromDB();

private:
  QVector<notesElement> modelData;
};

#endif // TODONOTESMODEL_H
