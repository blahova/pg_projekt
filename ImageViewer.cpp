#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	globalColor = Qt::blue;
	QString style_sheet = QString("background-color: #%1;").arg(globalColor.rgba(), 0, 16);
	ui->pushButtonSetColor->setStyleSheet(style_sheet);
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton && ui->radioButton_polygon->isChecked())
	{
		if (w->getDrawLineActivated())	//ak to nie je prvy bod
		{
			w->drawLine(w->getDrawLineBegin(), e->pos(), Qt::blue, ui->comboBoxLineAlg->currentIndex());
			o.body.push_back(e->pos());
			w->setDrawLineBegin(e->pos());
		}
		else	//ak to je prvy bod  v polygone
		{
			o.body.clear();
			o.body.push_back(e->pos());
			o.id = 0;
			o.color = Qt::blue;
			o.z = objekty.size();
			o.vypln = ui->checkBox_vypln->isChecked();
			w->setDrawLineBegin(e->pos());
			w->setDrawLineActivated(true);
			w->setPixel(e->pos().x(), e->pos().y(), o.color,o.z);
			w->update();
		}
	}
	else if (e->button() == Qt::RightButton && ui->radioButton_polygon->isChecked())
	{
		if (o.body.size() > 2)	//ak ma dost bodov na vykreslenie polyonu
		{
			objekty.push_back(o);
			kresli_objekty();
			w->setDrawLineActivated(false);
			ui->pushButton_vymaz->setEnabled(true);	//zapnem vymazavanie
			ui->groupBox_transform->setEnabled(true);	//zapnem transformacie
			zobraz_objekty();

			ui->buttonGroup->setExclusive(false);
			ui->buttonGroup->checkedButton()->setChecked(false);
			ui->buttonGroup->setExclusive(true);

			int index = objekty.size()-1;
			QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(0);
			QTreeWidgetItem* item = parentItem->child(index);
			ui->treeWidget->setCurrentItem(item);
		}
		else
		{
			ui->statusBar->showMessage("Pre polygon bolo zadanych malo bodov",2000);
		}
	}
	else if (ui->radioButton_usecka->isChecked())
	{
		if (w->getDrawLineActivated())	//ak to nie je prvy bod
		{
			o.body.push_back(e->pos());
			objekty.push_back(o);
			kresli_objekty();
			w->setDrawLineActivated(false);
			ui->pushButton_vymaz->setEnabled(true);	//zapnem vymazavanie
			ui->groupBox_transform->setEnabled(true);	//zapnem transformacie
			zobraz_objekty();

			ui->buttonGroup->setExclusive(false);
			ui->buttonGroup->checkedButton()->setChecked(false);
			ui->buttonGroup->setExclusive(true);

			int index = objekty.size() - 1;
			QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(0);
			QTreeWidgetItem* item = parentItem->child(index);
			ui->treeWidget->setCurrentItem(item);
		}
		else	//ak to je prvy bod  v usecke
		{
			o.body.clear();
			o.body.push_back(e->pos());
			o.id = 1;
			o.color = Qt::blue;
			o.z = objekty.size();
			w->setDrawLineBegin(e->pos());
			w->setDrawLineActivated(true);
			w->setPixel(e->pos().x(), e->pos().y(), o.color, o.z);
			w->update();
		}
	}
	else if (ui->radioButton_obdlznik->isChecked())
	{
		if (w->getDrawLineActivated())	//ak to nie je prvy bod
		{
			QPoint bod(e->pos().x(),o.body[0].y());
			o.body.push_back(bod);
			o.body.push_back(e->pos());
			bod = QPoint(o.body[0].x(), o.body[2].y());
			o.body.push_back(bod);

			objekty.push_back(o);
			kresli_objekty();
			w->setDrawLineActivated(false);
			ui->pushButton_vymaz->setEnabled(true);	//zapnem vymazavanie
			ui->groupBox_transform->setEnabled(true);	//zapnem transformacie
			zobraz_objekty();

			ui->buttonGroup->setExclusive(false);
			ui->buttonGroup->checkedButton()->setChecked(false);
			ui->buttonGroup->setExclusive(true);

			int index = objekty.size() - 1;
			QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(0);
			QTreeWidgetItem* item = parentItem->child(index);
			ui->treeWidget->setCurrentItem(item);
		}
		else	//ak to je prvy bod  v usecke
		{
			o.body.clear();
			o.body.push_back(e->pos());
			o.id = 2;
			o.color = Qt::blue;
			o.z = objekty.size();
			o.vypln = ui->checkBox_vypln->isChecked();
			w->setDrawLineBegin(e->pos());
			w->setDrawLineActivated(true);
			w->setPixel(e->pos().x(), e->pos().y(), o.color,o.z);
			w->update();
		}
	}

	else if (e->button() == Qt::LeftButton && ui->radioButton_kruznica->isChecked())
	{
		if (w->getDrawLineActivated())	//ak to nie je prvy bod
		{
			o.body.push_back(e->pos());
			objekty.push_back(o);
			kresli_objekty();
			w->setDrawLineActivated(false);
			ui->pushButton_vymaz->setEnabled(true);	//zapnem vymazavanie
			ui->groupBox_transform->setEnabled(true);	//zapnem transformacie
			zobraz_objekty();

			ui->buttonGroup->setExclusive(false);
			ui->buttonGroup->checkedButton()->setChecked(false);
			ui->buttonGroup->setExclusive(true);

			int index = objekty.size() - 1;
			QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(0);
			QTreeWidgetItem* item = parentItem->child(index);
			ui->treeWidget->setCurrentItem(item);
		}
		else	//ak to je stred
		{
			o.body.clear();
			o.body.push_back(e->pos());
			o.id = 3;
			o.color = Qt::blue;
			o.z = objekty.size();
			w->setDrawLineBegin(e->pos());
			w->setDrawLineActivated(true);
			w->setPixel(e->pos().x(), e->pos().y(), o.color,o.z);
			w->update();
		}
	}
	if (e->button() == Qt::LeftButton && ui->radioButton_bezier->isChecked())
	{
		if (w->getDrawLineActivated())	//ak to nie je prvy bod
		{
			o.body.push_back(e->pos());
		}
		else	//ak to je prvy bod 
		{
			o.body.clear();
			o.body.push_back(e->pos());
			o.id = 4;
			o.color = Qt::blue;
			o.z = objekty.size();
			o.vypln = ui->checkBox_vypln->isChecked();
			w->setDrawLineActivated(true);
			w->setPixel(e->pos().x(), e->pos().y(), o.color,o.z);
			w->update();
		}
	}
	else if (e->button() == Qt::RightButton && ui->radioButton_bezier->isChecked())
	{
		if (o.body.size() > 2)	//ak ma dost bodov na vykreslenie polyonu
		{
			objekty.push_back(o);
			kresli_objekty();
			w->setDrawLineActivated(false);
			ui->pushButton_vymaz->setEnabled(true);	//zapnem vymazavanie
			ui->groupBox_transform->setEnabled(true);	//zapnem transformacie
			zobraz_objekty();

			ui->buttonGroup->setExclusive(false);
			ui->buttonGroup->checkedButton()->setChecked(false);
			ui->buttonGroup->setExclusive(true);

			int index = objekty.size() - 1;
			QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(0);
			QTreeWidgetItem* item = parentItem->child(index);
			ui->treeWidget->setCurrentItem(item);
		}
		else
		{
			ui->statusBar->showMessage("Pre krivku bolo zadanych malo bodov", 2000);
		}
	}
	

	else if (!o.body.isEmpty() && e->button() == Qt::LeftButton)
	{
		lastPos = e->pos();
		posuvam = true;
	}
	//qDebug() << body.size();

}

