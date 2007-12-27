//
// C++ Interface: systemview
//
// Description: 
//
//
// Author: Peter Grasch <bedahr@gmx.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SYSTEMVIEW_H
#define SYSTEMVIEW_H

#include "inlinewidget.h"
#include "simonlistwidget.h"


class QListWidget;
class QTextEdit;
class QHBoxLayout;
class QStackedLayout;
class QPushButton;
class SystemWidget;
class Stack;
class QLineEdit;
class ShortcutControl;

/**
 \class SystemView
 \brief InlineWidget-Container that displays and manages SystemWidgets

Container for all the SystemWidgets;

Does all the basic stuff:
	* Manages its widgets
	* Maybe Password Protection later on...
	* etc.

	@author Peter Grasch <bedahr@gmx.net>
*/
class SystemView : public InlineWidget
{
Q_OBJECT

private slots:
	void displayId(int id);

private:
	QLabel *menueHeader;
	SimonListWidget *selectControl;
	QLineEdit *selectMenue;
	QTextEdit *help;
	QPushButton *pbApply, *pbReset;//, *pbOk;
	QStackedLayout *controls;
	void setupUi(QWidget *parent);

public slots:
	void apply();
	void reset();
public:
    SystemView(ShortcutControl *shortcutctrl, QWidget* parent);

	void registerControl(SystemWidget* control);
	void deleteControl(SystemWidget* control);
	
    ~SystemView();

signals:
	void guiAction(QString);
	void commandsChanged();

};

#endif
