#pragma once
#include <QtWidgets>

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;
	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);
	QList<QList <int>> Z;

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a = 255);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
	void setPixel(int x, int y, const QColor& color, int z);
	//test
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }
	bool isInside(QVector<QPoint>& v);	//toto bude overovat cely vektor ci je vnutri

	//Draw functions
	void drawLine(QPoint start, QPoint end, QColor color,int z, int algType = 0);
	void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
	QPoint getDrawLineBegin() { return drawLineBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }
	void resetBuffer();

	//Alghoritmy
	void DDA(QPoint A, QPoint B, QColor color, int z);
	void DDA_X(QPoint A, QPoint B, double m, QColor color, int z);
	void DDA_Y(QPoint A, QPoint B, double m, QColor color, int z);
	void Bresenham(QPoint A, QPoint B, QColor color, int z);
	void Bresenham_X(QPoint A, QPoint B,double m, QColor color, int z);
	void Bresenham_Y(QPoint A, QPoint B, double m, QColor color, int z);
	void ScanLine(QVector<QPoint>& v, QColor color, int z);
	void fillTriangle(QVector<QPoint> v, QColor color, int z);
	void drawKruznica(QPoint stred, QPoint bod, QColor color, int z);

	void CyrusBeck(QVector <QPoint>& v, QVector<QPoint>& orez);
	QVector<QPoint> SutherlandZlava(QVector <QPoint>& v, int xmin);
	QVector<QPoint> Sutherland(QVector <QPoint> v);

	void Bezier(QVector<QPoint>& v, QColor color, int z);



	void drawPolygon(QVector <QPoint>& v, QColor color,int z, int algType = 0);
	void checkPolygon(QVector <QPoint>& v, QColor color, bool vypln, int z, int algType = 0);

	void posun(QVector <QPoint>& v, int x, int y);
	void otoc(QVector <QPoint> &v, int stupne);
	void skaluj(QVector <QPoint>& v, double zvislo, double vodorovne);
	void preklop(QVector <QPoint>& v);
	void skos(QVector<QPoint>& v, double koef);
	void vypln(QVector<QPoint>& v, QColor color, int z);

	//Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void clear();

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};