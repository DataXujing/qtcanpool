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
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QUndoStack>
#include <qdebug.h>
#include <QMenu>
#include <QMimeData>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerDnDItemInterface>

#include "QtnRibbonStyleDsgnPlugin.h"
#include "QtnRibbonDsgnTaskMenu.h"
#include "QtnCommandDsgn.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonMainWindow.h"

QTITAN_USE_NAMESPACE

static bool g_removeBlock = false;
static QWidget* g_dragStyle = Q_NULL;

static void getAllChildren(QWidget* widget, QWidgetList& list)
{
    list.append(widget);

    QList<QWidget*> widgets = widget->findChildren<QWidget*>();
    foreach (QWidget* w, widgets)
        ::getAllChildren(w, list);
}

/* DesignerStyleInput */
DesignerStyleInput::DesignerStyleInput(QWidget* widget) 
    : /*QInputContext*/QObject(widget), m_widget(widget) 
{
    widget->installEventFilter(this);
    m_actRemoveStyle = new QAction(tr("Remove Style"), this);
    connect(m_actRemoveStyle, SIGNAL(triggered()), this, SLOT(removeStyle()));
}

DesignerStyleInput::~DesignerStyleInput() 
{
    parent()->removeEventFilter(this);
}

bool DesignerStyleInput::filterEvent(const QEvent* event)
{
    if (event->type()== QEvent::MouseButtonPress)
    {
        QMouseEvent* me = (QMouseEvent *)event;
        if (me->button() == Qt::LeftButton)
        {
            QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_widget);
            formWindow->clearSelection(false);
            formWindow->core()->propertyEditor()->setObject(m_widget);
            formWindow->core()->propertyEditor()->setEnabled(true);
            return true;
        }
        else if (me->button() == Qt::RightButton)
        {
            QList<QAction *> actions;
            actions.append(m_actRemoveStyle);
            QMenu::exec(actions, QCursor::pos());
            return true;
        }
    }
    return true; // Handle all input context events here.
}

void DesignerStyleInput::removeStyle()
{
    if (qobject_cast<QWidget*>(parent()))
    {
        if (g_removeBlock)
            return;

        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_widget);
        Q_ASSERT(formWindow != Q_NULL);

        const QString description = tr("Delete '%1'").arg(m_widget->objectName());
        formWindow->commandHistory()->beginMacro(description);

        DeleteStyleCommand* cmd = new DeleteStyleCommand(formWindow);
        cmd->init(m_widget);
        formWindow->commandHistory()->push(cmd);
        formWindow->commandHistory()->endMacro();
    }
}

/* DsgnRibbonStyle */
DsgnRibbonStyle::DsgnRibbonStyle(QWidget* parent) 
    : RibbonStyle()  
{ 
    m_parent = parent; 
}

QWidgetList DsgnRibbonStyle::getAllWidgets() const
{
    QWidgetList list;
    if (m_parent)
        ::getAllChildren(m_parent, list);

    return list;
}

QWidget* DsgnRibbonStyle::parentStyle() const 
{ 
    return m_parent; 
}


/* DsgnRibbonStyleWidget */
DsgnRibbonStyleWidget::DsgnRibbonStyleWidget(QWidget* parent) 
    : QWidget(parent) 
{ 
    m_targetStyle = new DsgnRibbonStyle(parent);
    setAttribute(Qt::WA_InputMethodEnabled);
    /*DesignerStyleInput* input =*/ new DesignerStyleInput(this);
//    setInputContext(input);
}

DsgnRibbonStyleWidget::~DsgnRibbonStyleWidget()
{
    Q_DELETE_AND_NULL(m_targetStyle);
}

bool DsgnRibbonStyleWidget::event(QEvent* event)
{
    bool res = QWidget::event(event);
    if (event->type() == QEvent::ParentChange)
    {
        if (DsgnRibbonStyle* desStyle = dynamic_cast<DsgnRibbonStyle*>(m_targetStyle))
            desStyle->setParentStyle(parentWidget());
    }
    else if (event->type() == QEvent::Hide && isHidden())
    {
        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(parentWidget());
        QDesignerFormEditorInterface* core = formWindow->core();
        QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

        if (container->widget(container->count() - 1) == this)
        {
            container->remove(container->count() - 1);
            core->metaDataBase()->remove(this);
            formWindow->emitSelectionChanged();
        }
    }
    return res;
}

void DsgnRibbonStyleWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(width() + width(), 0));
    linearGrad.setColorAt(0, QColor("#FFFFCC"));
    linearGrad.setColorAt(1, Qt::transparent);
    QBrush brush = linearGrad;
    painter.fillRect(QRect(0, 0, width(), height()), brush);
    QRect border = QRect(0, 0, width() - 1, height() - 1);
    painter.drawRect(border);
    painter.rotate(-90);
    painter.translate(-height(), 0);
    QFont f = painter.font();
    f.setBold(true);
    painter.setFont(f);
    QRect area = QRect(1, 1, height() - 1, width() - 1);
    painter.drawText(area, Qt::AlignCenter, "Ribbon Style");
}

QStyle* DsgnRibbonStyleWidget::targetStyle() const
{
    return m_targetStyle;
}

QSize DsgnRibbonStyleWidget::minimumSizeHint() const
{
    return QSize(20, 100);
}

/* DsgnOfficeStyle */
DsgnOfficeStyle::DsgnOfficeStyle(QWidget* parent) 
    : OfficeStyle() 
{ 
    m_parent = parent; 
}

QWidgetList DsgnOfficeStyle::getAllWidgets() const
{
    QWidgetList list;
    if (m_parent)
        ::getAllChildren(m_parent, list);

    return list;
}

QWidget* DsgnOfficeStyle::parentStyle() const
{
    return m_parent;
}

/* DsgnOfficeStyleWidget */
DsgnOfficeStyleWidget::DsgnOfficeStyleWidget(QWidget* parent)
    : QWidget(parent) 
{ 
    m_targetStyle = new DsgnOfficeStyle(parent);
    setAttribute(Qt::WA_InputMethodEnabled);
    /*DesignerStyleInput* input =*/ new DesignerStyleInput(this);
    /*setInputContext(input);*/
}

DsgnOfficeStyleWidget::~DsgnOfficeStyleWidget()
{
    Q_DELETE_AND_NULL(m_targetStyle);
}

QStyle* DsgnOfficeStyleWidget::targetStyle() const
{
    return m_targetStyle;
}

QSize DsgnOfficeStyleWidget::minimumSizeHint() const
{
    return QSize(20, 150);
}

bool DsgnOfficeStyleWidget::event(QEvent* event)
{
    bool res = QWidget::event(event);
    if (event->type() == QEvent::ParentChange)
    {
        if (DsgnOfficeStyle* desStyle = dynamic_cast<DsgnOfficeStyle *>(m_targetStyle))
            desStyle->setParentStyle(parentWidget());
    }
    else if (event->type() == QEvent::Hide && isHidden())
    {
        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(parentWidget());
        QDesignerFormEditorInterface* core = formWindow->core();
        QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

        if (container->widget(container->count() - 1) == this)
        {
            container->remove(container->count() - 1);
            core->metaDataBase()->remove(this);
            formWindow->emitSelectionChanged();
        }
    }
    return res;
}

void DsgnOfficeStyleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(width() + width(), 0));
    linearGrad.setColorAt(0, QColor("#FFFFCC"));
    linearGrad.setColorAt(1, Qt::transparent);
    QBrush brush = linearGrad;
    painter.fillRect(QRect(0, 0, width(), height()), brush);
    QRect border = QRect(0, 0, width() - 1, height() - 1);
    painter.drawRect(border);
    painter.rotate(-90);
    painter.translate(-height(), 0);
    QFont f = painter.font();
    f.setBold(true);
    painter.setFont(f);
    QRect area = QRect(1, 1, height() - 1, width() - 1);
    painter.drawText(area, Qt::AlignCenter, "Microsoft Office Style");
}

/* RibbonStyleFake */
RibbonStyleFake::RibbonStyleFake(QWidget* parent) 
    : DsgnRibbonStyleWidget(parent)
{
    m_metaObject = new QMetaObject;

    if (parent && !parent->parentWidget())
        g_dragStyle = this;
    setCursor(Qt::PointingHandCursor);
}

RibbonStyleFake::~RibbonStyleFake()
{
    g_dragStyle = Q_NULL;
    delete m_metaObject;
}

const QMetaObject* RibbonStyleFake::metaObject() const
{
    const QMetaObject* dataSuper = DsgnRibbonStyleWidget::metaObject();
    const QMetaObject* dataStyle = targetStyle()->metaObject();
    m_metaObject->d.superdata = dataSuper->superClass();
    m_metaObject->d.stringdata = dataStyle->d.stringdata;
    m_metaObject->d.data = dataStyle->d.data;
    m_metaObject->d.extradata = dataStyle->d.extradata;
    return m_metaObject;
}

