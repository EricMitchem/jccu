#ifndef JCCU_SOURCE_SYSTEM_TRAY_ICON_H
#define JCCU_SOURCE_SYSTEM_TRAY_ICON_H

//////////////////////////////////////////////////////////////////////////////
/// Includes
//////////////////////////////////////////////////////////////////////////////
#include <QString>
#include <QSystemTrayIcon>

namespace jccu
{

//////////////////////////////////////////////////////////////////////////////
/// System tray icon.
//////////////////////////////////////////////////////////////////////////////
class SystemTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT

  public:
    explicit SystemTrayIcon(QObject* parent = nullptr);
    ~SystemTrayIcon();

    void showWarning(const QString& warning);

  signals:
    void doubleClicked();

  private:
    SystemTrayIcon(const SystemTrayIcon&);
    SystemTrayIcon& operator=(const SystemTrayIcon&);

  private slots:
    void on_systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason);
};

} // namespace jccu

#endif // JCCU_SOURCE_SYSTEM_TRAY_ICON_H