///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include "can_manager.h"

#include <algorithm>
#include <QBrush>
#include <QDate>

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Constructor.
///////////////////////////////////////////////////////////////////////////////
CanManager::CanManager(GoodManager* good_manager, DateManager* date_manager)
  : goods_(good_manager),
    dates_(date_manager),
    last_row_added_(0)
{
}


///////////////////////////////////////////////////////////////////////////////
/// Destructor.
///////////////////////////////////////////////////////////////////////////////
CanManager::~CanManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Adds the can. If the good or date don't exist, they'll be created.
/// Gets the can id hint from can_id_hint if it's non-null.
/// Outputs the can id chosen to can_id_hint, if it's non-null.
/// Returns true on success.
///////////////////////////////////////////////////////////////////////////////
bool CanManager::add(const QString& good, int64_t date, int* can_id_hint)
{
  int good_id, date_id;

  // Good already exists. Get the id manually.
  if(!goods_->add(good, &good_id))
    good_id = goods_->id(good);

  // Same as above.
  if(!dates_->add(date, &date_id))
    date_id = dates_->id(date);

  int hint = (can_id_hint) ? *can_id_hint : 0;
  int can_id = nextId(hint);

  if(can_id_hint)
    *can_id_hint = can_id;

  return insert(can_id, good_id, date_id);
}


