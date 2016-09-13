#ifndef JCCU_SOURCE_DATE_MANAGER_H
#define JCCU_SOURCE_DATE_MANAGER_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <QAbstractTableModel>
#include <QHash>
#include <QList>
#include <QObject>

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Manages all dates.
///////////////////////////////////////////////////////////////////////////////
class DateManager : public QAbstractTableModel
{
  public:
    DateManager();
    ~DateManager();

    bool add(int64_t date, int* id = nullptr);
    bool insert(int date_id, int64_t date);
    
    bool remove(int64_t date);
    void clear();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    bool exists(int date_id) const;
    bool exists(int64_t date) const;

    int64_t date(int date_id) const;
    int id(int64_t date) const;
    int nextId() const;

  private:
    DateManager(const DateManager&);
    DateManager& operator=(const DateManager&);

    QHash<int, int64_t> fwd_dates_container_; // <DateId, Date>
    QHash<int64_t, int> rev_dates_container_; // <Date, DateId>
    QList<int64_t> dates_list_;               // <Date>
};

} // namespace jccu

#endif // JCCU_SOURCE_DATE_MANAGER_H