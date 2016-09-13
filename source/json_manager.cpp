///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include "json_manager.h"

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include "can_manager.h"

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Constructor.
///////////////////////////////////////////////////////////////////////////////
JsonManager::JsonManager(const QString& file_name,
                         GoodManager* good_manager,
                         DateManager* date_manager,
                         CanManager* can_manager)
  : file_name_(file_name),
    goods_(good_manager),
    dates_(date_manager),
    cans_(can_manager)
{
}


///////////////////////////////////////////////////////////////////////////////
/// Destructor.
///////////////////////////////////////////////////////////////////////////////
JsonManager::~JsonManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Reads in data from json.
///////////////////////////////////////////////////////////////////////////////
bool JsonManager::read()
{
  if(!valid())
    return false;

  // Write the template.
  if(!QFile::exists(file_name_))
    return write();

  QFile file(file_name_);

  if(!file.open(QFile::ReadOnly))
    return false;

  QByteArray json_data = file.readAll();
  auto json_document = QJsonDocument::fromJson(json_data);

  if(json_document.isNull())
    return false;

  if(json_document.isArray())
    return false;

  goods_->clear();
  dates_->clear();
  cans_->clear();

  auto json_object = json_document.object();

  auto goods_object = json_object.value("goods").toObject();
  auto goods_it = goods_object.constBegin(),
       goods_end = goods_object.constEnd();

  for(; goods_it != goods_end; ++goods_it) {
    int good_id = goods_it.key().toInt();
    QString good = goods_it.value().toString();

    goods_->insert(good_id, good);
  }

  auto dates_object = json_object.value("dates").toObject();
  auto dates_it = dates_object.constBegin(),
       dates_end = dates_object.constEnd();

  // Can't convert from QJsonValue -> long long.
  // Use QString as an intermediary.
  for(; dates_it != dates_end; ++dates_it) {
    int date_id = dates_it.key().toInt();
    int64_t date = dates_it.value().toString().toLongLong();

    dates_->insert(date_id, date);
  }

  auto cans_object = json_object.value("cans").toObject();
  auto cans_it = cans_object.constBegin(),
       cans_end = cans_object.constEnd();

  for(; cans_it != cans_end; ++cans_it) {
    auto can_array = cans_it.value().toArray();
    int can_id = cans_it.key().toInt();
    int good_id = can_array[0].toString().toInt();
    int date_id = can_array[1].toString().toInt();

    cans_->insert(can_id, good_id, date_id);
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Writes data out to json.
///////////////////////////////////////////////////////////////////////////////
bool JsonManager::write() const
{
  if(!valid())
    return false;

  QJsonObject goods_object;

  int rows = goods_->rowCount();

  for(int i = 0; i < rows; ++i) {
    QString good_id = goods_->data(goods_->index(i, 0)).toString();
    QString good = goods_->data(goods_->index(i, 1)).toString();
    goods_object[good_id] = good;
  }

  QJsonObject json_object;
  json_object["goods"] = goods_object;

  QJsonObject dates_object;

  rows = dates_->rowCount();

  for(int i = 0; i < rows; ++i) {
    QString date_id = dates_->data(dates_->index(i, 0)).toString();
    QString date = dates_->data(dates_->index(i, 1)).toString();
    dates_object[date_id] = date;
  }

  json_object["dates"] = dates_object;

  QJsonObject cans_object;

  rows = cans_->rowCount();

  for(int i = 0; i < rows; ++i) {
    QString can_id = cans_->data(cans_->index(i, 0)).toString();
    QString good_id = cans_->data(cans_->index(i, 1),
                                  CanManager::IdRole).toString();
    QString date_id = cans_->data(cans_->index(i, 2),
                                  CanManager::IdRole).toString();

    QJsonArray can_array;
    can_array.append(good_id);
    can_array.append(date_id);

    cans_object[can_id] = can_array;
  }

  json_object["cans"] = cans_object;

  QSaveFile file(file_name_);
  QJsonDocument json_document(json_object);
  
  if(!file.open(QSaveFile::WriteOnly))
    return false;

  file.write(json_document.toJson());

  if(!file.commit())
    return false;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Returns true if the manager is in a valid state.
///////////////////////////////////////////////////////////////////////////////
bool JsonManager::valid() const
{
  if(file_name_.isEmpty())
    return false;

  if(!goods_ || !dates_ || !cans_)
    return false;

  return true;
}

} // namespace jccu