/**
 * @file main.cpp
 * @brief Application entry point: creates the models and loads the QML UI.
 */
#include "dbmanager.h"
#include "eventlogsmodel.h"
#include "todolistmodel.h"
#include "todonotesmodel.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

/**
 * @brief Entry point for the MVCPatternExample Qt application.
 *
 * Enables high DPI scaling for Qt versions below 6, creates the
 * QGuiApplication, then DBManager::instance() (which opens `./database.db`
 * and runs the schema file relative to the working directory). The three
 * models are stack-allocated: ToDoListModel (tasks), TODONotesModel (lists)
 * and EventLogsModel (audit log; it loads itself on construction). The lists
 * are fetched explicitly, then the models are exposed to QML as the context
 * properties `todoModel`, `todoNotesModel` and `eventLogsModel`, and
 * `qrc:/main.qml` is loaded. If that root object fails to load, the
 * application exits with code -1.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Application exit code.
 */
int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  QGuiApplication app(argc, argv);
  DBManager *dbManager = DBManager::instance();
  Q_UNUSED(dbManager);
  ToDoListModel todoModel;
  TODONotesModel todoNotesModel;
  EventLogsModel logsModel;
  todoNotesModel.fetchAllNotesFromDB();
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("todoModel", &todoModel);
  engine.rootContext()->setContextProperty("todoNotesModel", &todoNotesModel);
  engine.rootContext()->setContextProperty("eventLogsModel", &logsModel);
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