void* RibbonStyleFake::qt_metacast(const char *_clname)
{
    return targetStyle()->qt_metacast(_clname);
}

/* OfficeStyleFake */
OfficeStyleFake::OfficeStyleFake(QWidget* parent) 
    : DsgnOfficeStyleWidget(parent)
{
    m_metaObject = new QMetaObject;

    if (parent && !parent->parentWidget())
        g_dragStyle = this;
    setCursor(Qt::PointingHandCursor);
}

OfficeStyleFake::~OfficeStyleFake()
{
    g_dragStyle = Q_NULL;
    delete m_metaObject;
}

const QMetaObject* OfficeStyleFake::metaObject() const
{
    const QMetaObject* dataSuper = DsgnOfficeStyleWidget::metaObject();
    const QMetaObject* dataStyle = targetStyle()->metaObject();
    m_metaObject->d.superdata = dataSuper->superClass();
    m_metaObject->d.stringdata = dataStyle->d.stringdata;
    m_metaObject->d.data = dataStyle->d.data;
    m_metaObject->d.extradata = dataStyle->d.extradata;
    return m_metaObject;
}

void* OfficeStyleFake::qt_metacast(const char *_clname)
{
    return targetStyle()->qt_metacast(_clname);
}

QWidget* Qtitan::create_fake_ribbon_style_widget(QWidget* parent)
{
    RibbonStyleFake* retval = new RibbonStyleFake(parent);
    retval->setMaximumSize(QSize(20, 100));
    retval->setMinimumSize(QSize(20, 100));
    return retval;
}

QWidget* Qtitan::create_fake_office_style_widget(QWidget* parent)
{
    OfficeStyleFake* retval = new OfficeStyleFake(parent);
    retval->setMaximumSize(QSize(20, 150));
    retval->setMinimumSize(QSize(20, 150));
    return retval;
}

///////////////////////////////////////////////////////////////////////////////
/* ExtensionManagerWrapper */
ExtensionManagerWrapper::ExtensionManagerWrapper(QExtensionManager *nativeManager)
    : QExtensionManager(nativeManager->parent()), m_nativeManager(nativeManager)
{
    nativeManager->setParent(this);
}

ExtensionManagerWrapper::~ExtensionManagerWrapper()
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    m_nativeManager = Q_NULL;
}

void ExtensionManagerWrapper::setStyleExtentions()
{
    QExtensionManager::registerExtensions(new DesignerMainWindowStyleContainerFactory(this), Q_TYPEID(QDesignerContainerExtension));
}

QExtensionManager* ExtensionManagerWrapper::nativeManager() const
{
    return m_nativeManager;
}

void ExtensionManagerWrapper::registerExtensions(QAbstractExtensionFactory *factory, const QString &iid)
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    if (m_nativeManager != Q_NULL)
        m_nativeManager->registerExtensions(factory, iid);
}

void ExtensionManagerWrapper::unregisterExtensions(QAbstractExtensionFactory *factory, const QString &iid)
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    if (m_nativeManager != Q_NULL)
        m_nativeManager->unregisterExtensions(factory, iid);
}

QObject* ExtensionManagerWrapper::extension(QObject *object, const QString &iid) const
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    if (m_nativeManager != Q_NULL)
    {
        if (object != Q_NULL && object->metaObject()->className() == create_qtitan_className("RibbonStyle"))
            object = static_cast<DsgnRibbonStyleWidget *>(object)->targetStyle();
        if (object != Q_NULL && object->metaObject()->className() == create_qtitan_className("OfficeStyle"))
            object = static_cast<DsgnOfficeStyleWidget *>(object)->targetStyle();

        QObject* extension = m_nativeManager->extension(object, iid);
        if (iid == Q_TYPEID(QDesignerContainerExtension) && extension != Q_NULL && qobject_cast<QMainWindow *>(object) != 0)
            extension = QExtensionManager::extension(object, iid);

        return extension;
    }
    else
        return Q_NULL;
}

///////////////////////////////////////////////////////////////////////////////
/* DesignerMainWindowStyleContainer */
DesignerMainWindowStyleContainer::DesignerMainWindowStyleContainer(QDesignerContainerExtension* nativeContainer, QMainWindow* mainWindow, QObject* parent)
    : QObject(parent), m_nativeContainer(nativeContainer), m_mainWindow(mainWindow), m_styleWidget(Q_NULL)
{
}

