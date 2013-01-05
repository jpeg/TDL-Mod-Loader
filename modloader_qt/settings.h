/******************************************************************************
 * settings.h
 *
 * Copyright (C) 2012-2013 Jason Gassel. All rights reserved.
 *
 * This file is part of the Jackal Mod Loader.
 *
 * Jackal Mod Loader is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jackal Mod Loader is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jackal Mod Loader.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent, QSettings* settings);
    ~Settings();
    
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::Settings *ui;
    QSettings* m_settings;
    bool m_accept;
};

#endif // SETTINGS_H
