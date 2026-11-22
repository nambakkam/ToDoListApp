#include "todonotesmodel.h"
#include "testenv.h"
#include <QSignalSpy>

/**
 * @class tst_TODONotesModel
 * @brief Unit tests for TODONotesModel, the model of lists on the home page.
 *
 * Checks roles, add/remove behaviour (in-memory rows, database rows and the
 * audit log entries) and documents the known bug that deleting a list leaves
 * its tasks behind.
 */
class tst_TODONotesModel : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();
  void init();

  void emptyModel();
  void roleNames();
  void headerData();
  void fetchPicksUpExistingRows();
  void addNoteToListStoresAndResets();
  void addNoteToListLogsNoteCreated();
  void dataForEachRole();
  void creationTimeIsParsed();
  void invalidIndexIsFalsy();
  void rowCountOfValidParentIsZero();
  void removeNoteFromListRemovesRowAndDbRow();
  void removeNoteFromListLogsNoteDeleted();
  void removeOnlyAffectsSelectedRow();
  void removeNoteAlsoRemovesItsTasks();

private:
  /** @brief Index of the row whose NoteName equals @p name, or -1. */
  static int rowOf(const TODONotesModel &model, const QString &name);

  TestEnv m_env;
};

void tst_TODONotesModel::initTestCase() { QVERIFY(m_env.setUp()); }

void tst_TODONotesModel::cleanupTestCase() { m_env.tearDown(); }

void tst_TODONotesModel::init() { TestEnv::clearTables(); }

int tst_TODONotesModel::rowOf(const TODONotesModel &model,
                              const QString &name) {
  for (int i = 0; i < model.rowCount(); ++i) {
    if (model.data(model.index(i), TODONotesModel::ItemNameRole).toString() ==
        name)
      return i;
  }
  return -1;
}

void tst_TODONotesModel::emptyModel() {
  TODONotesModel model;
  QCOMPARE(model.rowCount(), 0);
}

void tst_TODONotesModel::roleNames() {
  TODONotesModel model;
  const auto names = model.roleNames();
  QCOMPARE(names.value(TODONotesModel::noteIDRole), QByteArray("NoteID"));
  QCOMPARE(names.value(TODONotesModel::ItemNameRole), QByteArray("NoteName"));
  QCOMPARE(names.value(TODONotesModel::TimeStampRole),
           QByteArray("CreatedAt"));
  QCOMPARE(names.size(), 3);
}

void tst_TODONotesModel::headerData() {
  TODONotesModel model;
  QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("Note"));
  QCOMPARE(model.headerData(2, Qt::Vertical).toString(), QString("3"));
  QVERIFY(!model.headerData(0, Qt::Horizontal, Qt::EditRole).isValid());
}

void tst_TODONotesModel::fetchPicksUpExistingRows() {
  DBManager::instance()->addNote("alpha");
  DBManager::instance()->addNote("beta");
  TODONotesModel model;
  QCOMPARE(model.rowCount(), 0);
  QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
  model.fetchAllNotesFromDB();
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(rowOf(model, "alpha") >= 0);
  QVERIFY(rowOf(model, "beta") >= 0);
}

void tst_TODONotesModel::addNoteToListStoresAndResets() {
  TODONotesModel model;
  QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
  model.addNoteToList("Chores");
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 1);
  const auto notes = DBManager::instance()->getAllNotes();
  QCOMPARE(notes.size(), 1);
  QCOMPARE(notes.first().value("title").toString(), QString("Chores"));
}

void tst_TODONotesModel::addNoteToListLogsNoteCreated() {
  TODONotesModel model;
  model.addNoteToList("Chores");
  const auto rows = TestEnv::logsOfType("NOTE_CREATED");
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_description").toString(),
           QString("{\"NoteName\":\"Chores\"}"));
}

