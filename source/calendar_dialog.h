#ifndef JCCU_SOURCE_CALENDAR_DIALOG_H
#define JCCU_SOURCE_CALENDAR_DIALOG_H

//////////////////////////////////////////////////////////////////////////////
/// Includes
//////////////////////////////////////////////////////////////////////////////
#include <QDate>
#include <QDialog>

namespace Ui {class CalendarDialog;}

namespace jccu
{

//////////////////////////////////////////////////////////////////////////////
/// Calendar dialog.
//////////////////////////////////////////////////////////////////////////////
class CalendarDialog : public QDialog
{
  public:
    explicit CalendarDialog(QWidget* parent = nullptr);
    ~CalendarDialog();

    void setSelectedDate(const QDate& date);
    QDate selectedDate() const;

  private:
    CalendarDialog(const CalendarDialog&);
    CalendarDialog& operator=(const CalendarDialog&);

    Ui::CalendarDialog* ui_;
};

} // namespace jccu

#endif // JCCU_SOURCE_CALENDAR_DIALOG_H