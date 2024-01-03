/*
  palettedialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PALETTEDIALOG_H
#define GAMMARAY_PALETTEDIALOG_H

#include <QDialog>

#include <ui/uistatemanager.h>

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
    explicit PaletteDialog(const QPalette &palette, QWidget *parent = nullptr);
    ~PaletteDialog() override;

    void setEditable(bool editable);
    QPalette editedPalette() const;

private:
    Ui::PaletteDialog *ui;
    UIStateManager m_stateManager;
    PaletteModel *m_model;
};
}

#endif // GAMMARAY_PALETTEDIALOG_H
