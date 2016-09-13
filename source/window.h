#ifndef JCCU_SOURCE_WINDOW_H
#define JCCU_SOURCE_WINDOW_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <QList>
#include <QMainWindow>

namespace Ui {class Window;}
class QMenu;

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Main window.
///////////////////////////////////////////////////////////////////////////////
class Window : public QMainWindow
{
  Q_OBJECT

  public:
    Window();
    ~Window();

  private:
    Window(const Window&);
    Window& operator=(const Window&);

    bool eventFilter(QObject* object, QEvent* event);

    void selectCans(const QList<int>& can_ids);
    QList<int> selectedCans() const;

    Ui::Window* ui_;
    QMenu* can_context_menu_;

  private slots:
    void on_addCanButton_clicked();
    void on_addGoodToolButton_clicked();
    void on_newDateToolButton_clicked();
    void on_removeGoodButton_clicked();
    void on_actionEditGood_triggered();
    void on_actionEditDate_triggered();
    void on_actionRemoveCan_triggered();
};

} // namespace jccu

#endif // JCCU_SOURCE_WINDOW_H
