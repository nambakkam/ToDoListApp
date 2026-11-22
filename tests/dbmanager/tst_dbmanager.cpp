#include "dbmanager.h"
#include "testenv.h"

/**
 * @class tst_DBManager
 * @brief Unit tests for DBManager against a throw-away SQLite database.
 *
 * Covers schema creation and CRUD for notes (lists), note contents (tasks)
 * and event logs. The singleton cannot be reset, so every test starts by
 * emptying the tables.
 */
class tst_DBManager : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();
  void init();

  void instanceIsSingleton();
  void usesSandboxDatabase();
  void schemaCreatesTables();
  void openDbWhenAlreadyOpenSucceeds();

  void addNoteReturnsIncreasingIds();
  void getAllNotesReturnsFields();
  void updateNoteTitle();
  void deleteNote();
  void getNoteNameUnknownIsEmpty();

  void addNoteContentAndGet();
  void noteContentsAreScopedToNote();
  void updateNoteContentToggles();
  void deleteNoteContent();
  void deleteAllNoteContents();

  void addEventLogAndGet();

private:
  TestEnv m_env;
};

void tst_DBManager::initTestCase() { QVERIFY(m_env.setUp()); }

void tst_DBManager::cleanupTestCase() { m_env.tearDown(); }

void tst_DBManager::init() { TestEnv::clearTables(); }

void tst_DBManager::instanceIsSingleton() {
  QVERIFY(DBManager::instance() != nullptr);
  QCOMPARE(DBManager::instance(), DBManager::instance());
}

void tst_DBManager::usesSandboxDatabase() {
  QVERIFY(QFileInfo::exists(m_env.path() + "/database.db"));
  QCOMPARE(QSqlDatabase::database().databaseName(), QString("./database.db"));
  QCOMPARE(QDir::currentPath(), QDir(m_env.path()).absolutePath());
}

void tst_DBManager::schemaCreatesTables() {
  const QStringList tables = QSqlDatabase::database().tables();
  QVERIFY(tables.contains("Notes"));
  QVERIFY(tables.contains("NotesContents"));
  QVERIFY(tables.contains("eventLogs"));
}

void tst_DBManager::openDbWhenAlreadyOpenSucceeds() {
  QVERIFY(DBManager::instance()->openDB("./ignored.db"));
  QVERIFY(!QFileInfo::exists(m_env.path() + "/ignored.db"));
}

void tst_DBManager::addNoteReturnsIncreasingIds() {
  const int a = DBManager::instance()->addNote("first");
  const int b = DBManager::instance()->addNote("second");
  QVERIFY(a > 0);
  QVERIFY(b > a);
}

void tst_DBManager::getAllNotesReturnsFields() {
  const int id = DBManager::instance()->addNote("groceries");
  const auto notes = DBManager::instance()->getAllNotes();
  QCOMPARE(notes.size(), 1);
  QCOMPARE(notes.first().value("note_id").toInt(), id);
  QCOMPARE(notes.first().value("title").toString(), QString("groceries"));
  QVERIFY(!notes.first().value("created_at").toString().isEmpty());
}

void tst_DBManager::updateNoteTitle() {
  const int id = DBManager::instance()->addNote("old");
  QVERIFY(DBManager::instance()->updateNoteTitle(id, "new"));
  QCOMPARE(DBManager::instance()->getNoteName(id), QString("new"));
}

void tst_DBManager::deleteNote() {
  const int keep = DBManager::instance()->addNote("keep");
  const int drop = DBManager::instance()->addNote("drop");
  QVERIFY(DBManager::instance()->deleteNote(drop));
  const auto notes = DBManager::instance()->getAllNotes();
  QCOMPARE(notes.size(), 1);
  QCOMPARE(notes.first().value("note_id").toInt(), keep);
}

void tst_DBManager::getNoteNameUnknownIsEmpty() {
  QVERIFY(DBManager::instance()->getNoteName(987654).isEmpty());
}

void tst_DBManager::addNoteContentAndGet() {
  const int note = DBManager::instance()->addNote("list");
  const int c = DBManager::instance()->addNoteContent(note, "buy milk");
  QVERIFY(c > 0);
  const auto rows = DBManager::instance()->getNoteContents(note);
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("id").toInt(), c);
  QCOMPARE(rows.first().value("note_id").toInt(), note);
  QCOMPARE(rows.first().value("content").toString(), QString("buy milk"));
  QVERIFY(!rows.first().value("completed").toBool());
}

void tst_DBManager::noteContentsAreScopedToNote() {
  const int n1 = DBManager::instance()->addNote("one");
  const int n2 = DBManager::instance()->addNote("two");
  DBManager::instance()->addNoteContent(n1, "a");
  DBManager::instance()->addNoteContent(n2, "b");
  DBManager::instance()->addNoteContent(n2, "c");
  QCOMPARE(DBManager::instance()->getNoteContents(n1).size(), 1);
  QCOMPARE(DBManager::instance()->getNoteContents(n2).size(), 2);
  QCOMPARE(DBManager::instance()->getNoteContents(-1).size(), 0);
}

void tst_DBManager::updateNoteContentToggles() {
  const int note = DBManager::instance()->addNote("list");
  const int c = DBManager::instance()->addNoteContent(note, "task");
  QVERIFY(DBManager::instance()->updateNoteContent(c, true));
  auto rows = DBManager::instance()->getNoteContents(note);
  QVERIFY(rows.first().value("completed").toBool());
  QVERIFY(DBManager::instance()->updateNoteContent(c, false));
  rows = DBManager::instance()->getNoteContents(note);
  QVERIFY(!rows.first().value("completed").toBool());
}

void tst_DBManager::deleteNoteContent() {
  const int note = DBManager::instance()->addNote("list");
  const int c1 = DBManager::instance()->addNoteContent(note, "t1");
  DBManager::instance()->addNoteContent(note, "t2");
  QVERIFY(DBManager::instance()->deleteNoteContent(c1));
  const auto rows = DBManager::instance()->getNoteContents(note);
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("content").toString(), QString("t2"));
}

void tst_DBManager::deleteAllNoteContents() {
  const int n1 = DBManager::instance()->addNote("one");
  const int n2 = DBManager::instance()->addNote("two");
  DBManager::instance()->addNoteContent(n1, "a");
  DBManager::instance()->addNoteContent(n1, "b");
  DBManager::instance()->addNoteContent(n2, "c");
  QVERIFY(DBManager::instance()->deleteAllNoteContents(n1));
  QCOMPARE(DBManager::instance()->getNoteContents(n1).size(), 0);
  QCOMPARE(DBManager::instance()->getNoteContents(n2).size(), 1);
}

void tst_DBManager::addEventLogAndGet() {
  const int id = DBManager::instance()->addEventLog("NOTE_CREATED",
                                                    "{\"NoteName\":\"x\"}");
  QVERIFY(id > 0);
  const auto logs = DBManager::instance()->getEventLogs();
  QCOMPARE(logs.size(), 1);
  QCOMPARE(logs.first().value("id").toInt(), id);
  QCOMPARE(logs.first().value("event_type").toString(),
           QString("NOTE_CREATED"));
  QCOMPARE(logs.first().value("event_description").toString(),
           QString("{\"NoteName\":\"x\"}"));
  QVERIFY(!logs.first().value("created_at").toString().isEmpty());
}

QTEST_GUILESS_MAIN(tst_DBManager)
#include "tst_dbmanager.moc"
