#ifndef ENDOSCOPE_OBJECTCLASSINFOMODEL_H
#define ENDOSCOPE_OBJECTCLASSINFOMODEL_H

#include "metaobjectmodel.h"
#include <QMetaClassInfo>

namespace Endoscope {

class ObjectClassInfoModel : public MetaObjectModel<QMetaClassInfo, &QMetaObject::classInfo, &QMetaObject::classInfoCount, &QMetaObject::classInfoOffset>
{
  public:
    ObjectClassInfoModel(QObject* parent = 0);
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data( const QModelIndex &index, const QMetaClassInfo &classInfo, int role ) const;
    QString columnHeader(int index) const;
};

}

#endif // ENDOSCOPE_OBJECTCLASSINFOMODEL_H
