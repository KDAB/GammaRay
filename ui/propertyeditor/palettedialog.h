/*
  palettedialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_PALETTEDIALOG_H
#define GAMMARAY_PALETTEDIALOG_H

#include <QDialog>

namespace GammaRay {

class PropertyEditorFactory;

class PaletteModel;

namespace Ui {
  class PaletteDialog;
}

class PaletteDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit PaletteDialog(const QPalette &palette, QWidget *parent = 0);
    virtual ~PaletteDialog();

    QPalette editedPalette() const;

  private:
    Ui::PaletteDialog *ui;
    PaletteModel *m_model;
    QScopedPointer<PropertyEditorFactory> m_propertyEditorFactory;
};

}

#endif // GAMMARAY_PALETTEDIALOG_H
