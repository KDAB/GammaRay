#ifndef GAMMARAY_CLIENTTOOLMODEL_H
#define GAMMARAY_CLIENTTOOLMODEL_H

#include "remotemodel.h"

#include <QHash>
#include <QPointer>

class QWidget;

namespace GammaRay {

class ToolFactory;

/** Tool model for the client that implements the custom roles that return widget/factory pointers. */
class ClientToolModel : public RemoteModel
{
  Q_OBJECT
public:
  ClientToolModel(QObject* parent = 0);
  ~ClientToolModel();

  virtual QVariant data(const QModelIndex& index, int role) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:
  void insertFactory(ToolFactory* factory);

private:
  QHash<QString, ToolFactory*> m_factories; // ToolId -> ToolFactory
  mutable QHash<QString, QWidget*> m_widgets; // ToolId -> Widget
  QPointer<QWidget> m_parentWidget;
};

}

#endif // GAMMARAY_CLIENTTOOLMODEL_H