static void setChildStyle(QWidget* widget, QStyle* style)
{
    widget->setStyle(style);

    QList<QWidget*> widgets = widget->findChildren<QWidget*>();
    foreach (QWidget* w, widgets)
        ::setChildStyle(w, style);
}

void DesignerMainWindowStyleContainer::addWidget(QWidget *widget)
{
    if (widget->metaObject()->className() == create_qtitan_className("RibbonStyle"))
    {
        if (m_styleWidget != Q_NULL)
            remove(count() - 1);

        Q_ASSERT(m_styleWidget == Q_NULL);

        DsgnRibbonStyleWidget* styleWidget = static_cast<DsgnRibbonStyleWidget *>(widget);
        m_styleWidget = styleWidget;
        styleWidget->move(0, 200);
        styleWidget->setParent(m_mainWindow);
        styleWidget->setVisible(true);

        setChildStyle(m_mainWindow, styleWidget->targetStyle());
        return;
    }
    else if (widget->metaObject()->className() == create_qtitan_className("OfficeStyle"))
    {
        if (m_styleWidget != Q_NULL)
            remove(count() - 1);
        Q_ASSERT(m_styleWidget == Q_NULL);

        DsgnOfficeStyleWidget* styleWidget = static_cast<DsgnOfficeStyleWidget *>(widget);
        m_styleWidget = styleWidget;
        styleWidget->move(0, 200);
        styleWidget->setParent(m_mainWindow);
        styleWidget->show();
        setChildStyle(m_mainWindow, styleWidget->targetStyle());
        return;
    }

    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->addWidget(widget);

    if (dynamic_cast<DsgnRibbonStyleWidget*>(m_styleWidget))
        ::setChildStyle(widget, ((DsgnRibbonStyleWidget*)m_styleWidget)->targetStyle());
}

int DesignerMainWindowStyleContainer::count() const
{
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        return m_nativeContainer->count() + (m_styleWidget != Q_NULL ? 1 : 0);
    else
        return -1;
}

int DesignerMainWindowStyleContainer::currentIndex() const
{
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        return m_nativeContainer->currentIndex();
    else
        return -1;
}

void DesignerMainWindowStyleContainer::insertWidget(int index, QWidget *widget)
{
    if (widget->metaObject()->className() == create_qtitan_className("RibbonStyle") ||
        widget->metaObject()->className() == create_qtitan_className("OfficeStyle"))
    {
        addWidget(widget);
        return;
    }
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->insertWidget(index, widget);
}

void DesignerMainWindowStyleContainer::remove(int index)
{
    if (m_styleWidget != Q_NULL && index == count() - 1)
    {
        m_mainWindow->setStyle(Q_NULL);
        m_styleWidget = Q_NULL;
        setChildStyle(m_mainWindow, m_mainWindow->style());
        return;
    }
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->remove(index);
}

void DesignerMainWindowStyleContainer::setCurrentIndex(int index)
{
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->setCurrentIndex(index);
}

QWidget* DesignerMainWindowStyleContainer::widget(int index) const
{
    if (m_styleWidget != Q_NULL && index == count() - 1)
        return m_styleWidget;

    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        return m_nativeContainer->widget(index);
    else
        return Q_NULL;
}

QMainWindow* DesignerMainWindowStyleContainer::mainWindow() const
{
    return m_mainWindow;
}


///////////////////////////////////////////////////////////////////////////////
/* DesignerMainWindowStyleContainerFactory */
DesignerMainWindowStyleContainerFactory::DesignerMainWindowStyleContainerFactory(ExtensionManagerWrapper* parent)
    : QExtensionFactory(parent), m_nativeManager(parent->nativeManager())
{
}

QObject* DesignerMainWindowStyleContainerFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension))
        return Q_NULL;
    QObject* extension = m_nativeManager->extension(object, iid);
    return new DesignerMainWindowStyleContainer(
        qobject_cast<QDesignerContainerExtension *>(extension), qobject_cast<QMainWindow *>(object), parent);
}


///////////////////////////////////////////////////////////////////////////////
/* OfficeStyleDsgnPlugin */
OfficeStyleDsgnPlugin::OfficeStyleDsgnPlugin(QObject *parent)
  : QObject(parent), m_core(0)
{
    initialized = false;
}

bool OfficeStyleDsgnPlugin::isContainer() const
{
    return false;
}

bool OfficeStyleDsgnPlugin::isInitialized() const
{
    return initialized;
}

QIcon OfficeStyleDsgnPlugin::icon() const
{
    return QIcon(":res/style_icon.png");
}

