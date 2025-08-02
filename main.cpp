#include "dbmanager.h"
#include "eventlogsmodel.h"
#include "todolistmodel.h"
#include "todonotesmodel.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

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
