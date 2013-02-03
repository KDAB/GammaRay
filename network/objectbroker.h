#ifndef GAMMARAY_OBJECTBROKER_H
#define GAMMARAY_OBJECTBROKER_H

class QItemSelectionModel;
class QAbstractItemModel;
class QString;

namespace GammaRay {

/** Retrieve/expose objects independent of whether using in-process or out-of-process UI. */
namespace ObjectBroker {

  /** Register a newly created model with the given name. */
  void registerModel(const QString &name, QAbstractItemModel* model);

  /** Retrieve a model by name. */
  QAbstractItemModel* model(const QString &name);

  typedef QAbstractItemModel*(*ModelNotFoundCallback)(const QString &);

  /** Set a callback for the case that a model was requested but had not been registered before. */
  void setModelNotFoundCallback(ModelNotFoundCallback callback);

  /** Register a newly created selection model. */
  void registerSelectionModel(QItemSelectionModel *selectionModel);

  /** Retrieve the selection model for @p model. */
  QItemSelectionModel* selectionModel(QAbstractItemModel *model);

  typedef QItemSelectionModel*(*selectionModelNotFoundCallback)(QAbstractItemModel*);

  /** Set a callback for the case that a selection model was requested but had not been registered before. */
  void setSelectionModelNotFoundCallback(selectionModelNotFoundCallback callback);
}
}

#endif // GAMMARAY_OBJECTBROKER_H
