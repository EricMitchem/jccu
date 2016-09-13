///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include "window.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QEvent>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>
#include "application.h"
#include "calendar_dialog.h"
#include "can_manager.h"
#include "edit_date_dialog.h"
#include "edit_good_dialog.h"
#include "ui_Window.h"

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Constructor.
///////////////////////////////////////////////////////////////////////////////
Window::Window()
  : ui_(new Ui::Window)
{
  can_context_menu_ = new QMenu(this);

  QAction* editGood = new QAction(can_context_menu_);
  editGood->setObjectName("actionEditGood");
  editGood->setText("Edit &good");

  QAction* editDate = new QAction(can_context_menu_);
  editDate->setObjectName("actionEditDate");
  editDate->setText("Edit &date");

  QAction* remove =  new QAction(can_context_menu_);
  remove->setObjectName("actionRemoveCan");
  remove->setText("&Remove");

  can_context_menu_->addAction(editGood);
  can_context_menu_->addAction(editDate);
  can_context_menu_->addAction(remove);

  // Perform all manual ui setup before this. Then we don't have to make an
  // explicit call to connectSlotsByName.
  ui_->setupUi(this);

  auto good_manager = Application::Instance()->goodManager();
  auto can_manager = Application::Instance()->canManager();

  connect(ui_->actionQuit, &QAction::triggered,
          qApp, &QApplication::quit);

  ui_->addGoodComboBox->setModel(good_manager);
  ui_->addGoodComboBox->setModelColumn(1);
  ui_->dateDateEdit->setDate(QDate::currentDate());

  ui_->removeGoodComboBox->setModel(good_manager);
  ui_->removeGoodComboBox->setModelColumn(1);

  ui_->tableView->installEventFilter(this); // Catch context menu events.
  ui_->tableView->setModel(can_manager);
  ui_->tableView->horizontalHeader()
                ->setSectionResizeMode(QHeaderView::Stretch);

  setWindowTitle("jccu " + Application::Version());
}


///////////////////////////////////////////////////////////////////////////////
/// Destructor.
///////////////////////////////////////////////////////////////////////////////
Window::~Window()
{
  delete ui_;
}


