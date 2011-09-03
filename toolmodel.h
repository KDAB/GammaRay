#ifndef ENDOSCOPE_TOOLMODEL_H
#define ENDOSCOPE_TOOLMODEL_H

#include <qabstractitemmodel.h>
#include <qvector.h>

namespace Endoscope {

class ToolFactory;

/** Manages the list of available probing tools.
 */
class ToolModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    enum Role {
      ToolFactoryRole = Qt::UserRole + 1,
      ToolWidgetRole
    };
    explicit ToolModel(QObject* parent = 0);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  private:
    QVector<ToolFactory*> m_tools;
    QHash<ToolFactory*, QWidget*> m_toolWidgets;
};

}

#endif // ENDOSCOPE_TOOLMODEL_H
