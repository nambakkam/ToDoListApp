#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>

class EventLogsModel;
class ToDoListModel;
class TODONotesModel;

/**
 * @class Controller
 * @brief Controller of the MVC pattern: the single place where data changes.
 *
 * QML calls the Q_INVOKABLE methods of this class (exposed as the context
 * property `controller`). Each mutating method writes to the database through
 * DBManager, records an audit event through Logger and then tells the
 * relevant model to update itself, so the models only have to present data.
 *
 * Methods take database ids (not row indexes) so a stale or reordered view
 * can never target the wrong row.
 *
 * @note UI terms differ from code terms: a "list" is a note (Notes table,
 * TODONotesModel) and a "task" is a note content (NotesContents table,
 * ToDoListModel).
 *
 * @see ToDoListModel
 * @see TODONotesModel
 * @see EventLogsModel
 */
class Controller : public QObject {
  Q_OBJECT
public:
  explicit Controller(ToDoListModel *tasksModel, TODONotesModel *listsModel,
                      EventLogsModel *logsModel, QObject *parent = nullptr);

  Q_INVOKABLE void addList(const QString &name);
  Q_INVOKABLE void removeList(int noteId);
  Q_INVOKABLE void openList(int noteId);

  Q_INVOKABLE void addTask(const QString &text);
  Q_INVOKABLE void removeTask(int taskId);
  Q_INVOKABLE void toggleTask(int taskId, bool completed);

  Q_INVOKABLE void showLogs();

private:
  ToDoListModel *m_tasksModel;
  TODONotesModel *m_listsModel;
  EventLogsModel *m_logsModel;
};

#endif // CONTROLLER_H
