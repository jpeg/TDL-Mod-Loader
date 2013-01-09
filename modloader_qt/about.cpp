/******************************************************************************
 * about.cpp
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

#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    ui->labelProgramName->setText(QCoreApplication::applicationName());

    QString version;
    QTextStream versionStream(&version);
    versionStream << "Version: " << QCoreApplication::applicationVersion();
    ui->labelVersion->setText(version);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->width(), this->height());
}

About::~About()
{
    delete ui;
}
