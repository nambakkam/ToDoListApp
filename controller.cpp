#include "controller.h"
#include "dbmanager.h"
#include "eventlogsmodel.h"
#include "logger.h"
#include "todolistmodel.h"
#include "todonotesmodel.h"

/**
 * @brief Creates the controller.
 *
 * @param tasksModel Model of the tasks of the currently open list.
 * @param listsModel Model of all lists.
 * @param logsModel Model of the audit log.
 * @param parent Optional QObject parent. The models are not owned.
 */
Controller::Controller(ToDoListModel *tasksModel, TODONotesModel *listsModel,
                       EventLogsModel *logsModel, QObject *parent)
    : QObject{parent}, m_tasksModel{tasksModel}, m_listsModel{listsModel},
      m_logsModel{logsModel} {}

/**
 * @brief Creates a new list.
 *
 * Inserts the list into the database, logs NOTE_CREATED and reloads the lists
 * model. Blank names and database failures are ignored.
 *
 * @param name The name of the new list.
 */
void Controller::addList(const QString &name) {
  const QString trimmed = name.trimmed();
  if (trimmed.isEmpty())
    return;
  if (DBManager::instance()->addNote(trimmed) < 0)
    return;
  Logger::instance().logEvent(Logger::NOTE_CREATED, trimmed);
  m_listsModel->fetchAllNotesFromDB();
}

/**
 * @brief Deletes a list together with all of its tasks.
 *
 * Deletes the tasks first (DBManager::deleteAllNoteContents) and then the list
 * itself, logs NOTE_DELETED and removes the row from the lists model. If the
 * deleted list is the one currently open, the tasks model is cleared.
 *
 * @param noteId The id of the list to delete.
 */
void Controller::removeList(int noteId) {
  const QString name = DBManager::instance()->getNoteName(noteId);
  if (!DBManager::instance()->deleteAllNoteContents(noteId))
    return;
  if (!DBManager::instance()->deleteNote(noteId))
    return;
  Logger::instance().logEvent(Logger::NOTE_DELETED, name);
  m_listsModel->removeById(noteId);
  if (m_tasksModel->getNoteID() == noteId)
    m_tasksModel->setNoteID(-1);
}

/**
 * @brief Opens a list so the tasks model shows its tasks.
 *
 * @param noteId The id of the list to open.
 */
void Controller::openList(int noteId) { m_tasksModel->setNoteID(noteId); }

/**
 * @brief Adds a task to the currently open list.
 *
 * Inserts the task, logs TASK_ADDED and reloads the tasks model. Blank text,
 * no open list and database failures are ignored.
 *
 * @param text The task description.
 */
void Controller::addTask(const QString &text) {
  const int noteId = m_tasksModel->getNoteID();
  const QString trimmed = text.trimmed();
  if (noteId < 0 || trimmed.isEmpty())
    return;
  if (DBManager::instance()->addNoteContent(noteId, trimmed) < 0)
    return;
  Logger::instance().logEvent(
      Logger::TASK_ADDED, DBManager::instance()->getNoteName(noteId), trimmed);
  m_tasksModel->fetchListFromDB();
}

/**
 * @brief Deletes a task from the currently open list.
 *
 * Deletes the task from the database, logs TASK_DELETED and removes the row
 * from the tasks model.
 *
 * @param taskId The id of the task to delete.
 */
void Controller::removeTask(int taskId) {
  const QString taskName = m_tasksModel->itemNameById(taskId);
  if (!DBManager::instance()->deleteNoteContent(taskId))
    return;
  Logger::instance().logEvent(
      Logger::TASK_DELETED,
      DBManager::instance()->getNoteName(m_tasksModel->getNoteID()), taskName);
  m_tasksModel->removeById(taskId);
}

/**
 * @brief Sets the completion state of a task.
 *
 * Persists the state, logs TASK_STATUS_TOGGLED (the task name gets the new
 * state appended as `:0` or `:1`) and updates the tasks model.
 *
 * @param taskId The id of the task.
 * @param completed The new completion state.
 */
void Controller::toggleTask(int taskId, bool completed) {
  const QString taskName = m_tasksModel->itemNameById(taskId);
  if (!DBManager::instance()->updateNoteContent(taskId, completed))
    return;
  Logger::instance().logEvent(
      Logger::TASK_STATUS_TOGGLED,
      DBManager::instance()->getNoteName(m_tasksModel->getNoteID()),
      taskName + QString(":%1").arg(completed));
  m_tasksModel->setStatusById(taskId, completed);
}

/**
 * @brief Reloads the audit log so the logs page shows the latest events.
 */
void Controller::showLogs() { m_logsModel->refresh(); }
