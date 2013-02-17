/******************************************************************************
 * customization.h
 *
 * Copyright (C) 2012-2013 Jason Gassel. All rights reserved.
 *
 * This file is part of the Jackal Mod Manager.
 *
 * Jackal Mod Manager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jackal Mod Manager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jackal Mod Manager.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef CUSTOMIZATION_H
#define CUSTOMIZATION_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>

#include "modmanager.h"

namespace Ui {
class Customization;
}

class Customization : public QDialog
{
    Q_OBJECT
    
public:
    explicit Customization(QWidget* parent, ModManager::Mod* modPtr);
    ~Customization();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    
private:
    Ui::Customization *ui;

    ModManager::Mod* m_mod;
    QWidget* m_modesViewport;
    QWidget* m_optionsViewport;
    QButtonGroup* m_modesButtonGroup;
    QHBoxLayout* m_layoutOptionals;
    QVBoxLayout* m_layoutModes;
    QVBoxLayout* m_layoutOptions;
    QVector<QRadioButton*> m_modeButtons;
    QVector<QCheckBox*> m_optionButtons;
};

#endif // CUSTOMIZATION_H
