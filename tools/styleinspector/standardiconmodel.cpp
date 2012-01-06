#include "standardiconmodel.h"
#include <util.h>

using namespace GammaRay;

StandardIconModel::StandardIconModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void StandardIconModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant StandardIconModel::dataForStandardIcon(QStyle::StandardPixmap stdPix, const QString& name, int column, int role) const
{
  if (column == 0) {
    if (role == Qt::DisplayRole)
      return name;
  } else if (column == 1) {
    if (role == Qt::DecorationRole)
      return m_style->standardIcon(stdPix);
    else if (role == Qt::DisplayRole)
      return Util::variantToString(m_style->standardIcon(stdPix));
  }
  return QVariant();
}


#define MAKE_SP( stdPix ) \
if (index.row() == QStyle:: stdPix) return dataForStandardIcon(QStyle:: stdPix, QLatin1String( #stdPix ), index.column(), role)

QVariant StandardIconModel::data(const QModelIndex& index, int role) const
{
  if (!m_style || !index.isValid())
    return QVariant();

  MAKE_SP(SP_TitleBarMenuButton);
  MAKE_SP(SP_TitleBarMinButton);
  MAKE_SP(SP_TitleBarMaxButton);
  MAKE_SP(SP_TitleBarCloseButton);
  MAKE_SP(SP_TitleBarNormalButton);
  MAKE_SP(SP_TitleBarShadeButton);
  MAKE_SP(SP_TitleBarUnshadeButton);
  MAKE_SP(SP_TitleBarContextHelpButton);
  MAKE_SP(SP_DockWidgetCloseButton);
  MAKE_SP(SP_MessageBoxInformation);
  MAKE_SP(SP_MessageBoxWarning);
  MAKE_SP(SP_MessageBoxCritical);
  MAKE_SP(SP_MessageBoxQuestion);
  MAKE_SP(SP_DesktopIcon);
  MAKE_SP(SP_TrashIcon);
  MAKE_SP(SP_ComputerIcon);
  MAKE_SP(SP_DriveFDIcon);
  MAKE_SP(SP_DriveHDIcon);
  MAKE_SP(SP_DriveCDIcon);
  MAKE_SP(SP_DriveDVDIcon);
  MAKE_SP(SP_DriveNetIcon);
  MAKE_SP(SP_DirOpenIcon);
  MAKE_SP(SP_DirClosedIcon);
  MAKE_SP(SP_DirLinkIcon);
  MAKE_SP(SP_FileIcon);
  MAKE_SP(SP_FileLinkIcon);
  MAKE_SP(SP_ToolBarHorizontalExtensionButton);
  MAKE_SP(SP_ToolBarVerticalExtensionButton);
  MAKE_SP(SP_FileDialogStart);
  MAKE_SP(SP_FileDialogEnd);
  MAKE_SP(SP_FileDialogToParent);
  MAKE_SP(SP_FileDialogNewFolder);
  MAKE_SP(SP_FileDialogDetailedView);
  MAKE_SP(SP_FileDialogInfoView);
  MAKE_SP(SP_FileDialogContentsView);
  MAKE_SP(SP_FileDialogListView);
  MAKE_SP(SP_FileDialogBack);
  MAKE_SP(SP_DirIcon);
  MAKE_SP(SP_DialogOkButton);
  MAKE_SP(SP_DialogCancelButton);
  MAKE_SP(SP_DialogHelpButton);
  MAKE_SP(SP_DialogOpenButton);
  MAKE_SP(SP_DialogSaveButton);
  MAKE_SP(SP_DialogCloseButton);
  MAKE_SP(SP_DialogApplyButton);
  MAKE_SP(SP_DialogResetButton);
  MAKE_SP(SP_DialogDiscardButton);
  MAKE_SP(SP_DialogYesButton);
  MAKE_SP(SP_DialogNoButton);
  MAKE_SP(SP_ArrowUp);
  MAKE_SP(SP_ArrowDown);
  MAKE_SP(SP_ArrowLeft);
  MAKE_SP(SP_ArrowRight);
  MAKE_SP(SP_ArrowBack);
  MAKE_SP(SP_ArrowForward);
  MAKE_SP(SP_DirHomeIcon);
  MAKE_SP(SP_CommandLink);
  MAKE_SP(SP_VistaShield);
  MAKE_SP(SP_BrowserReload);
  MAKE_SP(SP_BrowserStop);
  MAKE_SP(SP_MediaPlay);
  MAKE_SP(SP_MediaStop);
  MAKE_SP(SP_MediaPause);
  MAKE_SP(SP_MediaSkipForward);
  MAKE_SP(SP_MediaSkipBackward);
  MAKE_SP(SP_MediaSeekForward);
  MAKE_SP(SP_MediaSeekBackward);
  MAKE_SP(SP_MediaVolume);
  MAKE_SP(SP_MediaVolumeMuted);

  return QVariant();
}

int StandardIconModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int StandardIconModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid() || !m_style)
    return 0;
  return QStyle::SP_MediaVolumeMuted + 1;
}

QVariant StandardIconModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Name");
      case 1: return tr("Icon");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "standardiconmodel.moc"
