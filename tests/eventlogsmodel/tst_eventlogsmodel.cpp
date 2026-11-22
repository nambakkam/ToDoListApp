#include "eventlogsmodel.h"
#include "logger.h"
#include "testenv.h"
#include <QSignalSpy>

/**
 * @class tst_EventLogsModel
 * @brief Unit tests for EventLogsModel roles, JSON parsing and refresh().
 *
 * The model loads at construction and on refresh() only, so tests build the
 * model after inserting rows and check that later inserts stay invisible
 * until refresh().
 */
class tst_EventLogsModel : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();
  void init();

  void emptyModel();
  void roleNames();
  void dataParsesJsonFields();
  void missingTaskNameIsEmpty();
  void malformedJsonGivesEmptyNames();
  void invalidIndexReturnsInvalidVariant();
  void unknownRoleReturnsInvalidVariant();
  void rowCountOfValidParentIsZero();
  void loadsOnlyAtConstructionAndRefresh();
  void refreshEmitsModelReset();
  void reflectsLoggerOutput();

private:
  TestEnv m_env;
};

void tst_EventLogsModel::initTestCase() { QVERIFY(m_env.setUp()); }

void tst_EventLogsModel::cleanupTestCase() { m_env.tearDown(); }

void tst_EventLogsModel::init() { TestEnv::clearTables(); }

void tst_EventLogsModel::emptyModel() {
  EventLogsModel model;
  QCOMPARE(model.rowCount(), 0);
}

void tst_EventLogsModel::roleNames() {
  EventLogsModel model;
  const auto names = model.roleNames();
  QCOMPARE(names.value(EventLogsModel::IdRole), QByteArray("id"));
  QCOMPARE(names.value(EventLogsModel::EventTypeRole),
           QByteArray("eventType"));
  QCOMPARE(names.value(EventLogsModel::NoteNameRole), QByteArray("noteName"));
  QCOMPARE(names.value(EventLogsModel::TaskNameRole), QByteArray("taskName"));
  QCOMPARE(names.value(EventLogsModel::TimestampRole),
           QByteArray("timestamp"));
  QCOMPARE(names.size(), 5);
}

void tst_EventLogsModel::dataParsesJsonFields() {
  const int id = DBManager::instance()->addEventLog(
      "TASK_ADDED", "{\"NoteName\":\"Home\",\"TaskName\":\"Sweep\"}");
  EventLogsModel model;
  QCOMPARE(model.rowCount(), 1);
  const QModelIndex idx = model.index(0);
  QCOMPARE(model.data(idx, EventLogsModel::IdRole).toInt(), id);
  QCOMPARE(model.data(idx, EventLogsModel::EventTypeRole).toString(),
           QString("TASK_ADDED"));
  QCOMPARE(model.data(idx, EventLogsModel::NoteNameRole).toString(),
           QString("Home"));
  QCOMPARE(model.data(idx, EventLogsModel::TaskNameRole).toString(),
           QString("Sweep"));
  QVERIFY(
      !model.data(idx, EventLogsModel::TimestampRole).toString().isEmpty());
}

void tst_EventLogsModel::missingTaskNameIsEmpty() {
  DBManager::instance()->addEventLog("NOTE_CREATED",
                                     "{\"NoteName\":\"Home\"}");
  EventLogsModel model;
  const QModelIndex idx = model.index(0);
  QCOMPARE(model.data(idx, EventLogsModel::NoteNameRole).toString(),
           QString("Home"));
  QCOMPARE(model.data(idx, EventLogsModel::TaskNameRole).toString(),
           QString());
}

void tst_EventLogsModel::malformedJsonGivesEmptyNames() {
  DBManager::instance()->addEventLog("NOTE_CREATED", "not json at all");
  EventLogsModel model;
  const QModelIndex idx = model.index(0);
  QCOMPARE(model.data(idx, EventLogsModel::NoteNameRole).toString(),
           QString());
  QCOMPARE(model.data(idx, EventLogsModel::TaskNameRole).toString(),
           QString());
  QCOMPARE(model.data(idx, EventLogsModel::EventTypeRole).toString(),
           QString("NOTE_CREATED"));
}

void tst_EventLogsModel::invalidIndexReturnsInvalidVariant() {
  DBManager::instance()->addEventLog("NOTE_CREATED", "{\"NoteName\":\"a\"}");
  EventLogsModel model;
  QVERIFY(!model.data(QModelIndex(), EventLogsModel::IdRole).isValid());
  QVERIFY(!model.data(model.index(5), EventLogsModel::IdRole).isValid());
}

void tst_EventLogsModel::unknownRoleReturnsInvalidVariant() {
  DBManager::instance()->addEventLog("NOTE_CREATED", "{\"NoteName\":\"a\"}");
  EventLogsModel model;
  QVERIFY(!model.data(model.index(0), Qt::UserRole + 100).isValid());
}

void tst_EventLogsModel::rowCountOfValidParentIsZero() {
  DBManager::instance()->addEventLog("NOTE_CREATED", "{\"NoteName\":\"a\"}");
  EventLogsModel model;
  QCOMPARE(model.rowCount(model.index(0)), 0);
}

void tst_EventLogsModel::loadsOnlyAtConstructionAndRefresh() {
  DBManager::instance()->addEventLog("NOTE_CREATED", "{\"NoteName\":\"a\"}");
  EventLogsModel model;
  QCOMPARE(model.rowCount(), 1);
  DBManager::instance()->addEventLog("NOTE_CREATED", "{\"NoteName\":\"b\"}");
  QCOMPARE(model.rowCount(), 1);
  model.refresh();
  QCOMPARE(model.rowCount(), 2);
}

void tst_EventLogsModel::refreshEmitsModelReset() {
  EventLogsModel model;
  QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
  model.refresh();
  QCOMPARE(resetSpy.count(), 1);
}

void tst_EventLogsModel::reflectsLoggerOutput() {
  Logger::instance().logEvent(Logger::TASK_STATUS_TOGGLED, "Work", "Email:1");
  EventLogsModel model;
  QCOMPARE(model.rowCount(), 1);
  const QModelIndex idx = model.index(0);
  QCOMPARE(model.data(idx, EventLogsModel::EventTypeRole).toString(),
           QString("TASK_STATUS_TOGGLED"));
  QCOMPARE(model.data(idx, EventLogsModel::NoteNameRole).toString(),
           QString("Work"));
  QCOMPARE(model.data(idx, EventLogsModel::TaskNameRole).toString(),
           QString("Email:1"));
}

QTEST_GUILESS_MAIN(tst_EventLogsModel)
#include "tst_eventlogsmodel.moc"
