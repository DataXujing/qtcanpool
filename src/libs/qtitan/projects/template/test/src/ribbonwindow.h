/***************************************************************************
 **
 **  Copyright (C) 2022 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#ifndef RIBBONWINDOW_H
#define RIBBONWINDOW_H

#include <QtitanRibbon.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>


/* RibbonWindow */
class RibbonWindow : public Qtitan::RibbonMainWindow
{
public:
    RibbonWindow(QWidget* parent = Q_NULL);
    virtual ~RibbonWindow();

private:
    Q_DISABLE_COPY(RibbonWindow)
};

#endif // RIBBONWINDOW_H
