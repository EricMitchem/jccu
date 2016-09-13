#ifndef JCCU_SOURCE_EDIT_GOOD_DIALOG_H
#define JCCU_SOURCE_EDIT_GOOD_DIALOG_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <QDialog>
#include <QString>

namespace Ui {class EditGoodDialog;}

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Edit good dialog.
///////////////////////////////////////////////////////////////////////////////
class EditGoodDialog : public QDialog
{
  public:
    explicit EditGoodDialog(QWidget* parent = nullptr);
    ~EditGoodDialog();

    void setText(const QString& text);

    QString good() const;

  private:
    EditGoodDialog(const EditGoodDialog&);
    EditGoodDialog& operator=(const EditGoodDialog&);

    Ui::EditGoodDialog* ui_;
};

} // namespace jccu

#endif // JCCU_SOURCE_EDIT_GOOD_DIALOG_H