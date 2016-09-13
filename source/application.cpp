//////////////////////////////////////////////////////////////////////////////
/// Includes
//////////////////////////////////////////////////////////////////////////////
#include "application.h"

#include <QApplication>
#include <QFile>
#include <QTimer>
#include "can_manager.h"
#include "json_manager.h"
#include "system_tray_icon.h"
#include "window.h"

namespace jccu
{

Application* Application::Instance_ = nullptr;


//////////////////////////////////////////////////////////////////////////////
/// Constructor.
//////////////////////////////////////////////////////////////////////////////
Application::Application()
  : goods_(nullptr),
    dates_(nullptr),
    cans_(nullptr),
    json_(nullptr),
    icon_(nullptr),
    window_(nullptr)
{
}


//////////////////////////////////////////////////////////////////////////////
/// Destructor.
//////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
  delete goods_;
  delete dates_;
  delete cans_;
  delete json_;
}


//////////////////////////////////////////////////////////////////////////////
/// Runs the application and returns the exit code once finished.
//////////////////////////////////////////////////////////////////////////////
int Application::run(int argc, char** argv)
{
  QApplication qapp(argc, argv);

  QTimer::singleShot(0, this, SLOT(load()));
  return qapp.exec();
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the good manager.
///////////////////////////////////////////////////////////////////////////////
GoodManager* Application::goodManager() const
{
  return goods_;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the date manager.
///////////////////////////////////////////////////////////////////////////////
DateManager* Application::dateManager() const
{
  return dates_;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the can manager.
///////////////////////////////////////////////////////////////////////////////
CanManager* Application::canManager() const
{
  return cans_;
}


//////////////////////////////////////////////////////////////////////////////
/// Returns the application instance.
//////////////////////////////////////////////////////////////////////////////
Application* Application::Instance()
{
  return Instance_;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the current version.
///////////////////////////////////////////////////////////////////////////////
QString Application::Version()
{
  const int Major = 1;
  const int Minor = 0;
  const int Patch = 11;

  if(Patch)
    return QString("%1.%2.%3").arg(Major).arg(Minor).arg(Patch);
  else
    return QString("%1.%2").arg(Major).arg(Minor);
}


//////////////////////////////////////////////////////////////////////////////
/// Loads the application.
//////////////////////////////////////////////////////////////////////////////
void Application::load()
{ 
  Instance_ = this;

  goods_ = new GoodManager;
  dates_ = new DateManager;
  cans_  = new CanManager(goods_, dates_);

  json_ = new JsonManager("can_data.json", goods_, dates_, cans_);
  json_->read();

  icon_ = new SystemTrayIcon(this);

  window_ = new Window;
  window_->show();

  auto expiration_timer = new QTimer(this);
  expiration_timer->setObjectName("expirationTimer");
  expiration_timer->setInterval(1000 * 60 * 60 * 24); // 1 day.
  expiration_timer->start();
  on_expirationTimer_timeout(); // Manual invoke on application startup.

  qApp->setQuitOnLastWindowClosed(false);

  connect(QApplication::instance(), &QApplication::aboutToQuit,
          window_, &QObject::deleteLater);
  connect(QApplication::instance(), &QApplication::aboutToQuit,
          this, &Application::save);

  QMetaObject::connectSlotsByName(this);
}


///////////////////////////////////////////////////////////////////////////////
/// Saves the application data.
///////////////////////////////////////////////////////////////////////////////
void Application::save()
{
  json_->write();
}


//////////////////////////////////////////////////////////////////////////////
/// Toggles the window on double-clicks.
//////////////////////////////////////////////////////////////////////////////
void Application::on_systemTrayIcon_doubleClicked()
{
  window_->setVisible(!window_->isVisible());
  
  if(window_->isVisible()) {
    window_->activateWindow();
    window_->raise();
  }
}


//////////////////////////////////////////////////////////////////////////////
/// Pops a system tray icon warning if any cans are expiring soon.
//////////////////////////////////////////////////////////////////////////////
void Application::on_expirationTimer_timeout()
{
  int expired_count = cans_->expiringWithin(0);
  int expiring_count = cans_->expiringWithin(7) - expired_count;

  if(!expiring_count && !expired_count)
    return;

  QString expired("%1 can%2 %3 expired.");
  expired = expired.arg(expired_count).
                    arg((expired_count != 1) ? "s" : "").
                    arg((expired_count != 1) ? "have" : "has");

  QString expiring("%1 can%2 will expire soon.");
  expiring = expiring.arg(expiring_count).
                      arg((expiring_count != 1) ? "s" : "");

  QString message;

  if(expired_count)
    message = expired;

  if(expiring_count) {
    if(expired_count)
      message.append("\n");

    message.append(expiring);
  }

  icon_->showWarning(message);
}

} // namespace jccu
