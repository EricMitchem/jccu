///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include "good_manager.h"

#include <algorithm>

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Constructor.
///////////////////////////////////////////////////////////////////////////////
GoodManager::GoodManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Destructor.
///////////////////////////////////////////////////////////////////////////////
GoodManager::~GoodManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Adds the good if it doesn't exist.
/// Outputs the chosen good id if id is non-null.
/// Returns true on success.
///////////////////////////////////////////////////////////////////////////////
bool GoodManager::add(const QString& good, int* id)
{
  if(exists(good))
    return false;

  int good_id = nextId();

  if(id)
    *id = good_id;

  return insert(good_id, good);
}


///////////////////////////////////////////////////////////////////////////////
/// Inserts good good with id good_id, if neither exists and good_id > 0.
///////////////////////////////////////////////////////////////////////////////
bool GoodManager::insert(int good_id, const QString& good)
{
  if(good_id < 1)
    return false;

  if(good.isEmpty())
    return false;

  if(exists(good_id) || exists(good))
    return false;

  int new_row = goods_list_.size();
  
  beginInsertRows(QModelIndex(), new_row, new_row);
    fwd_goods_container_.insert(good_id, good);
    rev_goods_container_.insert(good, good_id);
    goods_list_.append(good);
  endInsertRows();
  
  sort();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Removes the good.
///////////////////////////////////////////////////////////////////////////////
bool GoodManager::remove(const QString& good)
{
  if(!exists(good))
    return false;

  int index = goods_list_.indexOf(good);

  beginRemoveRows(QModelIndex(), index, index);
    fwd_goods_container_.remove(id(good));
    rev_goods_container_.remove(good);
    goods_list_.removeOne(good);
  endRemoveRows();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Clears all goods.
///////////////////////////////////////////////////////////////////////////////
void GoodManager::clear()
{
  const int size = goods_list_.size();

  if(!size)
    return;

  beginRemoveRows(QModelIndex(), 0, size - 1);
    fwd_goods_container_.clear();
    rev_goods_container_.clear();
    goods_list_.clear();
  endRemoveRows();
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the good id or good at the given index.
///////////////////////////////////////////////////////////////////////////////
QVariant GoodManager::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();

  if(role != Qt::DisplayRole)
    return QVariant();

  if(index.column() >= columnCount())
    return QVariant();

  if(index.row() >= goods_list_.size())
    return QVariant();

  QString good = goods_list_.at(index.row());
  int good_id = id(good);

  if(index.column() == 0)
    return good_id;
  else
    return good;
}


///////////////////////////////////////////////////////////////////////////////
/// Header data.
///////////////////////////////////////////////////////////////////////////////
QVariant GoodManager::headerData(int section,
                                 Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole)
    return QVariant();

  if(orientation != Qt::Horizontal)
    return QVariant();

  if(section == 0)
    return QString("Id");
  else
    return QString("Good");
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of columns.
///////////////////////////////////////////////////////////////////////////////
int GoodManager::columnCount(const QModelIndex& parent) const
{
  // GoodId, Good
  return 2;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the number of goods.
///////////////////////////////////////////////////////////////////////////////
int GoodManager::rowCount(const QModelIndex& parent) const
{
  return goods_list_.size();
}


///////////////////////////////////////////////////////////////////////////////
/// Sorts alphabetically, ascending.
///////////////////////////////////////////////////////////////////////////////
void GoodManager::sort(int column, Qt::SortOrder order)
{
  // Case insensitive compare.
  auto Compare = [](const QString& a, const QString& b) -> bool {
    QString ins_a = a.toLower(),
            ins_b = b.toLower();
    return ins_a < ins_b;
  };

  std::sort(goods_list_.begin(), goods_list_.end(), Compare);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns true if a good with good_id exists.
///////////////////////////////////////////////////////////////////////////////
bool GoodManager::exists(int good_id) const
{
  return fwd_goods_container_.contains(good_id);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns true if the good exists.
///////////////////////////////////////////////////////////////////////////////
bool GoodManager::exists(const QString& good) const
{
  return rev_goods_container_.contains(good);
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the good with the given id.
///////////////////////////////////////////////////////////////////////////////
QString GoodManager::good(int good_id) const
{
  QString good;

  if(exists(good_id))
    good = fwd_goods_container_.value(good_id);

  return good;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the id of the good or zero if the good doesn't exist.
///////////////////////////////////////////////////////////////////////////////
int GoodManager::id(const QString& good) const
{
  int good_id = 0;

  if(exists(good))
    good_id = rev_goods_container_.value(good);

  return good_id;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns the smallest available good id.
///////////////////////////////////////////////////////////////////////////////
int GoodManager::nextId() const
{
  auto good_ids = fwd_goods_container_.keys();
  std::sort(good_ids.begin(), good_ids.end());

  int i = 1;
  const int size = good_ids.size();

  for(; i <= size; ++i)
    if(i < good_ids[i - 1])
      break;

  return i;
}

} // namespace jccu