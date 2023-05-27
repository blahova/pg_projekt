#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"

struct Objekt {
	QVector<QPoint> body;
	int id, z;
	bool vypln=0;
	QColor color;
};

//id 0=polygon
//id 1=usecka
//id 2=obdlznik
//id 3=kruznica
//id 4=bezierova krivka

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;

	QColor globalColor;
	QSettings settings;
	QMessageBox msgBox;
	

	bool posuvam = false;
	QVector <Objekt> objekty;
	Objekt o;
	QPoint lastPos;

	//pre kruznicu
	QPoint stred;


	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);

private slots:
	void zobraz_objekty();
	void kresli_objekty();

	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionSave_as_data_triggered();

	//Tools slots
	void on_pushButtonSetColor_clicked();
	void on_pushButton_vymaz_clicked();
	void on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void on_checkBox_vypln_stateChanged(int state);
	void on_buttonGroup_buttonClicked(QAbstractButton* button);
	
	void on_pushButton_otocenie_clicked();
	void on_pushButton_skalovanie_clicked();
	void on_pushButton_osova_clicked();
	void on_pushButton_z_clicked();
};