QString OfficeStyleDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"" + create_qtitan_className("OfficeStyle") + "\" name=\"officeStyle\">\n"
        " </widget>\n"
        " <customwidgets>\n"
        "     <customwidget>\n"
        "         <class>" + create_qtitan_className("OfficeStyle") + "</class>\n"
        "         <extends>Q3DockWindow</extends>\n"
        "     </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString OfficeStyleDsgnPlugin::group() const
{
    return "Developer Machines: Styles";
}

QString OfficeStyleDsgnPlugin::includeFile() const
{
    return "QtnRibbonStyle.h";
}

QString OfficeStyleDsgnPlugin::name() const
{
    return create_qtitan_className("OfficeStyle");
}

QString OfficeStyleDsgnPlugin::toolTip() const
{
    return "";
}

QString OfficeStyleDsgnPlugin::whatsThis() const
{
    return "";
}

QWidget* OfficeStyleDsgnPlugin::createWidget(QWidget *parent)
{
    return create_fake_office_style_widget(parent);
}

void OfficeStyleDsgnPlugin::formWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    m_filter = new DsgnFormWindowManagerFilter(m_core, formWindow, this);
    qApp->installEventFilter(m_filter);

    connect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void OfficeStyleDsgnPlugin::formWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    disconnect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void OfficeStyleDsgnPlugin::widgetManaged(QWidget* widget)
{
    QDesignerFormWindowInterface* formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
    QDesignerFormEditorInterface* core = formWindow->core();
    QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

    if (widget->metaObject()->className() == create_qtitan_className("OfficeStyle"))
    {
        formWindow->unmanageWidget(widget);

        QUndoStack* stack = formWindow->commandHistory();
        if (!stack->isClean())
        {
            //This code check the InsertWidget command on the stack.
            const QUndoCommand* command = stack->command(stack->index());

            if (command->childCount() == 0)
                return;
        }

        if (qobject_cast<QMainWindow *>(formWindow->mainContainer()) == 0)
        {
            QMessageBox::critical(formWindow->mainContainer(), tr("Can't add Office Style"), 
                tr("You can't drag-drop the style to this QWidget form. The style can be placed only onto the form of QMainWindow successor."));
            widget->deleteLater();
            return;
        }

        QWidget* widgetStyle = Q_NULL;
        for (int i = 0; i < container->count() && widgetStyle == Q_NULL; ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == create_qtitan_className("OfficeStyle") ||
                w->metaObject()->className() == create_qtitan_className("RibbonStyle"))
                widgetStyle = w;
        }

        if (widgetStyle && widgetStyle->metaObject()->className() == create_qtitan_className("OfficeStyle"))
        {
            QMessageBox::critical(
                formWindow->mainContainer(),
                tr("Can't add Microsoft Office Style"), 
                tr("Only one instance of the Microsoft Office Style can be adding to the main form."));
            widget->deleteLater();
            return;
        }

/*
        for (int i = 0; i < container->count(); ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == create_qtitan_className("OfficeStyle"))
            {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Microsoft Office Style"), 
                    tr("Only one instance of the Microsoft Office Style can be adding to the main form."));
                widget->deleteLater();
                return;
            }
        }
*/
        if (widgetStyle)
        {
            g_removeBlock = true;
            const QString description = tr("Delete '%1'").arg(widgetStyle->objectName());
            formWindow->commandHistory()->beginMacro(description);

            DeleteStyleCommand* cmd = new DeleteStyleCommand(formWindow);
            cmd->init(widgetStyle);
            formWindow->commandHistory()->push(cmd);
            formWindow->commandHistory()->endMacro();
            g_removeBlock = false;
        }

        container->addWidget(widget);
        formWindow->core()->metaDataBase()->add(widget);
        widget->setCursor(Qt::PointingHandCursor);
    }
    else if (DesignerMainWindowStyleContainer* styelContainer = dynamic_cast<DesignerMainWindowStyleContainer*>(container))
    {
        if (QMainWindow* mainindow = static_cast<QMainWindow*>(styelContainer->mainWindow()))
        {
            if (CommonStyle* currentStyle = qobject_cast<CommonStyle*>(mainindow->style()))
                ::setChildStyle(widget, currentStyle);
        }
    }
}

void OfficeStyleDsgnPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;

    connect(core->formWindowManager(), SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), 
        this, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));
#if 0
    QExtensionManager* manager = core->extensionManager();
    Q_ASSERT(manager != Q_NULL);
    manager->registerExtensions(new DsgnRibbonTaskMenuFactory(manager), QLatin1String("QDesignerInternalTaskMenuExtension"));
