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
#ifndef QTN_RIBBON_STYLE_DSGN_H
#define QTN_RIBBON_STYLE_DSGN_H

#include <QMainWindow>
#include <QtDesigner/QExtensionManager>

#if QT_VERSION >= 0x050500
    #include <QtUiPlugin/QDesignerCustomWidgetInterface>
#else
    #include <QtDesigner/QDesignerCustomWidgetInterface>
#endif
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerContainerExtension>

#include "QtnRibbonStyle.h"

namespace Qtitan
{
    class DsgnFormWindowManagerFilter;

    /* DsgnRibbonStyle */
    class DsgnRibbonStyle: public RibbonStyle
    {
//        Q_OBJECT
    public:
        DsgnRibbonStyle(QWidget* parent);
    public:
        virtual QWidgetList getAllWidgets() const;
        virtual QWidget* parentStyle() const;
        virtual void setParentStyle(QWidget* parent) { m_parent = parent; }
    protected:
        QWidget* m_parent;
    };

    /* DsgnOfficeStyle */
    class DsgnOfficeStyle: public OfficeStyle
    {
//        Q_OBJECT
    public:
        DsgnOfficeStyle(QWidget* parent);
    public:
        virtual QWidgetList getAllWidgets() const;
        virtual QWidget* parentStyle() const;
        virtual void setParentStyle(QWidget* parent) { m_parent = parent; };
    protected:
        QWidget* m_parent;
    };

    /* DesignerStyleInput */
    //  QPlatformInputContext
    class DesignerStyleInput : public QObject//QInputContext
    {
        Q_OBJECT
    public:
        DesignerStyleInput(QWidget* widget);
        ~DesignerStyleInput();
    public:
        virtual QString identifierName() { return QString(); }
        virtual QString language() { return QString(); }
        virtual void reset() {}
        virtual bool isComposing() const { return false; }
        virtual bool filterEvent(const QEvent* event);

    public Q_SLOTS:
        void removeStyle();

    private:
        QWidget* m_widget;
        QAction* m_actRemoveStyle;
    };

