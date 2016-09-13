#ifndef JCCU_SOURCE_GOOD_MANAGER_H
#define JCCU_SOURCE_GOOD_MANAGER_H

///////////////////////////////////////////////////////////////////////////////
/// Includes
///////////////////////////////////////////////////////////////////////////////
#include <QAbstractTableModel>
#include <QHash>
#include <QList>
#include <QString>

namespace jccu
{

///////////////////////////////////////////////////////////////////////////////
/// Manages all goods.
///////////////////////////////////////////////////////////////////////////////
class GoodManager : public QAbstractTableModel
{
  public:
    GoodManager();
    ~GoodManager();

    bool add(const QString& good, int* id = nullptr);
    bool insert(int good_id, const QString& good);
    
    bool remove(const QString& good);
    void clear();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    
    void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder);

    bool exists(int good_id) const;
    bool exists(const QString& good) const;

    QString good(int good_id) const;
    int id(const QString& good) const;
    int nextId() const;

  private:
    GoodManager(const GoodManager&);
    GoodManager& operator=(const GoodManager&);

    QHash<int, QString> fwd_goods_container_;
    QHash<QString, int> rev_goods_container_;
    QList<QString> goods_list_;
};

} // namespace jccu

#endif // JCCU_SOURCE_GOOD_MANAGER_H