void ImageViewer::zobraz_objekty()
{
	ui->treeWidget->blockSignals(true);
	ui->treeWidget->clear(); 
	QTreeWidgetItem* parent = new QTreeWidgetItem(ui->treeWidget);
	parent->setText(0, "Objekty");
	for (size_t i = 0; i < objekty.size(); i++)
	{
		QTreeWidgetItem* child = new QTreeWidgetItem();
		child->setText(0, QString("%1").arg(i));
		if (objekty[i].id == 0)
		{
			child->setText(1, QString("%1").arg("polygon"));
			child->setText(2, QString("%1").arg(objekty[i].z));
		}
		else if (objekty[i].id == 1)
		{
			child->setText(1, QString("%1").arg("usecka"));
			child->setText(2, QString("%1").arg(objekty[i].z));
		}
		else if (objekty[i].id == 2)
		{
			child->setText(1, QString("%1").arg("obdlznik"));
			child->setText(2, QString("%1").arg(objekty[i].z));
		}
		else if (objekty[i].id == 3)
		{
			child->setText(1, QString("%1").arg("kruznica"));
			child->setText(2, QString("%1").arg(objekty[i].z));
		}
		else
		{
			child->setText(1, QString("%1").arg("bezier"));
			child->setText(2, QString("%1").arg(objekty[i].z));
		}
		parent->addChild(child);
	}
	ui->treeWidget->expandItem(parent);
	ui->treeWidget->blockSignals(false);
}

