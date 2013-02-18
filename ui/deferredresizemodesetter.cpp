#include "deferredresizemodesetter.h"

using namespace GammaRay;

DeferredResizeModeSetter::DeferredResizeModeSetter(QHeaderView* headerView, int section, QHeaderView::ResizeMode resizeMode):
  QObject(headerView),
  m_view(headerView),
  m_section(section),
  m_resizeMode(resizeMode)
{
  connect(m_view, SIGNAL(sectionCountChanged(int,int)), SLOT(setSectionResizeMode()));
  setSectionResizeMode();
}

DeferredResizeModeSetter::~DeferredResizeModeSetter()
{
}

void DeferredResizeModeSetter::setSectionResizeMode()
{
  if (m_view->count() <= m_section)
    return; // section not loaded yet

  m_view->setResizeMode(m_section, m_resizeMode);
}

#include "deferredresizemodesetter.moc"
