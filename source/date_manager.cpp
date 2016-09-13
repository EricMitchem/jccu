///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include "date_manager.h"

#include <algorithm>

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Constructor.
///////////////////////////////////////////////////////////////////////////////
DateManager::DateManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Destructor.
///////////////////////////////////////////////////////////////////////////////
DateManager::~DateManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Adds the date if it doesn't exist.
/// Outputs the chosen date id if id is non-null.
/// Returns true on success.
///////////////////////////////////////////////////////////////////////////////
bool DateManager::add(int64_t date, int* id)
{
  if(exists(date))
    return false;

  int date_id = nextId();

  if(id)
    *id = date_id;

  return insert(date_id, date);
}


///////////////////////////////////////////////////////////////////////////////
/// Inserts date date with id date_id, if neither exists and date_id > 0.
///////////////////////////////////////////////////////////////////////////////
bool DateManager::insert(int date_id, int64_t date)
{
  if(date_id < 1)
    return false;

  if(!date)
    return false;

  if(exists(date_id) || exists(date))
    return false;

  int new_row = dates_list_.size();

  beginInsertRows(QModelIndex(), new_row, new_row);
    fwd_dates_container_.insert(date_id, date);
    rev_dates_container_.insert(date, date_id);
    dates_list_.append(date);
  endInsertRows();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Removes the date.
///////////////////////////////////////////////////////////////////////////////
bool DateManager::remove(int64_t date)
{
  if(!date)
    return false;

  if(!exists(date))
    return false;

  int index = dates_list_.indexOf(date);

  beginRemoveRows(QModelIndex(), index, index);
    fwd_dates_container_.remove(id(date));
    rev_dates_container_.remove(date);
    dates_list_.removeOne(date);
  endRemoveRows();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Clears all dates.
///////////////////////////////////////////////////////////////////////////////
void DateManager::clear()
{
  const int size = dates_list_.size();

  if(!size)
    return;

  beginRemoveRows(QModelIndex(), 0, size - 1);
    fwd_dates_container_.clear();
    rev_dates_container_.clear();
    dates_list_.clear();
  endRemoveRows();
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the date id or date at the given index.
///////////////////////////////////////////////////////////////////////////////
QVariant DateManager::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();

  if(role != Qt::DisplayRole)
    return QVariant();

  if(index.column() >= columnCount())
    return QVariant();

  if(index.row() >= dates_list_.size())
    return QVariant();

  int64_t date = dates_list_.at(index.row());
  int date_id = id(date);

  if(index.column() == 0)
    return date_id;
  else
    return date;
}


///////////////////////////////////////////////////////////////////////////////
/// Header data.
///////////////////////////////////////////////////////////////////////////////
QVariant DateManager::headerData(int section,
                                 Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole)
    return QVariant();

  if(orientation != Qt::Horizontal)
    return QVariant();

  if(section == 0)
    return QString("Id");
  else
    return QString("Date");
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of columns.
///////////////////////////////////////////////////////////////////////////////
int DateManager::columnCount(const QModelIndex& parent) const
{
  // DateId, Date
  return 2;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of rows.
///////////////////////////////////////////////////////////////////////////////
int DateManager::rowCount(const QModelIndex& parent) const
{
  return dates_list_.size();
}


///////////////////////////////////////////////////////////////////////////////
/// Returns true if a date with date_id exists.
///////////////////////////////////////////////////////////////////////////////
bool DateManager::exists(int date_id) const
{
  return fwd_dates_container_.contains(date_id);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns true if the date exists.
///////////////////////////////////////////////////////////////////////////////
bool DateManager::exists(int64_t date) const
{
  return rev_dates_container_.contains(date);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the date with the given id.
///////////////////////////////////////////////////////////////////////////////
int64_t DateManager::date(int date_id) const
{
  int64_t date = 0;

  if(exists(date_id))
    date = fwd_dates_container_.value(date_id);

  return date;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the id of the date or zero if the date doesn't exist.
///////////////////////////////////////////////////////////////////////////////
int DateManager::id(int64_t date) const
{
  int date_id = 0;

  if(exists(date))
    date_id = rev_dates_container_.value(date);

  return date_id;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the smallest available date id.
///////////////////////////////////////////////////////////////////////////////
int DateManager::nextId() const
{
  auto date_ids = fwd_dates_container_.keys();
  std::sort(date_ids.begin(), date_ids.end());

  int i = 1;
  const int size = date_ids.size();

  for(; i <= size; ++i)
    if(i < date_ids[i - 1])
      break;

  return i;
}

} // namespace jccu