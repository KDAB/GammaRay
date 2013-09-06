#include "webviewmodel.h"

#include "include/objectmodel.h"

#include <QWebPage>

using namespace GammaRay;

WebViewModel::WebViewModel(QObject* parent): ObjectFilterProxyModelBase(parent)
{
}

WebViewModel::~WebViewModel()
{
}

QVariant WebViewModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != WebKitVersionRole || index.column() != 0)
    return QSortFilterProxyModel::data(index, role);

  const QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
  const bool isWk1 = qobject_cast<const QWebPage*>(obj);

  if (role == Qt::DisplayRole)
    return QString(Util::displayString(obj) + (isWk1 ? " [WebKit1]" : " [WebKit2]"));
  if (role == WebKitVersionRole)
    return isWk1 ? 1 : 2;

  Q_ASSERT(!"WTF?");
  return QVariant();
}

QMap< int, QVariant > WebViewModel::itemData(const QModelIndex& index) const
{
  QMap<int, QVariant> d = ObjectFilterProxyModelBase::itemData(index);
  d.insert(WebKitVersionRole, data(index, WebKitVersionRole));
  return d;
}

bool WebViewModel::filterAcceptsObject(QObject* object) const
{
  return qobject_cast<QWebPage*>(object) || object->inherits("QQuickWebView");
}


#include "webviewmodel.moc"
