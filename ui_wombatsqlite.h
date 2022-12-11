/********************************************************************************
** Form generated from reading UI file 'wombatsqlite.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WOMBATSQLITE_H
#define UI_WOMBATSQLITE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WombatSqlite
{
public:
    QAction *actionOpenDB;
    QAction *actionManageTags;
    QAction *actionAbout;
    QAction *actionPreviewReport;
    QAction *actionPublish;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *pagespinbox;
    QLabel *countlabel;
    QSpacerItem *horizontalSpacer;
    QSplitter *splitter_2;
    QListWidget *treewidget;
    QSplitter *splitter;
    QTextEdit *textedit;
    QTableWidget *tablewidget;
    QTableWidget *propwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *WombatSqlite)
    {
        if (WombatSqlite->objectName().isEmpty())
            WombatSqlite->setObjectName(QString::fromUtf8("WombatSqlite"));
        WombatSqlite->resize(1022, 712);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/wombat"), QSize(), QIcon::Normal, QIcon::Off);
        WombatSqlite->setWindowIcon(icon);
        actionOpenDB = new QAction(WombatSqlite);
        actionOpenDB->setObjectName(QString::fromUtf8("actionOpenDB"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/bar/open"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenDB->setIcon(icon1);
        actionManageTags = new QAction(WombatSqlite);
        actionManageTags->setObjectName(QString::fromUtf8("actionManageTags"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/bar/managetags"), QSize(), QIcon::Normal, QIcon::Off);
        actionManageTags->setIcon(icon2);
        actionAbout = new QAction(WombatSqlite);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/help"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon3);
        actionPreviewReport = new QAction(WombatSqlite);
        actionPreviewReport->setObjectName(QString::fromUtf8("actionPreviewReport"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/bar/previewreport"), QSize(), QIcon::Normal, QIcon::Off);
        actionPreviewReport->setIcon(icon4);
        actionPublish = new QAction(WombatSqlite);
        actionPublish->setObjectName(QString::fromUtf8("actionPublish"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/bar/publish"), QSize(), QIcon::Normal, QIcon::Off);
        actionPublish->setIcon(icon5);
        centralwidget = new QWidget(WombatSqlite);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(0, 0));
        label->setMaximumSize(QSize(16777215, 26));

        horizontalLayout->addWidget(label);

        pagespinbox = new QSpinBox(centralwidget);
        pagespinbox->setObjectName(QString::fromUtf8("pagespinbox"));
        pagespinbox->setMinimumSize(QSize(75, 0));
        pagespinbox->setMaximumSize(QSize(16777215, 26));
        pagespinbox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pagespinbox->setMinimum(1);

        horizontalLayout->addWidget(pagespinbox);

        countlabel = new QLabel(centralwidget);
        countlabel->setObjectName(QString::fromUtf8("countlabel"));
        countlabel->setMinimumSize(QSize(0, 0));
        countlabel->setMaximumSize(QSize(16777215, 26));

        horizontalLayout->addWidget(countlabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        splitter_2 = new QSplitter(centralwidget);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        treewidget = new QListWidget(splitter_2);
        treewidget->setObjectName(QString::fromUtf8("treewidget"));
        treewidget->setProperty("showDropIndicator", QVariant(false));
        treewidget->setAlternatingRowColors(true);
        treewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        treewidget->setUniformItemSizes(true);
        splitter_2->addWidget(treewidget);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy1);
        splitter->setOrientation(Qt::Vertical);
        textedit = new QTextEdit(splitter);
        textedit->setObjectName(QString::fromUtf8("textedit"));
        sizePolicy1.setHeightForWidth(textedit->sizePolicy().hasHeightForWidth());
        textedit->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QString::fromUtf8("Noto Sans Mono"));
        textedit->setFont(font);
        textedit->setReadOnly(true);
        splitter->addWidget(textedit);
        tablewidget = new QTableWidget(splitter);
        tablewidget->setObjectName(QString::fromUtf8("tablewidget"));
        sizePolicy1.setHeightForWidth(tablewidget->sizePolicy().hasHeightForWidth());
        tablewidget->setSizePolicy(sizePolicy1);
        splitter->addWidget(tablewidget);
        splitter_2->addWidget(splitter);
        propwidget = new QTableWidget(splitter_2);
        if (propwidget->columnCount() < 3)
            propwidget->setColumnCount(3);
        propwidget->setObjectName(QString::fromUtf8("propwidget"));
        propwidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        propwidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        propwidget->setDragDropOverwriteMode(false);
        propwidget->setAlternatingRowColors(true);
        propwidget->setSelectionMode(QAbstractItemView::SingleSelection);
        propwidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        propwidget->setColumnCount(3);
        splitter_2->addWidget(propwidget);
        propwidget->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(splitter_2);

        WombatSqlite->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WombatSqlite);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1022, 22));
        WombatSqlite->setMenuBar(menubar);
        statusbar = new QStatusBar(WombatSqlite);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        WombatSqlite->setStatusBar(statusbar);
        toolBar = new QToolBar(WombatSqlite);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        WombatSqlite->addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addAction(actionOpenDB);
        toolBar->addAction(actionManageTags);
        toolBar->addAction(actionPreviewReport);
        toolBar->addAction(actionPublish);
        toolBar->addAction(actionAbout);

        retranslateUi(WombatSqlite);

        QMetaObject::connectSlotsByName(WombatSqlite);
    } // setupUi

    void retranslateUi(QMainWindow *WombatSqlite)
    {
        WombatSqlite->setWindowTitle(QCoreApplication::translate("WombatSqlite", "Wombat Registry", nullptr));
        actionOpenDB->setText(QCoreApplication::translate("WombatSqlite", "Open Hive", nullptr));
#if QT_CONFIG(shortcut)
        actionOpenDB->setShortcut(QCoreApplication::translate("WombatSqlite", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionManageTags->setText(QCoreApplication::translate("WombatSqlite", "Manage Tags", nullptr));
#if QT_CONFIG(tooltip)
        actionManageTags->setToolTip(QCoreApplication::translate("WombatSqlite", "Manage Tags", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionManageTags->setShortcut(QCoreApplication::translate("WombatSqlite", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("WombatSqlite", "About", nullptr));
#if QT_CONFIG(tooltip)
        actionAbout->setToolTip(QCoreApplication::translate("WombatSqlite", "About", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionAbout->setShortcut(QCoreApplication::translate("WombatSqlite", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPreviewReport->setText(QCoreApplication::translate("WombatSqlite", "Preview Report", nullptr));
#if QT_CONFIG(tooltip)
        actionPreviewReport->setToolTip(QCoreApplication::translate("WombatSqlite", "Preview Report", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionPreviewReport->setShortcut(QCoreApplication::translate("WombatSqlite", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPublish->setText(QCoreApplication::translate("WombatSqlite", "Publish Report", nullptr));
#if QT_CONFIG(tooltip)
        actionPublish->setToolTip(QCoreApplication::translate("WombatSqlite", "Publish Report", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionPublish->setShortcut(QCoreApplication::translate("WombatSqlite", "Ctrl+G", nullptr));
#endif // QT_CONFIG(shortcut)
        label->setText(QCoreApplication::translate("WombatSqlite", "Page: ", nullptr));
        pagespinbox->setSuffix(QString());
        pagespinbox->setPrefix(QString());
        countlabel->setText(QCoreApplication::translate("WombatSqlite", "of ", nullptr));
        textedit->setDocumentTitle(QString());
        toolBar->setWindowTitle(QCoreApplication::translate("WombatSqlite", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WombatSqlite: public Ui_WombatSqlite {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WOMBATSQLITE_H
