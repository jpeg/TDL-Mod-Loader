/******************************************************************************
 * settings.cpp
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

#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent, QSettings* settings) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    m_accept = false;

    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->width(), this->height());

    m_settings = settings;
    ui->lineEditGameInstallLocation->setText(m_settings->value("game/path").toString());
    ui->checkBoxStatusBar->setChecked(m_settings->value("settings/debug").toBool());
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_buttonBox_clicked(QAbstractButton *button)
{
    bool close = false;
    switch(ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Ok:
        close = true;
    case QDialogButtonBox::Apply:
        m_accept = true;
        m_settings->setValue("game/path", ui->lineEditGameInstallLocation->text());
        m_settings->setValue("settings/debug", ui->checkBoxStatusBar->isChecked());
        break;
    case QDialogButtonBox::Cancel:
        close = true;
        break;
    default:
        break;
    }
    if(close && m_accept)
        this->accept();
    else if(close)
        this->reject();
}
