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

  if (role == Qt::DisplayRole && index.column() == 0) {
    const QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    const bool isWk1 = qobject_cast<const QWebPage*>(obj);
    return QString(Util::displayString(obj) + (isWk1 ? " [WebKit1]" : " [WebKit2]"));
  }
  return QSortFilterProxyModel::data(index, role);
}

bool WebViewModel::filterAcceptsObject(QObject* object) const
{
  return qobject_cast<QWebPage*>(object) || object->inherits("QQuickWebView");
}


#include "webviewmodel.moc"