void ImageViewer::kresli_objekty()
{
	vW->clear();
	for (int i = 0; i<objekty.size(); i++)
	{
		if (objekty[i].id == 0 || objekty[i].id==1 || objekty[i].id==2)
		{
			vW->checkPolygon(objekty[i].body, objekty[i].color,objekty[i].vypln, objekty[i].z);
		}
		else if (objekty[i].id == 3)
		{
			vW->drawKruznica(objekty[i].body[0], objekty[i].body[1], objekty[i].color, objekty[i].z);
		}
		else
		{
			vW->Bezier(objekty[i].body, objekty[i].color, objekty[i].z);
		}
	}
}

void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	posuvam = false;
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	QPoint currentPos = e->pos();
	int deltaX = currentPos.x() - lastPos.x();
	int deltaY = currentPos.y() - lastPos.y();
	QVector<QPoint> orezane{};

	if (posuvam && !objekty.isEmpty())	//takto je to cez logicku premennu posuvam, true je ak uz je nieco nakreslene a nasledne stlacim lave tlacitko
	{
		w->posun(objekty[ui->treeWidget->currentIndex().row()].body, deltaX, deltaY);
		kresli_objekty();	
	}
	
	lastPos = currentPos;
	
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}

//Slots
void ImageViewer::on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	QString style_sheet = QString("background-color: #%1;").arg(objekty[ui->treeWidget->currentIndex().row()].color.rgba(), 0, 16);
	ui->pushButtonSetColor->setStyleSheet(style_sheet);
	ui->spinBox_z->setValue(objekty[ui->treeWidget->currentIndex().row()].z);
}


void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	vW->clear();
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

void ImageViewer::on_pushButton_z_clicked()
{
	int index = ui->treeWidget->currentIndex().row();
	objekty[index].z = ui->spinBox_z->value();
	kresli_objekty();
	zobraz_objekty();
	QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(0);
	QTreeWidgetItem* item = parentItem->child(index);
	ui->treeWidget->setCurrentItem(item);
}

void ImageViewer::on_pushButtonSetColor_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;
		objekty[ui->treeWidget->currentIndex().row()].color = newColor;
		kresli_objekty();
	}
}

void ImageViewer::on_pushButton_vymaz_clicked()
{
	vW->clear();
	ui->groupBox_kreslenie->setEnabled(true);
	ui->pushButton_vymaz->setEnabled(false);
	ui->groupBox_transform->setEnabled(false);
	objekty.clear();
}



void ImageViewer::on_pushButton_otocenie_clicked()
{
	vW->otoc(objekty[ui->treeWidget->currentIndex().row()].body, ui->spinBox_stupne->value());
	kresli_objekty();
}

void ImageViewer::on_pushButton_skalovanie_clicked()
{
	vW->skaluj(objekty[ui->treeWidget->currentIndex().row()].body, ui->doubleSpinBox_zvislo->value(), ui->doubleSpinBox_vodorovne->value());
	kresli_objekty();
}

void ImageViewer::on_pushButton_osova_clicked()
{
	vW->preklop(objekty[ui->treeWidget->currentIndex().row()].body);
	kresli_objekty();
}