///////////////////////////////////////////////////////////////////////////////
/// Filters events.
///////////////////////////////////////////////////////////////////////////////
bool Window::eventFilter(QObject* object, QEvent* event)
{
  if(object == ui_->tableView) {
    if(event->type() == QEvent::ContextMenu) {
      auto context_menu_event = static_cast<QContextMenuEvent*>(event);
      can_context_menu_->exec(context_menu_event->globalPos());
      return true;
    }
    else {
      return QObject::eventFilter(object, event);
    }
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
/// Select the cans in the table view by id. Clears the current selection.
///////////////////////////////////////////////////////////////////////////////
void Window::selectCans(const QList<int>& can_ids)
{
  ui_->tableView->clearSelection();

  auto can_manager = Application::Instance()->canManager();
  auto selection_model = ui_->tableView->selectionModel();

  auto it = can_ids.constBegin(),
       end = can_ids.constEnd();

  for(; it != end; ++it)
    selection_model->select(can_manager->index(can_manager->row(*it), 0),
                            QItemSelectionModel::Select |
                            QItemSelectionModel::Rows);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns a list of cans that are selected in the table view.
///////////////////////////////////////////////////////////////////////////////
QList<int> Window::selectedCans() const
{
  auto can_manager = Application::Instance()->canManager();

  auto index_list = ui_->tableView->selectionModel()->selectedRows();
  QList<int> can_ids; {
    auto it = index_list.constBegin(),
         end = index_list.constEnd();

    for(; it != end; ++it)
      can_ids.append(can_manager->data(*it).toInt());
  }

  return can_ids;
}


///////////////////////////////////////////////////////////////////////////////
/// Submits the can data as a new can.
///////////////////////////////////////////////////////////////////////////////
void Window::on_addCanButton_clicked()
{
  QString good = ui_->addGoodComboBox->currentText();
  int64_t date = ui_->dateDateEdit->date().toJulianDay();
  int id_hint = ui_->idHintSpinBox->value();

  Application::Instance()->canManager()->add(good, date, &id_hint);
  ui_->idHintSpinBox->setValue(0);

  auto can_manager = Application::Instance()->canManager();
  ui_->tableView->setFocus(Qt::OtherFocusReason);
  ui_->tableView->selectRow(can_manager->lastRowAdded());
}


///////////////////////////////////////////////////////////////////////////////
/// Pops a dialog to add a new good.
///////////////////////////////////////////////////////////////////////////////
void Window::on_addGoodToolButton_clicked()
{
  bool ok;
  QString good = QInputDialog::getText(this,              // Parent
                                       "Add Good",        // Title
                                       "Good:",           // Label
                                       QLineEdit::Normal, // Echo mode
                                       "",                // Default text
                                       &ok);              // Ok | Cancel

  if(!ok || good.isEmpty())
    return;

  // Must be alphabetical, whitespace allowed.
  // 1/13/2015: Noticed bug. regex isn't constrained enough; it will accept only whitespace.
  if(good.contains(QRegExp("[^a-zA-Z\\s]+")))
    return;

  auto good_manager = Application::Instance()->goodManager();
  good_manager->add(good);

  int index = ui_->addGoodComboBox->findText(good);
  ui_->addGoodComboBox->setCurrentIndex(index);
}


///////////////////////////////////////////////////////////////////////////////
/// Pops a calendar dialog for date selection.
///////////////////////////////////////////////////////////////////////////////
void Window::on_newDateToolButton_clicked()
{
  CalendarDialog calendar_dialog(this);
  calendar_dialog.setSelectedDate(ui_->dateDateEdit->date());

  if(calendar_dialog.exec() == QDialog::Accepted)
    ui_->dateDateEdit->setDate(calendar_dialog.selectedDate());
}


///////////////////////////////////////////////////////////////////////////////
/// Pops a dialog to remove a good.
/// Warns the user if the removal would also delete any cans.
///////////////////////////////////////////////////////////////////////////////
void Window::on_removeGoodButton_clicked()
{
  auto good_manager = Application::Instance()->goodManager();
  auto can_manager = Application::Instance()->canManager();
  QString good = ui_->removeGoodComboBox->currentText();
  int good_id = good_manager->id(good);
  int can_count = can_manager->goodRefCount(good_id);

  QString text = QString("Are you sure you want to remove '%1'?").arg(good);
  QString informative_text = QString(
                             "This will also delete %1 cans."
                             ).arg(can_count);
  
  QMessageBox message_box;
  message_box.setText(text);
  message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  message_box.setDefaultButton(QMessageBox::Cancel);
  message_box.setIcon(QMessageBox::Warning);
  
  if(can_count)
    message_box.setInformativeText(informative_text);

  if(message_box.exec() == QMessageBox::Ok) {
    can_manager->removeByGood(good_id);
    good_manager->remove(good);
    ui_->removeGoodComboBox->setCurrentIndex(0);
  }
}


///////////////////////////////////////////////////////////////////////////////
/// Edits the good of all the selected cans.
///////////////////////////////////////////////////////////////////////////////
void Window::on_actionEditGood_triggered()
{
  auto can_ids = selectedCans();
  int can_count = can_ids.count();

  EditGoodDialog dialog(this);
  dialog.setText(QString(
                 "Are you sure you want to change the good of %1 can%2?"
                 ).arg(can_count).arg((can_count > 1) ? "s" : ""));

  if(dialog.exec() == QDialog::Accepted) {
    auto can_manager = Application::Instance()->canManager();
    QString good = dialog.good();
    auto it = can_ids.constBegin(),
         end = can_ids.constEnd();

    for(; it != end; ++it)
      can_manager->editGood(*it, good);

    // After sorting, the selections are invalid. Reselect.
    can_manager->sort();
    selectCans(can_ids);
  }
}


///////////////////////////////////////////////////////////////////////////////
/// Edits the date of all the selected cans.
///////////////////////////////////////////////////////////////////////////////
void Window::on_actionEditDate_triggered()
{
  auto can_ids = selectedCans();
  int can_count = can_ids.count();

  EditDateDialog dialog(this);
  dialog.setText(QString(
                 "Are you sure you want to change the date of %1 can%2?"
                 ).arg(can_count).arg((can_count > 1) ? "s" : ""));

  if(dialog.exec() == QDialog::Accepted) {
    auto can_manager = Application::Instance()->canManager();
    int64_t date = dialog.selectedDate().toJulianDay();
    auto it = can_ids.constBegin(),
         end = can_ids.constEnd();

    for(; it != end; ++it)
      can_manager->editDate(*it, date);

    // After sorting, the selections are invalid. Reselect.
    can_manager->sort();
    selectCans(can_ids);
  }
}


///////////////////////////////////////////////////////////////////////////////
/// Removes the cans that are selected in the table view.
///////////////////////////////////////////////////////////////////////////////
void Window::on_actionRemoveCan_triggered()
{
  auto can_ids = selectedCans();
  int can_count = can_ids.count();

  QMessageBox message_box;
  message_box.setText(QString(
                      "Are you sure you want to delete %1 can%2?"
                      ).arg(can_count).arg((can_count > 1) ? "s" : ""));
  message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  message_box.setDefaultButton(QMessageBox::Cancel);
  message_box.setIcon(QMessageBox::Warning);

  if(message_box.exec() == QMessageBox::Ok) {
    auto can_manager = Application::Instance()->canManager();
    auto it = can_ids.constBegin(),
         end = can_ids.constEnd();

    for(; it != end; ++it)
      can_manager->remove(*it);
  }
}

} // namespace jccu
