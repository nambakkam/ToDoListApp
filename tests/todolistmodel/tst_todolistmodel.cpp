#include "todolistmodel.h"
#include "testenv.h"
#include <QSignalSpy>

/**
 * @class tst_ToDoListModel
 * @brief Unit tests for ToDoListModel, the model of tasks of the open list.
 *
 * Checks roles, setNoteID() fetching, add/remove/toggle behaviour (in-memory
 * rows, database rows and audit log entries including the ":0"/":1" suffix
 * of TASK_STATUS_TOGGLED).
 */
class tst_ToDoListModel : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();
  void init();

  void defaultNoteIdIsMinusOne();
  void emptyModelAndNoList();
  void roleNames();
  void headerData();
  void setNoteIdEmitsAndFetches();
  void setNoteIdSameValueDoesNotEmit();
  void switchingListReplacesTasks();
  void dataForEachRole();
  void invalidIndexIsFalsy();
  void rowCountOfValidParentIsZero();
  void addItemToListStoresAndResets();
  void addItemToListLogsTaskAdded();
  void removeItemFromListRemovesRowAndDbRow();
  void removeItemFromListLogsTaskDeleted();
  void toggleTaskStatusUpdatesModelAndDb();
  void toggleTaskStatusLogsWithSuffix();
  void toggleOnlyAffectsSelectedRow();

private:
  /** @brief Creates a list in the DB and opens it in @p model. */
  int openList(ToDoListModel &model, const QString &name);

  TestEnv m_env;
};

void tst_ToDoListModel::initTestCase() { QVERIFY(m_env.setUp()); }

void tst_ToDoListModel::cleanupTestCase() { m_env.tearDown(); }

void tst_ToDoListModel::init() { TestEnv::clearTables(); }

int tst_ToDoListModel::openList(ToDoListModel &model, const QString &name) {
  const int id = DBManager::instance()->addNote(name);
  model.setNoteID(id);
  return id;
}

void tst_ToDoListModel::defaultNoteIdIsMinusOne() {
  ToDoListModel model;
  QCOMPARE(model.getNoteID(), -1);
}

void tst_ToDoListModel::emptyModelAndNoList() {
  ToDoListModel model;
  QCOMPARE(model.rowCount(), 0);
  model.fetchListFromDB();
  QCOMPARE(model.rowCount(), 0);
}

void tst_ToDoListModel::roleNames() {
  ToDoListModel model;
  const auto names = model.roleNames();
  QCOMPARE(names.value(ToDoListModel::IdRole), QByteArray("id"));
  QCOMPARE(names.value(ToDoListModel::ItemNameRole), QByteArray("ItemName"));
  QCOMPARE(names.value(ToDoListModel::StatusRole), QByteArray("StatusRole"));
  QCOMPARE(names.size(), 3);
}

void tst_ToDoListModel::headerData() {
  ToDoListModel model;
  QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("Task"));
  QCOMPARE(model.headerData(0, Qt::Vertical).toString(), QString("1"));
  QVERIFY(!model.headerData(0, Qt::Horizontal, Qt::EditRole).isValid());
}

void tst_ToDoListModel::setNoteIdEmitsAndFetches() {
  const int id = DBManager::instance()->addNote("List");
  DBManager::instance()->addNoteContent(id, "t1");
  DBManager::instance()->addNoteContent(id, "t2");
  ToDoListModel model;
  QSignalSpy changedSpy(&model, &ToDoListModel::noteIDChanged);
  QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
  model.setNoteID(id);
  QCOMPARE(changedSpy.count(), 1);
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(model.getNoteID(), id);
  QCOMPARE(model.rowCount(), 2);
}

void tst_ToDoListModel::setNoteIdSameValueDoesNotEmit() {
  ToDoListModel model;
  const int id = openList(model, "List");
  QSignalSpy changedSpy(&model, &ToDoListModel::noteIDChanged);
  model.setNoteID(id);
  QCOMPARE(changedSpy.count(), 0);
}

void tst_ToDoListModel::switchingListReplacesTasks() {
  const int a = DBManager::instance()->addNote("A");
  const int b = DBManager::instance()->addNote("B");
  DBManager::instance()->addNoteContent(a, "a1");
  DBManager::instance()->addNoteContent(b, "b1");
  DBManager::instance()->addNoteContent(b, "b2");
  ToDoListModel model;
  model.setNoteID(a);
  QCOMPARE(model.rowCount(), 1);
  model.setNoteID(b);
  QCOMPARE(model.rowCount(), 2);
  model.setNoteID(a);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.data(model.index(0), ToDoListModel::ItemNameRole).toString(),
           QString("a1"));
}

void tst_ToDoListModel::dataForEachRole() {
  ToDoListModel model;
  const int list = openList(model, "List");
  const int task = DBManager::instance()->addNoteContent(list, "Write tests");
  DBManager::instance()->updateNoteContent(task, true);
  model.fetchListFromDB();
  const QModelIndex idx = model.index(0);
  QCOMPARE(model.data(idx, ToDoListModel::IdRole).toInt(), task);
  QCOMPARE(model.data(idx, ToDoListModel::ItemNameRole).toString(),
           QString("Write tests"));
  QCOMPARE(model.data(idx, ToDoListModel::StatusRole).toBool(), true);
  QVERIFY(!model.data(idx, 999999).isValid());
}

