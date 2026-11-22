#include "logger.h"
#include "testenv.h"
#include <QJsonDocument>
#include <QJsonObject>

/**
 * @class tst_Logger
 * @brief Unit tests for Logger event rows (type name and JSON payload).
 *
 * Verifies that each Logger::EventType is stored by enumerator name and that
 * event_description is compact JSON with NoteName and an optional TaskName.
 */
class tst_Logger : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();
  void init();

  void instanceIsSingleton();
  void storesEnumeratorName_data();
  void storesEnumeratorName();
  void descriptionIsCompactJsonWithoutTask();
  void descriptionIncludesTaskName();
  void statusToggledAppendsState();
  void specialCharactersAreEscaped();
  void everyCallAddsOneRow();

private:
  TestEnv m_env;
};

void tst_Logger::initTestCase() { QVERIFY(m_env.setUp()); }

void tst_Logger::cleanupTestCase() { m_env.tearDown(); }

void tst_Logger::init() { TestEnv::clearTables(); }

void tst_Logger::instanceIsSingleton() {
  QCOMPARE(&Logger::instance(), &Logger::instance());
}

void tst_Logger::storesEnumeratorName_data() {
  QTest::addColumn<int>("type");
  QTest::addColumn<QString>("name");
  QTest::newRow("created") << int(Logger::NOTE_CREATED) << "NOTE_CREATED";
  QTest::newRow("deleted") << int(Logger::NOTE_DELETED) << "NOTE_DELETED";
  QTest::newRow("updated") << int(Logger::NOTE_UPDATED) << "NOTE_UPDATED";
  QTest::newRow("added") << int(Logger::TASK_ADDED) << "TASK_ADDED";
  QTest::newRow("task deleted") << int(Logger::TASK_DELETED) << "TASK_DELETED";
  QTest::newRow("toggled") << int(Logger::TASK_STATUS_TOGGLED)
                           << "TASK_STATUS_TOGGLED";
}

void tst_Logger::storesEnumeratorName() {
  QFETCH(int, type);
  QFETCH(QString, name);
  Logger::instance().logEvent(Logger::EventType(type), "n");
  const auto rows = DBManager::instance()->getEventLogs();
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_type").toString(), name);
}

void tst_Logger::descriptionIsCompactJsonWithoutTask() {
  Logger::instance().logEvent(Logger::NOTE_CREATED, "Shopping");
  const auto rows = DBManager::instance()->getEventLogs();
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_description").toString(),
           QString("{\"NoteName\":\"Shopping\"}"));
}

void tst_Logger::descriptionIncludesTaskName() {
  Logger::instance().logEvent(Logger::TASK_ADDED, "Shopping", "Milk");
  const auto rows = TestEnv::logsOfType("TASK_ADDED");
  QCOMPARE(rows.size(), 1);
  QCOMPARE(rows.first().value("event_description").toString(),
           QString("{\"NoteName\":\"Shopping\",\"TaskName\":\"Milk\"}"));
}

void tst_Logger::statusToggledAppendsState() {
  Logger::instance().logEvent(Logger::TASK_STATUS_TOGGLED, "L", "T:1");
  Logger::instance().logEvent(Logger::TASK_STATUS_TOGGLED, "L", "T:0");
  const auto rows = TestEnv::logsOfType("TASK_STATUS_TOGGLED");
  QCOMPARE(rows.size(), 2);
  QStringList tasks;
  for (const QVariantMap &r : rows) {
    const auto doc = QJsonDocument::fromJson(
        r.value("event_description").toString().toUtf8());
    tasks << doc.object().value("TaskName").toString();
  }
  tasks.sort();
  QCOMPARE(tasks, QStringList({"T:0", "T:1"}));
}

void tst_Logger::specialCharactersAreEscaped() {
  const QString note = QString::fromUtf8("say \"hi\" \\ \xC3\xA9");
  Logger::instance().logEvent(Logger::NOTE_UPDATED, note, "tab\there");
  const auto rows = DBManager::instance()->getEventLogs();
  QCOMPARE(rows.size(), 1);
  const QByteArray json =
      rows.first().value("event_description").toString().toUtf8();
  QJsonParseError err;
  const auto doc = QJsonDocument::fromJson(json, &err);
  QCOMPARE(err.error, QJsonParseError::NoError);
  QCOMPARE(doc.object().value("NoteName").toString(), note);
  QCOMPARE(doc.object().value("TaskName").toString(), QString("tab\there"));
}

void tst_Logger::everyCallAddsOneRow() {
  for (int i = 0; i < 3; ++i)
    Logger::instance().logEvent(Logger::NOTE_CREATED, QString::number(i));
  QCOMPARE(DBManager::instance()->getEventLogs().size(), 3);
}

QTEST_GUILESS_MAIN(tst_Logger)
#include "tst_logger.moc"
