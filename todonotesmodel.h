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
