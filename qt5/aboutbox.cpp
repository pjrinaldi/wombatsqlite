#include "aboutbox.h"

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

AboutBox::AboutBox(QWidget* parent) : QDialog(parent), ui(new Ui::AboutBox)
{
    ui->setupUi(this);
    QString abouttext = "<h3>About Wombat SQLite Forensics v0.1</h3>";
    abouttext += "<h5>License: GPLv2</h5>";
    abouttext += "<h4>Copyright 2022-2023 Pasquale J. Rinaldi, Jr.</h4>";
    abouttext += "<h5>Email: pjrinaldi@gmail.com</h5>";
    abouttext += "<p>Wombat SQLite Forensics parses a SQLite file (database, WAL, JOURNAL) to manually parse the files from a forensic perspective rather than a simple database browser.</p>";
    abouttext += "<p>This program incorporates Qt5.15.0, provided by the LGPL, some of the elementary (GPLv3), and tango (public domain) icons.</p>";
    abouttext += "<p>The wombat icon as well as the About dialog graphic was designed by Lindsey Pargman and a lot of the design implementation ideas were brainstormed with Mark Smith.</p>";
    abouttext += "<p>If I have forgotten to mention any library or anyone, let me know and I will update this content accordingly.</p>";
    ui->textlabel->setText(abouttext);
    this->hide();
}

AboutBox::~AboutBox()
{
}

void AboutBox::HideClicked()
{
    this->hide();
}

void AboutBox::ShowAbout()
{
    this->show();
}

void AboutBox::mousePressEvent(QMouseEvent* e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        this->hide();
    }
}

