//////////////////////////////////////////////////////////////////////////////
/// Includes
//////////////////////////////////////////////////////////////////////////////
#include "system_tray_icon.h"

#include <QApplication>
#include <QMenu>

#include <QMessageBox>

namespace jccu
{

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
//////////////////////////////////////////////////////////////////////////////
SystemTrayIcon::SystemTrayIcon(QObject* parent)
  : QSystemTrayIcon(parent)
{
  auto menu = new QMenu;
  menu->addAction("&Exit", qApp, SLOT(quit()));

  setObjectName("systemTrayIcon");
  setIcon(QIcon(":/assets/icons/jccu"));
  setContextMenu(menu);
  show();

  QMetaObject::connectSlotsByName(this);
}


//////////////////////////////////////////////////////////////////////////////
/// Destructor.
//////////////////////////////////////////////////////////////////////////////
SystemTrayIcon::~SystemTrayIcon()
{
  delete contextMenu();
}


//////////////////////////////////////////////////////////////////////////////
/// Pops a warning message from the system tray icon.
//////////////////////////////////////////////////////////////////////////////
void SystemTrayIcon::showWarning(const QString& warning)
{
  if(!supportsMessages())
    return;

  showMessage("jccu", warning, Warning);
}


//////////////////////////////////////////////////////////////////////////////
/// Handles activation.
//////////////////////////////////////////////////////////////////////////////
void SystemTrayIcon::on_systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
  if(reason == DoubleClick)
    emit doubleClicked();
}

} // namespace jccu