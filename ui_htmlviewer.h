/********************************************************************************
** Form generated from reading UI file 'htmlviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HTMLVIEWER_H
#define UI_HTMLVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_HtmlViewer
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *homebutton;
    QPushButton *backbutton;
    QPushButton *forbutton;
    QSpacerItem *horizontalSpacer;
    QTextBrowser *textbrowser;

    void setupUi(QDialog *HtmlViewer)
    {
        if (HtmlViewer->objectName().isEmpty())
            HtmlViewer->setObjectName(QString::fromUtf8("HtmlViewer"));
        HtmlViewer->resize(640, 480);
        HtmlViewer->setSizeGripEnabled(true);
        verticalLayout = new QVBoxLayout(HtmlViewer);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(6, 6, 6, 6);
        homebutton = new QPushButton(HtmlViewer);
        homebutton->setObjectName(QString::fromUtf8("homebutton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/bar/home"), QSize(), QIcon::Normal, QIcon::Off);
        homebutton->setIcon(icon);
        homebutton->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(homebutton);

        backbutton = new QPushButton(HtmlViewer);
        backbutton->setObjectName(QString::fromUtf8("backbutton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/bar/prev"), QSize(), QIcon::Normal, QIcon::Off);
        backbutton->setIcon(icon1);
        backbutton->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(backbutton);

        forbutton = new QPushButton(HtmlViewer);
        forbutton->setObjectName(QString::fromUtf8("forbutton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/bar/next"), QSize(), QIcon::Normal, QIcon::Off);
        forbutton->setIcon(icon2);
        forbutton->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(forbutton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        textbrowser = new QTextBrowser(HtmlViewer);
        textbrowser->setObjectName(QString::fromUtf8("textbrowser"));
        textbrowser->setUndoRedoEnabled(false);
        textbrowser->setAcceptRichText(false);

        verticalLayout->addWidget(textbrowser);


        retranslateUi(HtmlViewer);

        QMetaObject::connectSlotsByName(HtmlViewer);
    } // setupUi

    void retranslateUi(QDialog *HtmlViewer)
    {
        HtmlViewer->setWindowTitle(QCoreApplication::translate("HtmlViewer", "Html Viewer", nullptr));
        homebutton->setText(QString());
        backbutton->setText(QString());
        forbutton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class HtmlViewer: public Ui_HtmlViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HTMLVIEWER_H