void tst_TODONotesModel::dataForEachRole() {
  const int id = DBManager::instance()->addNote("Work");
  TODONotesModel model;
  model.fetchAllNotesFromDB();
  const QModelIndex idx = model.index(0);
  QCOMPARE(model.data(idx, TODONotesModel::noteIDRole).toInt(), id);
  QCOMPARE(model.data(idx, TODONotesModel::ItemNameRole).toString(),
           QString("Work"));
  QVERIFY(!model.data(idx, 999999).isValid());
}

void tst_TODONotesModel::creationTimeIsParsed() {
  DBManager::instance()->addNote("Work");
  TODONotesModel model;
  model.fetchAllNotesFromDB();
  const QVariant stamp =
      model.data(model.index(0), TODONotesModel::TimeStampRole);
  // The DB stores "yyyy-MM-dd HH:mm:ss" but the model parses it with
  // QDateTime::fromString() without a format, which yields an invalid date.
  QEXPECT_FAIL("", "Known bug: CreatedAt is an invalid QDateTime because "
                   "fromString() is called without a format", Continue);
  QVERIFY(stamp.toDateTime().isValid());
}

void tst_TODONotesModel::invalidIndexIsFalsy() {
  TODONotesModel model;
  // The model returns int 0 (not an invalid QVariant) for bad indexes.
  QVERIFY(!model.data(QModelIndex(), TODONotesModel::ItemNameRole).toBool());
  QVERIFY(!model.data(model.index(3), TODONotesModel::ItemNameRole).toBool());
}

void tst_TODONotesModel::rowCountOfValidParentIsZero() {
  DBManager::instance()->addNote("x");
  TODONotesModel model;
  model.fetchAllNotesFromDB();
  QCOMPARE(model.rowCount(model.index(0)), 0);
}

void tst_TODONotesModel::removeNoteFromListRemovesRowAndDbRow() {
  TODONotesModel model;
  model.addNoteToList("Temp");
  QSignalSpy removedSpy(&model, &QAbstractItemModel::rowsRemoved);
  model.removeNoteFromList(0);
  QCOMPARE(removedSpy.count(), 1);
  QCOMPARE(removedSpy.first().at(1).toInt(), 0);
  QCOMPARE(removedSpy.first().at(2).toInt(), 0);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(DBManager::instance()->getAllNotes().size(), 0);
}

void tst_TODONotesModel::removeNoteFromListLogsNoteDeleted() {
  TODONotesModel model;
  model.addNoteToList("Temp");
  model.removeNoteFromList(0);
  const auto rows = TestEnv::logsOfType("NOTE_DELETED");
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_description").toString(),
           QString("{\"NoteName\":\"Temp\"}"));
}

void tst_TODONotesModel::removeOnlyAffectsSelectedRow() {
  TODONotesModel model;
  model.addNoteToList("one");
  model.addNoteToList("two");
  model.addNoteToList("three");
  const int row = rowOf(model, "two");
  QVERIFY(row >= 0);
  model.removeNoteFromList(row);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowOf(model, "two"), -1);
  QVERIFY(rowOf(model, "one") >= 0);
  QVERIFY(rowOf(model, "three") >= 0);
  QCOMPARE(DBManager::instance()->getAllNotes().size(), 2);
}

void tst_TODONotesModel::removeNoteAlsoRemovesItsTasks() {
  TODONotesModel model;
  model.addNoteToList("WithTasks");
  const int noteId = model.data(model.index(0), TODONotesModel::noteIDRole)
                         .toInt();
  DBManager::instance()->addNoteContent(noteId, "orphan candidate");
  model.removeNoteFromList(0);
  QEXPECT_FAIL("", "Known bug: removeNoteFromList never calls "
                   "deleteAllNoteContents(), so tasks are left behind",
               Continue);
  QCOMPARE(DBManager::instance()->getNoteContents(noteId).size(), 0);
}

QTEST_GUILESS_MAIN(tst_TODONotesModel)
#include "tst_todonotesmodel.moc"