#endif
}

/* RibbonStyleDsgnPlugin */
RibbonStyleDsgnPlugin::RibbonStyleDsgnPlugin(QObject *parent)
  : QObject(parent), m_core(0)
{
    initialized = false;
}

bool RibbonStyleDsgnPlugin::isContainer() const
{
    return false;
}

bool RibbonStyleDsgnPlugin::isInitialized() const
{
    return initialized;
}

QIcon RibbonStyleDsgnPlugin::icon() const
{
    return QIcon(":res/style_icon.png");
}

QString RibbonStyleDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"" + create_qtitan_className("RibbonStyle") + "\" name=\"ribbonStyle\">\n"
        " </widget>\n"
        " <customwidgets>\n"
        "     <customwidget>\n"
        "         <class>" + create_qtitan_className("RibbonStyle") + "</class>\n"
        "         <extends>Q3DockWindow</extends>\n"
        "     </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString RibbonStyleDsgnPlugin::group() const
{
    return "Developer Machines: Styles";
}

QString RibbonStyleDsgnPlugin::includeFile() const
{
    return "QtnRibbonStyle.h";
}

QString RibbonStyleDsgnPlugin::name() const
{
    return create_qtitan_className("RibbonStyle");
}

QString RibbonStyleDsgnPlugin::toolTip() const
{
    return "";
}

QString RibbonStyleDsgnPlugin::whatsThis() const
{
    return "";
}

QWidget* RibbonStyleDsgnPlugin::createWidget(QWidget* parent)
{
    return create_fake_ribbon_style_widget(parent);
}

void RibbonStyleDsgnPlugin::formWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    connect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void RibbonStyleDsgnPlugin::formWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    disconnect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void RibbonStyleDsgnPlugin::widgetManaged(QWidget* widget)
{
    QDesignerFormWindowInterface* formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
    QDesignerFormEditorInterface* core = formWindow->core();
    QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

    if (widget->metaObject()->className() == create_qtitan_className("RibbonStyle"))
    {
        formWindow->unmanageWidget(widget);

        QUndoStack* stack = formWindow->commandHistory();
        if (!stack->isClean())
        {
            //This code check the InsertWidget command on the stack.
            const QUndoCommand* command = stack->command(stack->index());
            if (command->childCount() == 0)
                return;
        }

        if (qobject_cast<QMainWindow *>(formWindow->mainContainer()) == 0)
        {
            QMessageBox::critical(formWindow->mainContainer(), tr("Can't add Ribbon Style"), 
                tr("You can't drag-drop the style to this QWidget form. The style can be placed only onto the form of QMainWindow successor."));
            widget->deleteLater();
            return;
        }

        QWidget* widgetStyle = Q_NULL;
        for (int i = 0; i < container->count() && widgetStyle == Q_NULL; ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == create_qtitan_className("OfficeStyle") ||
                w->metaObject()->className() == create_qtitan_className("RibbonStyle"))
                widgetStyle = w;
        }

        if (widgetStyle && widgetStyle->metaObject()->className() == create_qtitan_className("RibbonStyle"))
        {
            QMessageBox::critical(
                formWindow->mainContainer(),
                tr("Can't add Ribbon Style"), 
                tr("Only one instance of the Ribbon Style can be adding to the main form."));
            widget->deleteLater();
            return;
        }

        if (widgetStyle)
        {
            g_removeBlock = true;
            const QString description = tr("Delete '%1'").arg(widgetStyle->objectName());
            formWindow->commandHistory()->beginMacro(description);

            DeleteStyleCommand* cmd = new DeleteStyleCommand(formWindow);
            cmd->init(widgetStyle);
            formWindow->commandHistory()->push(cmd);
            formWindow->commandHistory()->endMacro();
            g_removeBlock = false;
        }

        container->addWidget(widget);
        formWindow->core()->metaDataBase()->add(widget);
        widget->setCursor(Qt::PointingHandCursor);
    }
    else if (DesignerMainWindowStyleContainer* styelContainer = dynamic_cast<DesignerMainWindowStyleContainer*>(container))
    {
        if (QMainWindow* mainindow = static_cast<QMainWindow*>(styelContainer->mainWindow()))
        {
            if (CommonStyle* currentStyle = qobject_cast<CommonStyle *>(mainindow->style()))
                ::setChildStyle(widget, currentStyle);
        }
    }
}

