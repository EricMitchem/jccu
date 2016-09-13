#ifndef JCCU_SOURCE_EDIT_DATE_DIALOG_H
#define JCCU_SOURCE_EDIT_DATE_DIALOG_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <QDate>
#include <QDialog>
#include <QString>

namespace Ui {class EditDateDialog;}

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Edit date dialog.
///////////////////////////////////////////////////////////////////////////////
class EditDateDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit EditDateDialog(QWidget* parent = nullptr);
    ~EditDateDialog();

    void setText(const QString& text);

    QDate selectedDate() const;

  private:
    EditDateDialog(const EditDateDialog&);
    EditDateDialog& operator=(const EditDateDialog&);

    Ui::EditDateDialog* ui_;

  private slots:
    void on_toolButton_clicked();
};

} // namespac jccu

#endif // JCCU_SOURCE_EDIT_DATE_DIALOG_H