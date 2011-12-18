#ifndef GAMMARAY_METAOBJECTREPOSITORY_H
#define GAMMARAY_METAOBJECTREPOSITORY_H

#include <QHash>

class QString;

namespace GammaRay {

class MetaObject;

/** Repository of compile-time introspection information for stuff not covered by the Qt meta object system. */
class MetaObjectRepository
{
public:
  ~MetaObjectRepository();

  /** Singleton accessor. */
  static MetaObjectRepository* instance();

  /** Add object type information to the repository. */
  void addMetaObject( const QString &typeName, MetaObject* mo );

  /** Convenience method for creating new object tpye information.
   *  Will automatically add base class reference and insert object into the repository.
   */
  MetaObject* createMetaObject( const QString &typeName, const QString &baseClassName = QString() );
  MetaObject* createMetaObject( const char* typeName, const char *baseClassName = 0 );

  /** Returns the introspection information for the type with the given name. */
  MetaObject* metaObject( const QString &typeName ) const;

protected:
    MetaObjectRepository();

private:
  void initBuiltInTypes();
  void initQObjectTypes();
  void initGraphicsViewTypes();

private:
  QHash<QString, MetaObject*> m_metaObjects;
};

}

#endif // GAMMARAY_METAOBJECTREPOSITORY_H
