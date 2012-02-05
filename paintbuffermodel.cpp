#include "config-gammaray.h"
#ifdef HAVE_PRIVATE_QT_HEADERS
#include "paintbuffermodel.h"

using namespace GammaRay;

struct cmd_t {
  QPaintBufferPrivate::Command cmd;
  const char* name;
};

#define CMD(cmd) { QPaintBufferPrivate::Cmd_ ## cmd, #cmd }

static cmd_t cmdTypes[] =  {
  CMD(Save),
  CMD(Restore),
  CMD(SetBrush),
  CMD(SetBrushOrigin),
  CMD(SetClipEnabled),
  CMD(SetCompositionMode),
  CMD(SetOpacity),
  CMD(SetPen),
  CMD(SetRenderHints),
  CMD(SetTransform),
  CMD(SetBackgroundMode),
  CMD(ClipPath),
  CMD(ClipRect),
  CMD(ClipRegion),
  CMD(ClipVectorPath),
  CMD(DrawVectorPath),
  CMD(FillVectorPath),
  CMD(StrokeVectorPath),
  CMD(DrawConvexPolygonF),
  CMD(DrawConvexPolygonI),
  CMD(DrawEllipseF),
  CMD(DrawEllipseI),
  CMD(DrawLineF),
  CMD(DrawLineI),
  CMD(DrawPath),
  CMD(DrawPointsF),
  CMD(DrawPointsI),
  CMD(DrawPolygonF),
  CMD(DrawPolygonI),
  CMD(DrawPolylineF),
  CMD(DrawPolylineI),
  CMD(DrawRectF),
  CMD(DrawRectI),
  CMD(FillRectBrush),
  CMD(FillRectColor),
  CMD(DrawText),
  CMD(DrawTextItem),
  CMD(DrawImagePos),
  CMD(DrawImageRect),
  CMD(DrawPixmapPos),
  CMD(DrawPixmapRect),
  CMD(DrawTiledPixmap),
  CMD(SystemStateChanged),
  CMD(Translate),
  CMD(DrawStaticText)
};

class PaintBufferPrivacyViolater : public QPainterReplayer
{
  public:
    QPaintBufferPrivate* extract() const { return d; }
};

PaintBufferModel::PaintBufferModel(QObject* parent): QAbstractTableModel(parent), m_privateBuffer(0)
{
}

void PaintBufferModel::setPaintBuffer(const QPaintBuffer& buffer)
{
  beginResetModel();
  m_buffer = buffer;
  PaintBufferPrivacyViolater p;
  p.processCommands(buffer, 0, 0, -1); // end < begin -> no processing
  m_privateBuffer = p.extract();
  endResetModel();
}

QVariant PaintBufferModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || !m_privateBuffer)
    return QVariant();

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0:
      {
        QPaintBufferCommand cmd = m_privateBuffer->commands.at(index.row());
        return cmdTypes[cmd.id].name;
      }
      case 1:
        return m_buffer.commandDescription(index.row());
    }
  }

  return QVariant();
}

int PaintBufferModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int PaintBufferModel::rowCount(const QModelIndex& parent) const
{
  if (!m_privateBuffer || parent.isValid())
    return 0;
  return m_privateBuffer->commands.size();
}

QVariant PaintBufferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Command");
      case 1: return tr("Description");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "paintbuffermodel.moc"
#endif
