/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONTABBAR_H
#define QTN_RIBBONTABBAR_H

#include <QWidget>
#include <QMenu>

#include "QtitanDef.h"
#include "QtnRibbonPage.h"

QTITAN_BEGIN_NAMESPACE

class RibbonTabPrivate;
class RibbonTab;

class ContextHeader
{
public:
    ContextHeader(RibbonTab* tab);
    virtual ~ContextHeader();

public:
    RibbonTab* firstTab;
    RibbonTab* lastTab;
    RibbonPage::ContextColor color;
    QString strTitle;
    QString strGroupName;
    QRect rcRect;
};

/* RibbonTab */
class RibbonTab : public QWidget
{
    Q_OBJECT
protected:
    RibbonTab(const QString& text, QWidget* parent);
    virtual ~RibbonTab();

public:
    void setPage(RibbonPage* page);

    void setSelect(bool);
    bool select() const;

    int indent() const;
    void setIndent(int);

    int margin() const;
    void setMargin(int);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment);

    void setTextTab(const QString&);
    const QString& textTab() const;

    void setContextTextTab(const QString&);
    const QString& contextTextTab() const;

    void setContextGroupName(const QString&);
    const QString& contextGroupName() const;

    void setContextTab(RibbonPage::ContextColor color);
    RibbonPage::ContextColor contextColor() const;

    void setContextHeader(ContextHeader* contextHeaders);
    ContextHeader* getContextHeader() const;

    void setTrackingMode(bool tracking);
    bool isTrackingMode() const;

    void setTabWidth(int width);
    int tabWidth() const;

    QAction* defaultAction() const;

protected:
    int shortcut() const;
    bool validRect() const;

public:
    virtual QSize sizeHint() const;

signals:
    void pressTab(QWidget* widget);

protected:
    virtual void paintEvent(QPaintEvent* event);

    virtual QSize minimumSizeHint() const;

    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);
    virtual void mousePressEvent(QMouseEvent* );
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);

protected:
    int lastTab;

private:
    friend class RibbonStyle;
    friend class RibbonPage;
    friend class RibbonTabBar;
    friend class RibbonQuickAccessBar;
    friend class RibbonSystemButton;
    friend class RibbonTabBarPrivate;
    QTN_DECLARE_PRIVATE(RibbonTab)
    Q_DISABLE_COPY(RibbonTab)
};


class RibbonPage;
class RibbonSystemPopupBar;
class RibbonTabBarPrivate;
/* RibbonTabBar */
class RibbonTabBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(SelectionBehavior selectionBehaviorOnRemove READ selectionBehaviorOnRemove WRITE setSelectionBehaviorOnRemove)
protected:
    RibbonTabBar(QWidget* parent);
    virtual ~RibbonTabBar();

    enum SelectionBehavior 
    {
        SelectLeftTab,
        SelectRightTab,
        SelectPreviousTab
    };

protected:
    int addTab(const QString& text);
    int addTab(const QIcon& icon, const QString& text);

    int insertTab(int index, const QString& text);
    int insertTab(int index, const QIcon& icon, const QString& text);

    void moveTab(int index, int newIndex);
    void removeTab(int index);

    int currentIndex() const;

    RibbonTab* getTab(int nIndex) const;
    int getTabCount() const;
    int getIndex(const RibbonTab* tab) const;
        
    SelectionBehavior selectionBehaviorOnRemove() const;
    void setSelectionBehaviorOnRemove(SelectionBehavior behavior);

    void setEnabledWidgets(bool enabled);

protected:
    QAction* addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL);
    QAction* addAction(QAction* action, Qt::ToolButtonStyle style);
    QMenu* addMenu(const QString& title);

public Q_SLOTS:
    void setCurrentIndex(int index);
    void currentNextTab(bool next);

private Q_SLOTS:
    void activateTab(QWidget* widget);

Q_SIGNALS:
    void currentChanged(int index);
    void activating(bool& allow);

protected:
    void layoutWidgets();
    void refresh();
    bool validWidth() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
    friend class RibbonTab;
    friend class RibbonBar;
    friend class RibbonBarPrivate;
    friend class RibbonPaintManager2013;
    QTN_DECLARE_PRIVATE(RibbonTabBar)
    Q_DISABLE_COPY(RibbonTabBar)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONTABBAR_H