    /* RibbonStyleWidget */
    class DsgnRibbonStyleWidget: public QWidget
    {
        Q_OBJECT
    public:
        DsgnRibbonStyleWidget(QWidget* parent);
        virtual ~DsgnRibbonStyleWidget();
    public:
        QStyle* targetStyle() const;
        virtual QSize minimumSizeHint() const;
    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent *);
    private:
        QStyle* m_targetStyle;
        QMainWindow* m_mainWindow;
    };

    /* RibbonStyleFake */
    class RibbonStyleFake : public DsgnRibbonStyleWidget
    {
    public:
        RibbonStyleFake(QWidget* parent);
        virtual ~RibbonStyleFake();
    public:
        virtual const QMetaObject* metaObject() const;
        void* qt_metacast(const char *_clname);
    protected:
        QMetaObject* m_metaObject;
    };
    
    /* DsgnOfficeStyleWidget */
    class DsgnOfficeStyleWidget: public QWidget
    {
        Q_OBJECT
    public:
        DsgnOfficeStyleWidget(QWidget* parent);
        virtual ~DsgnOfficeStyleWidget();
    public:
        QStyle* targetStyle() const;
        virtual QSize minimumSizeHint() const;
    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent *);
    private:
        QStyle* m_targetStyle;
    };

    /* OfficeStyleFake */
    class OfficeStyleFake : public DsgnOfficeStyleWidget
    {
    public:
        OfficeStyleFake(QWidget* parent);
        virtual ~OfficeStyleFake();
    public:
        virtual const QMetaObject *metaObject() const;
        void* qt_metacast(const char *_clname);
    protected:
        QMetaObject* m_metaObject;
    };

    QWidget* create_fake_ribbon_style_widget(QWidget* parent);
    QWidget* create_fake_office_style_widget(QWidget* parent);

    /* ExtensionManagerWrapper */
    class ExtensionManagerWrapper : public QExtensionManager
    {
        Q_OBJECT
        Q_INTERFACES(QAbstractExtensionManager)
    public:
        ExtensionManagerWrapper(QExtensionManager *nativeManager = 0);
        virtual ~ExtensionManagerWrapper();
        void setStyleExtentions();
        virtual void registerExtensions(QAbstractExtensionFactory *factory, const QString &iid = QString());
        virtual void unregisterExtensions(QAbstractExtensionFactory *factory, const QString &iid = QString());
        virtual QObject *extension(QObject *object, const QString &iid) const;
        QExtensionManager* nativeManager() const;
    private:
        QExtensionManager* m_nativeManager;
    };

    /* DesignerMainWindowStyleContainer */
    class DesignerMainWindowStyleContainer: public QObject, public QDesignerContainerExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerContainerExtension)
    public:
        DesignerMainWindowStyleContainer(QDesignerContainerExtension* nativeContainer, QMainWindow* mainWindow, QObject* parent);
    public:
        void addWidget(QWidget *widget);
        int count() const;
        int currentIndex() const;
        void insertWidget(int index, QWidget *widget);
        void remove(int index);
        void setCurrentIndex(int index);
        QWidget *widget(int index) const;
        QMainWindow* mainWindow() const;
    private:
        QDesignerContainerExtension* m_nativeContainer;
        QMainWindow* m_mainWindow;
        QWidget* m_styleWidget;
    };


    /* DesignerMainWindowStyleContainerFactory */
    class DesignerMainWindowStyleContainerFactory: public QExtensionFactory
    {
        Q_OBJECT
    public:
        DesignerMainWindowStyleContainerFactory(ExtensionManagerWrapper* parent = Q_NULL);
    protected:
        QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    private:
        QExtensionManager* m_nativeManager;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    class FilterFormWindowManager;
    class OfficeStyleDsgnPlugin : public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        OfficeStyleDsgnPlugin(QObject *parent = 0);
        bool isContainer() const;
        bool isInitialized() const;
        QIcon icon() const;
        QString domXml() const;
        QString group() const;
        QString includeFile() const;
        QString name() const;
        QString toolTip() const;
        QString whatsThis() const;
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);
    private Q_SLOTS:
        void formWindowAdded(QDesignerFormWindowInterface *formWindow);
        void formWindowRemoved(QDesignerFormWindowInterface *formWindow);
        void widgetManaged(QWidget* widget);

    private:
        bool initialized;
        QDesignerFormEditorInterface* m_core;
        DsgnFormWindowManagerFilter* m_filter;
    };

    /* RibbonStyleDsgnPlugin */
    class RibbonStyleDsgnPlugin : public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        RibbonStyleDsgnPlugin(QObject *parent = 0);
        bool isContainer() const;
        bool isInitialized() const;
        QIcon icon() const;
        QString domXml() const;
        QString group() const;
        QString includeFile() const;
        QString name() const;
        QString toolTip() const;
        QString whatsThis() const;
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);
    private Q_SLOTS:
        void formWindowAdded(QDesignerFormWindowInterface *formWindow);
        void formWindowRemoved(QDesignerFormWindowInterface *formWindow);
        void widgetManaged(QWidget* widget);
    private:
        bool initialized;
        QDesignerFormEditorInterface* m_core;
    };

    /* DsgnFormWindowManagerFilter */
    class DsgnFormWindowManagerFilter: public QObject
    {
    public:
        DsgnFormWindowManagerFilter(QDesignerFormEditorInterface* core, QDesignerFormWindowInterface* formWindow, QObject* parent);
    protected:
        bool verifyExistenceStyle(QWidget* style);
    protected:
        virtual bool eventFilter(QObject* o, QEvent* event);
    protected:
        QDesignerFormEditorInterface* m_core;
        QDesignerFormWindowInterface* m_formWindow;
    };

};

#endif //QTN_RIBBON_STYLE_DSGN_H

