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
 * Initializes the Qt application, sets up high DPI scaling for Qt versions below 6,
 * creates and initializes the database manager, models for ToDo list, notes, and event logs,
 * and fetches all notes from the database. Sets up the QML application engine,
 * exposes the models to QML context, and loads the main QML file.
 * Handles application exit if the QML root object fails to load.
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
