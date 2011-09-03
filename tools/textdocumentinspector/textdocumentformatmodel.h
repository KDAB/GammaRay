#ifndef ENDOSCOPE_TEXTDOCUMENTFORMATMODEL_H
#define ENDOSCOPE_TEXTDOCUMENTFORMATMODEL_H

#include <QAbstractTableModel>
#include <qtextformat.h>

namespace Endoscope {

class TextDocumentFormatModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit TextDocumentFormatModel( QObject * parent = 0 );

    void setFormat( const QTextFormat &format );

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  private:
    QTextFormat m_format;
};

}

#endif