void tst_ToDoListModel::invalidIndexIsFalsy() {
  ToDoListModel model;
  // The model returns int 0 (not an invalid QVariant) for bad indexes.
  QVERIFY(!model.data(QModelIndex(), ToDoListModel::ItemNameRole).toBool());
  QVERIFY(!model.data(model.index(2), ToDoListModel::ItemNameRole).toBool());
}

void tst_ToDoListModel::rowCountOfValidParentIsZero() {
  ToDoListModel model;
  const int list = openList(model, "List");
  DBManager::instance()->addNoteContent(list, "t");
  model.fetchListFromDB();
  QCOMPARE(model.rowCount(model.index(0)), 0);
}

void tst_ToDoListModel::addItemToListStoresAndResets() {
  ToDoListModel model;
  const int list = openList(model, "List");
  QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
  model.addItemToList("Task A");
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.data(model.index(0), ToDoListModel::ItemNameRole).toString(),
           QString("Task A"));
  QCOMPARE(model.data(model.index(0), ToDoListModel::StatusRole).toBool(),
           false);
  QCOMPARE(DBManager::instance()->getNoteContents(list).size(), 1);
}

void tst_ToDoListModel::addItemToListLogsTaskAdded() {
  ToDoListModel model;
  openList(model, "Groceries");
  model.addItemToList("Milk");
  const auto rows = TestEnv::logsOfType("TASK_ADDED");
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_description").toString(),
           QString("{\"NoteName\":\"Groceries\",\"TaskName\":\"Milk\"}"));
}

void tst_ToDoListModel::removeItemFromListRemovesRowAndDbRow() {
  ToDoListModel model;
  const int list = openList(model, "List");
  model.addItemToList("only");
  QSignalSpy removedSpy(&model, &QAbstractItemModel::rowsRemoved);
  model.removeItemFromList(0);
  QCOMPARE(removedSpy.count(), 1);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(DBManager::instance()->getNoteContents(list).size(), 0);
}

void tst_ToDoListModel::removeItemFromListLogsTaskDeleted() {
  ToDoListModel model;
  openList(model, "Groceries");
  model.addItemToList("Milk");
  model.removeItemFromList(0);
  const auto rows = TestEnv::logsOfType("TASK_DELETED");
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_description").toString(),
           QString("{\"NoteName\":\"Groceries\",\"TaskName\":\"Milk\"}"));
}

void tst_ToDoListModel::toggleTaskStatusUpdatesModelAndDb() {
  ToDoListModel model;
  const int list = openList(model, "List");
  model.addItemToList("task");
  QSignalSpy changedSpy(&model, &QAbstractItemModel::dataChanged);
  model.toggleTaskStatus(0, true);
  QCOMPARE(changedSpy.count(), 1);
  QCOMPARE(changedSpy.first().at(2).value<QVector<int>>(),
           QVector<int>({ToDoListModel::StatusRole}));
  QVERIFY(model.data(model.index(0), ToDoListModel::StatusRole).toBool());
  QVERIFY(DBManager::instance()->getNoteContents(list).first()
              .value("completed").toBool());
  model.toggleTaskStatus(0, false);
  QVERIFY(!model.data(model.index(0), ToDoListModel::StatusRole).toBool());
  QVERIFY(!DBManager::instance()->getNoteContents(list).first()
               .value("completed").toBool());
}

void tst_ToDoListModel::toggleTaskStatusLogsWithSuffix() {
  ToDoListModel model;
  openList(model, "Work");
  model.addItemToList("Email");
  model.toggleTaskStatus(0, true);
  model.toggleTaskStatus(0, false);
  const auto rows = TestEnv::logsOfType("TASK_STATUS_TOGGLED");
  QCOMPARE(rows.size(), 2);
  QStringList descs;
  for (const QVariantMap &r : rows)
    descs << r.value("event_description").toString();
  descs.sort();
  QCOMPARE(descs,
           QStringList({"{\"NoteName\":\"Work\",\"TaskName\":\"Email:0\"}",
                        "{\"NoteName\":\"Work\",\"TaskName\":\"Email:1\"}"}));
}

void tst_ToDoListModel::toggleOnlyAffectsSelectedRow() {
  ToDoListModel model;
  openList(model, "List");
  model.addItemToList("one");
  model.addItemToList("two");
  QCOMPARE(model.rowCount(), 2);
  // Same-second rows tie on created_at, so do not assume their order.
  const int row =
      model.data(model.index(0), ToDoListModel::ItemNameRole).toString() ==
              "two"
          ? 0
          : 1;
  model.toggleTaskStatus(row, true);
  QVERIFY(model.data(model.index(row), ToDoListModel::StatusRole).toBool());
  QVERIFY(!model.data(model.index(1 - row), ToDoListModel::StatusRole)
               .toBool());
}

QTEST_GUILESS_MAIN(tst_ToDoListModel)
#include "tst_todolistmodel.moc"
