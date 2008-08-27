#ifndef SIMONTABLEWIDGET_H
#define SIMONTABLEWIDGET_H

#include <QTableWidget>
#include <QVector>
#include "simonlineedit.h"

class QLineEdit;


class SimonTableWidget : public QTableWidget
{	
	
	Q_OBJECT
	
private:
	
	bool redFlag;
	QVector<SimonLineEdit*> linevector;
	int count;

public:
	SimonTableWidget( QWidget * parent  = 0);
	~SimonTableWidget();
	
	void resizeEvent(QResizeEvent * event);
	void controlRedFlag(int lineValue);
	void showEntries();
	void fillVector();
	void invisibleRows(SimonLineEdit * temp, QString text);
	void returnFilterEntries(SimonLineEdit* temp);
	SimonLineEdit* getFocusedLineEdit();
	void invisibleLineEdits();
	QTableWidgetItem* selectItem();
	

public slots:
	void showLineEdit();
	void filterEntries(QString text);
	void initLineEdit(QString action);
	void checkFocus();
	void moveLineEdit();

	

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void focusOutEvent ( QFocusEvent * event );

signals:
    void returnPressed();
	
	
};

#endif