///////////////////////////////////////////////////////////////////////////////
/// Inserts the can if all ids are valid.
/// good_id and date_id must exist; can_id must not. No id may be < 1.
/// Returns true on success.
///////////////////////////////////////////////////////////////////////////////
bool CanManager::insert(int can_id, int good_id, int date_id)
{
  if(can_id < 1 || good_id < 1 || date_id < 1)
    return false;

  if(!goods_->exists(good_id))
    return false;

  if(!dates_->exists(date_id))
    return false;

  if(exists(can_id))
    return false;

  int new_row = cans_list_.size();

  beginInsertRows(QModelIndex(), new_row, new_row);
    cans_goods_container_.insert(can_id, good_id);
    cans_dates_container_.insert(can_id, date_id);
    cans_list_.append(can_id);
  endInsertRows();

  sort();
  last_row_added_ = cans_list_.indexOf(can_id);

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Changes the good of the given can to new_good.
/// new_good must already exist.
///////////////////////////////////////////////////////////////////////////////
bool CanManager::editGood(int can_id, const QString& new_good)
{
  if(!exists(can_id))
    return false;

  if(!goods_->exists(new_good))
    return false;

  int good_id = goods_->id(new_good);

  if(good_id == cans_goods_container_.value(can_id))
    return false;

  // Good id is column 1.
  auto good_index = index(cans_list_.indexOf(can_id), 1);
  cans_goods_container_[can_id] = good_id;
  emit dataChanged(good_index, good_index);
  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Changes the date of the given can to new_date.
/// new_date must already exist.
///////////////////////////////////////////////////////////////////////////////
bool CanManager::editDate(int can_id, int64_t new_date)
{
  if(!exists(can_id))
    return false;

  if(!dates_->exists(new_date))
    dates_->add(new_date);

  int date_id = dates_->id(new_date);
  int old_date_id = cans_dates_container_.value(can_id);

  if(date_id == old_date_id)
    return false;

  // Date id is column 2.
  auto date_index = index(cans_list_.indexOf(can_id), 2);
  cans_dates_container_[can_id] = date_id;
  emit dataChanged(date_index, date_index);

  if(!dateRefCount(old_date_id))
    dates_->remove(dates_->date(old_date_id));

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Removes the can if it exists.
/// Returns true on success.
///////////////////////////////////////////////////////////////////////////////
bool CanManager::remove(int id)
{
  if(!exists(id))
    return false;

  int index = cans_list_.indexOf(id);
  int date_id = cans_dates_container_.value(id);

  beginRemoveRows(QModelIndex(), index, index);
    cans_goods_container_.remove(id);
    cans_dates_container_.remove(id);
    cans_list_.removeOne(id);
  endRemoveRows();

  if(!dateRefCount(date_id))
    dates_->remove(dates_->date(date_id));

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Removes all cans with good id good_id.
/// Returns the number of cans removed.
///////////////////////////////////////////////////////////////////////////////
int CanManager::removeByGood(int good_id)
{
  auto can_ids = cans_goods_container_.keys(good_id);
  auto it = can_ids.constBegin(),
       end = can_ids.constEnd();

  for(; it != end; ++it)
    remove(*it);

  return can_ids.count();
}


///////////////////////////////////////////////////////////////////////////////
/// Clears all cans.
///////////////////////////////////////////////////////////////////////////////
void CanManager::clear()
{
  const int size = cans_list_.size();

  if(!size)
    return;

  beginRemoveRows(QModelIndex(), 0, size - 1);
    cans_goods_container_.clear();
    cans_dates_container_.clear();
    cans_list_.clear();
  endRemoveRows();
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the can id, good, or date at the given index.
///////////////////////////////////////////////////////////////////////////////
QVariant CanManager::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();

  if(index.column() >= columnCount())
    return QVariant();

  if(index.row() >= cans_list_.size())
    return QVariant();

  int can_id = cans_list_.at(index.row());
  int good_id = cans_goods_container_.value(can_id);
  int date_id = cans_dates_container_.value(can_id);
  QDate can_date = QDate::fromJulianDay(dates_->date(date_id));
  int days_to_expiration = QDate::currentDate().daysTo(can_date);

  switch(role) {
    case Qt::DisplayRole:
      if(index.column() == 0)
        return can_id;
      else if(index.column() == 1)
        return goods_->good(good_id);
      else // if(index.column() == 2)
        return can_date;
      
    case Qt::ForegroundRole:
      if(days_to_expiration <= 0)
        return QBrush(Qt::red);
      else if(days_to_expiration <= 7)
        return QBrush(QColor(255, 127, 0)); // Orange
      else if(days_to_expiration <= 30)
        return QBrush(QColor(205, 0, 205)); // Purple
      else
        return QBrush(Qt::black);

    case IdRole:
      if(index.column() == 0)
        return can_id;
      else if(index.column() == 1)
        return good_id;
      else // if(index.column() == 2)
        return date_id;

    default:
      return QVariant();
  }
}


///////////////////////////////////////////////////////////////////////////////
/// Header data.
///////////////////////////////////////////////////////////////////////////////
QVariant CanManager::headerData(int section,
                                Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole)
    return QVariant();

  if(orientation != Qt::Horizontal)
    return QVariant();

  if(section == 0)
    return QString("Can Id");
  else if(section == 1)
    return QString("Good");
  else if(section == 2)
    return QString("Expires");
  else
    return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of columns.
///////////////////////////////////////////////////////////////////////////////
int CanManager::columnCount(const QModelIndex& parent) const
{
  // CanId, Good, Date
  return 3;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of rows.
///////////////////////////////////////////////////////////////////////////////
int CanManager::rowCount(const QModelIndex& parent) const
{
  return cans_list_.size();
}


///////////////////////////////////////////////////////////////////////////////
/// Sorts by date, then good, and finally can id. Ascending.
///////////////////////////////////////////////////////////////////////////////
void CanManager::sort(int column, Qt::SortOrder order)
{
  auto Compare = [this](int can_id_a, int can_id_b) -> bool {
    int64_t date_a = dates_->date(cans_dates_container_.value(can_id_a)),
            date_b = dates_->date(cans_dates_container_.value(can_id_b));

    if(date_a != date_b)
      return date_a < date_b;

    QString good_a = goods_
                     ->good(cans_goods_container_.value(can_id_a)).toLower(),
            good_b = goods_
                     ->good(cans_goods_container_.value(can_id_b)).toLower();

    if(good_a != good_b)
      return good_a < good_b;

    return can_id_a < can_id_b;
  };

  std::sort(cans_list_.begin(), cans_list_.end(), Compare);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns true if the can exists.
///////////////////////////////////////////////////////////////////////////////
bool CanManager::exists(int id) const
{
  return cans_list_.contains(id);
}


//////////////////////////////////////////////////////////////////////////////
/// Returns the number of cans expiring within days days.
//////////////////////////////////////////////////////////////////////////////
int CanManager::expiringWithin(int days) const
{
  auto current_date = QDate::currentDate();
  auto it = cans_list_.constBegin(),
       end = cans_list_.constEnd();
  int matches = 0;

  for(; it != end; ++it) {
    auto date = QDate::fromJulianDay(
                       dates_->date(cans_dates_container_.value(*it)));
    int days_to_expiration = current_date.daysTo(date);

    if(days_to_expiration <= days)
      ++matches;
  }

  return matches;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of cans that reference the good with id good_id.
///////////////////////////////////////////////////////////////////////////////
int CanManager::goodRefCount(int good_id) const
{
  auto good_ids = cans_goods_container_.values();
  return good_ids.count(good_id);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of cans that reference the good with id good_id.
///////////////////////////////////////////////////////////////////////////////
int CanManager::dateRefCount(int date_id) const
{
  auto date_ids = cans_dates_container_.values();
  return date_ids.count(date_id);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the row of the given can.
/// Returns zero if the can doesn't exist.
///////////////////////////////////////////////////////////////////////////////
int CanManager::row(int can_id) const
{
  int row = 0;

  if(exists(can_id))
    row = cans_list_.indexOf(can_id);

  return row;
}


//////////////////////////////////////////////////////////////////////////////
/// Returns the last row that was added or zero if none have been added yet.
//////////////////////////////////////////////////////////////////////////////
int CanManager::lastRowAdded() const
{
  return last_row_added_;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the smallest available can id.
/// It will return hint if it's available and > 0.
///////////////////////////////////////////////////////////////////////////////
int CanManager::nextId(int hint) const
{
  if(hint > 0 && !exists(hint))
    return hint;

  auto can_ids = cans_list_;
  std::sort(can_ids.begin(), can_ids.end());

  int i = 1;
  const int size = can_ids.size();

  for(; i <= size; ++i)
    if(i < can_ids[i - 1])
      break;

  return i;
}

} // namespace jccu