#ifndef JCCU_SOURCE_JSON_MANAGER_H
#define JCCU_SOURCE_JSON_MANAGER_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <QString>

namespace jccu
{

class GoodManager;
class DateManager;
class CanManager;

///////////////////////////////////////////////////////////////////////////////
/// Manages json file reading and writing.
///////////////////////////////////////////////////////////////////////////////
class JsonManager
{
  public:
    JsonManager(const QString& file_name,
                GoodManager* good_manager,
                DateManager* date_manager,
                CanManager* can_manager);
    ~JsonManager();

    bool read();
    bool write() const;

  private:
    JsonManager(const JsonManager&);
    JsonManager& operator=(const JsonManager&);

    bool valid() const;

    QString file_name_;
    GoodManager* goods_;
    DateManager* dates_;
    CanManager* cans_;
};

} // namespace jccu

#endif // JCCU_SOURCE_JSON_MANAGER_H