class HackDesignerFormEditorInterface: public QDesignerFormEditorInterface
{
    friend class Qtitan::RibbonStyleDsgnPlugin;
public:
    HackDesignerFormEditorInterface() {}
    void hackSetExtensionManager(QExtensionManager* extensionManager)
    {
        setExtensionManager(extensionManager);
    }
};

void RibbonStyleDsgnPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
    connect(core->formWindowManager(), SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), 
        this, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));

    QExtensionManager* manager = core->extensionManager();
    Q_ASSERT(manager != Q_NULL);
    ExtensionManagerWrapper* wrapper = new ExtensionManagerWrapper(manager);
    ((HackDesignerFormEditorInterface*)core)->hackSetExtensionManager(wrapper);
    wrapper->setStyleExtentions();
}

/* DsgnFormWindowManagerFilter */
DsgnFormWindowManagerFilter::DsgnFormWindowManagerFilter(QDesignerFormEditorInterface* core, QDesignerFormWindowInterface* formWindow, QObject* parent)
    : QObject(parent)
    , m_core(core)
    , m_formWindow(formWindow)
{
}

bool DsgnFormWindowManagerFilter::verifyExistenceStyle(QWidget* style)
{
    QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(m_core->extensionManager(), m_formWindow->mainContainer());
    if (QWidget* wd = container->widget(container->count()-1))
        return wd->metaObject()->className() == style->metaObject()->className();
    return false;
}

/* Please see original QDesignerMimeData declaration */
class  QDesignerMimeData_fake : public QMimeData {
    Q_OBJECT
public:
    typedef QList<QDesignerDnDItemInterface *> QDesignerDnDItemsEx;
    virtual ~QDesignerMimeData_fake ();
    const QDesignerDnDItemsEx &items() const { return m_items; }
    void acceptEvent(QDropEvent *e) const;
private:
    const QDesignerDnDItemsEx m_items;
};

bool DsgnFormWindowManagerFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (!obj->isWidgetType())
        return QObject::eventFilter(obj, event);

    RibbonBar* targetRibbonBar = qobject_cast<RibbonBar *>(obj);
    RibbonPage* targetRibbonPage = qobject_cast<RibbonPage *>(obj);
    RibbonGroup* targetRibbonGroup = qobject_cast<RibbonGroup *>(obj);
    const bool isTargetRibbonElement = targetRibbonBar || targetRibbonPage || targetRibbonGroup;

    switch (event->type())
    {
    case QEvent::ParentChange:
        {
            QWidget* widget = static_cast<QWidget *>(obj);
            QWidget* parentWidget = widget->parentWidget();
            while (parentWidget)
            {
                if (qobject_cast<RibbonBar *>(parentWidget) || qobject_cast<RibbonMainWindow *>(parentWidget))
                {
                    widget->setStyle(parentWidget->style());
                    break;
                }
                parentWidget = parentWidget->parentWidget();
            }
            return false;
        }
        break;
    case QEvent::MouseMove:
        {
            if (isTargetRibbonElement && static_cast<QMouseEvent*>(event)->buttons() == Qt::LeftButton)
                return true;
        }
        break;
    case QEvent::DragMove:
    case QEvent::DragEnter:
        {
            if (QDragEnterEvent* dEvent = static_cast<QDragEnterEvent*>(event))
            {
                const QMetaObject* mObject = dEvent->mimeData()->metaObject();
                const char * name = mObject ? mObject->className() : Q_NULL;
                if (name == QString("qdesigner_internal::ActionRepositoryMimeData"))
                    return false;

                const QDesignerMimeData_fake* mimedata = static_cast<const QDesignerMimeData_fake *>(dEvent->mimeData());
                if (mimedata)
                {
                    bool isSourceRibbonBar = false;
                    bool isSourceRibbonPage = false;
                    bool isSourceRibbonGroup = false;
                    //bool isSourceLayout = false;

                    for (QList<QDesignerDnDItemInterface *>::ConstIterator it = mimedata->items().constBegin(); it != mimedata->items().constEnd(); ++it)
                    {
                        QWidget* w = (*it)->widget();
                        if (w == 0)
                            w = (*it)->decoration();

                        QList<QWidget *> allWidgets = w->findChildren<QWidget*>();
                        if (allWidgets.size() == 0)
                            return false;
                        w = allWidgets.front();

                        isSourceRibbonBar = isSourceRibbonBar || qobject_cast<RibbonBar *>(w);
                        isSourceRibbonPage = isSourceRibbonPage || qobject_cast<RibbonPage *>(w);
                        isSourceRibbonGroup = isSourceRibbonGroup || qobject_cast<RibbonGroup *>(w);
                        //isSourceLayout = isSourceLayout || qobject_cast<QLayout *>(w);
                    }
                    const bool isSourceRibbonElement = isSourceRibbonBar || isSourceRibbonPage || isSourceRibbonGroup;
                    
                    if (!isSourceRibbonElement && !isTargetRibbonElement)
                        return false;

                    if (
                        (targetRibbonBar && isSourceRibbonPage) || 
                        (targetRibbonPage && isSourceRibbonGroup))
                    {
                        dEvent->accept();
                        return true;
                    }

                    dEvent->ignore();
                    return true;
                }
            }                
        }
        break;
    case QEvent::DragLeave:
        {
#if 0
            if (QDragLeaveEvent* dEvent = static_cast<QDragLeaveEvent*>(event))
            {
            }
#endif
        }
        break;
    case QEvent::Drop:
        {
            if (QDropEvent* dEvent = static_cast<QDropEvent*>(event))
            {
                const QMetaObject* mObject = dEvent->mimeData()->metaObject();
                const char * name = mObject ? mObject->className() : Q_NULL;
                if (name == QString("qdesigner_internal::ActionRepositoryMimeData"))
                    return false;

                const QDesignerMimeData_fake* mimedata = static_cast<const QDesignerMimeData_fake *>(dEvent->mimeData());
                if (mimedata)
                {
                    RibbonBar* sourceRibbonBar = Q_NULL;
                    RibbonPage* sourceRibbonPage = Q_NULL;
                    RibbonGroup* sourceRibbonGroup = Q_NULL;

                    //bool isSourceLayout = false;
                    
                    for (QList<QDesignerDnDItemInterface *>::ConstIterator it = mimedata->items().constBegin(); it != mimedata->items().constEnd(); ++it)
                    {
                        QWidget* w = (*it)->widget();
                        if (w == 0)
                            w = (*it)->decoration();

                        QList<QWidget *> allWidgets = w->findChildren<QWidget*>();
                        if (allWidgets.size() == 0)
                            return false;
                        w = allWidgets.front();
        
                        if (sourceRibbonBar == Q_NULL && qobject_cast<RibbonBar *>(w))
                            sourceRibbonBar = qobject_cast<RibbonBar *>(w);
                        if (sourceRibbonPage == Q_NULL && qobject_cast<RibbonPage *>(w))
                            sourceRibbonPage = qobject_cast<RibbonPage *>(w);
                        if (sourceRibbonGroup == Q_NULL && qobject_cast<RibbonGroup *>(w))
                            sourceRibbonGroup = qobject_cast<RibbonGroup *>(w);

                        //isSourceLayout = isSourceLayout || qobject_cast<QLayout *>(w);
                    }
                    const bool isSourceRibbonElement = sourceRibbonBar || sourceRibbonPage || sourceRibbonGroup;
                    
                    if (!isSourceRibbonElement && !isTargetRibbonElement)
                        return false;

                    if (targetRibbonBar && sourceRibbonPage)
                    {
                        if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)targetRibbonBar)) 
                        {
                            AddRibbonPageCommand* cmd = new AddRibbonPageCommand(fw);
                            cmd->init(targetRibbonBar, sourceRibbonPage);
                            cmd->redo();
                        }
                        return true;
                    }
                    else if (targetRibbonPage && sourceRibbonGroup)
                    {
                        if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow(targetRibbonPage)) 
                        {
                            AddRibbonGroupCommand* cmd = new AddRibbonGroupCommand(fw);
                            cmd->init(targetRibbonPage, sourceRibbonGroup);
                            cmd->redo();
                        }
                    }
                    return true;
                }
            }
        }
        break;
    default:
        break;
    }

    if (m_formWindow == obj)
    {
        if (event->type() == QEvent::DragEnter &&  g_dragStyle)
        {
            if (QDragEnterEvent* dragEvent = static_cast<QDragEnterEvent*>(event))
            {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                const QMimeData* mimeData = dragEvent->mimeData();
#else
                const QMimeData* mimeData = qobject_cast<const QMimeData*>(dragEvent->mimeData());
#endif
                if (!mimeData /*&& !drag*/)
                    return false;

                if (QMainWindow* mw = qobject_cast<QMainWindow*>(m_formWindow->mainContainer()))
                {
                    if (verifyExistenceStyle(g_dragStyle))
                        return true;

                    if (QWidget* wd = mw->childAt(mw->mapFromGlobal(QCursor::pos())))
                        return wd->metaObject()->className() != QString("QDesignerWidget");
                }
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
