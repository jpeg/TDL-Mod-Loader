/******************************************************************************
 * customization.cpp
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

#include "customization.h"
#include "ui_customization.h"

Customization::Customization(QWidget* parent, ModManager::Mod* modPtr) :
    QDialog(parent),
    ui(new Ui::Customization)
{
    m_mod = modPtr;

    ui->setupUi(this);

    ui->labelModName->setText(m_mod->prettyName);

    // Setup optionals UI
    m_modesViewport = new QWidget;
    m_optionsViewport = new QWidget;
    m_modesButtonGroup = new QButtonGroup;
    m_layoutOptionals = new QHBoxLayout;
    m_layoutModes = new QVBoxLayout;
    m_layoutOptions = new QVBoxLayout;
    m_modesViewport->setLayout(m_layoutModes);
    m_optionsViewport->setLayout(m_layoutOptions);
    ui->scrollAreaModes->setWidget(m_modesViewport);
    ui->scrollAreaOptions->setWidget(m_optionsViewport);
    m_layoutOptionals->addWidget(m_modesViewport);
    m_layoutOptionals->addWidget(m_optionsViewport);

    // Add modes to UI
    if(m_mod->modes.size() > 0)
    {
        for(int i=0; i<m_mod->modes.size(); i++)
        {
            m_modeButtons.push_back(new QRadioButton);
            m_modeButtons[i]->setText(m_mod->modes[i]->prettyName);
            m_modeButtons[i]->setChecked(m_mod->enabledMode == (unsigned int)i);
            m_layoutModes->addWidget(m_modeButtons[i]);
            m_modesButtonGroup->addButton(m_modeButtons[i], i);
        }
    }
    else
        ui->scrollAreaModes->hide();
    m_layoutModes->addStretch();

    // Add options to UI
    if(m_mod->options.size() > 0)
    {
        for(int i=0; i<m_mod->options.size(); i++)
        {
            m_optionButtons.push_back(new QCheckBox);
            m_optionButtons[i]->setText(m_mod->options[i]->prettyName);
            m_optionButtons[i]->setChecked(m_mod->options[i]->optionEnabled);
            m_layoutOptions->addWidget(m_optionButtons[i]);
        }
    }
    else
        ui->scrollAreaOptions->hide();
    m_layoutOptions->addStretch();

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Customization::~Customization()
{
    for(int i=0; i<m_modeButtons.size(); i++)
    {
        delete m_modeButtons[i];
    }
    for(int i=0; i<m_optionButtons.size(); i++)
    {
        delete m_optionButtons[i];
    }
    delete m_layoutOptions;
    delete m_layoutModes;
    delete m_layoutOptionals;
    delete m_modesButtonGroup;
    delete m_optionsViewport;
    delete m_modesViewport;
    delete ui;
}

void Customization::on_buttonBox_clicked(QAbstractButton *button)
{
    switch(ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Ok:
        m_mod->enabledMode = m_modesButtonGroup->checkedId();
        for(int i=0; i<m_optionButtons.size(); i++)
            m_mod->options[i]->optionEnabled = m_optionButtons[i]->isChecked();
        this->accept();
        break;
    case QDialogButtonBox::Cancel:
    default:
        this->reject();
        break;
    }
}
