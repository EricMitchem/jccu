#ifndef JCCU_SOURCE_CAN_MANAGER_H
#define JCCU_SOURCE_CAN_MANAGER_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <QAbstractTableModel>
#include <QHash>
#include "date_manager.h"
#include "good_manager.h"

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Manages all cans. This class has terrible container names. Sorry.
///////////////////////////////////////////////////////////////////////////////
class CanManager : public QAbstractTableModel
{
  public:
    enum ItemRole {
      IdRole = Qt::UserRole + 1
    };

    CanManager(GoodManager* good_manager, DateManager* date_manager);
    ~CanManager();

    bool add(const QString& good, int64_t date, int* can_id_hint = nullptr);
    bool insert(int can_id, int good_id, int date_id);

    bool editGood(int can_id, const QString& new_good);
    bool editDate(int can_id, int64_t new_date);

    bool remove(int id);
    int removeByGood(int good_id);
    void clear();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder);

    bool exists(int id) const;
    int expiringWithin(int days) const;
    int goodRefCount(int good_id) const;
    int dateRefCount(int date_id) const;
    int row(int can_id) const;
    int lastRowAdded() const;
    
    int nextId(int hint) const;

  private:
    CanManager(const CanManager&);
    CanManager& operator=(const CanManager&);

    QHash<int, int> cans_goods_container_; // <CanId, GoodId>
    QHash<int, int> cans_dates_container_; // <CanId, DateId>
    QList<int> cans_list_;                 // <CanId>
    GoodManager* goods_;
    DateManager* dates_;
    int last_row_added_;
};

} // namespace jccu

#endif // JCCU_SOURCE_CAN_MANAGER_H