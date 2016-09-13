#ifndef JCCU_SOURCE_APPLICATION_H
#define JCCU_SOURCE_APPLICATION_H

//////////////////////////////////////////////////////////////////////////////
/// Includes
//////////////////////////////////////////////////////////////////////////////
#include <QObject>
#include <QString>

namespace jccu
{

class GoodManager;
class DateManager;
class CanManager;
class JsonManager;
class SystemTrayIcon;
class Window;

//////////////////////////////////////////////////////////////////////////////
/// Master class.
//////////////////////////////////////////////////////////////////////////////
class Application : public QObject
{
  Q_OBJECT

  public:
    Application();
    ~Application();

    int run(int argc, char** argv);

    GoodManager* goodManager() const;
    DateManager* dateManager() const;
    CanManager* canManager() const;

    static Application* Instance();
    static QString Version();

  private:
    Application(const Application&);
    Application& operator=(const Application&);

    GoodManager* goods_;
    DateManager* dates_;
    CanManager* cans_;
    JsonManager* json_;
    SystemTrayIcon* icon_;
    Window* window_;

    static Application* Instance_;

  private slots:
    void load();
    void save();

    void on_systemTrayIcon_doubleClicked();
    void on_expirationTimer_timeout();
};

} // namespace jccu

#endif // JCCU_SOURCE_APPLICATION_H
