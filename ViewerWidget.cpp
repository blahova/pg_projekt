#include   "ViewerWidget.h"
#include <algorithm>

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
	resetBuffer();
}
ViewerWidget::~ViewerWidget()
{	
	/*if (painter != nullptr) {
		delete painter;
		delete img;
	}*/
	
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

void ViewerWidget::resetBuffer()
{
	Z.clear();
	Z.resize(img->width());
	for (int i = 0; i < Z.size(); i++)
	{
		Z[i].resize(img->height());
		for (int j = 0; j < Z.size(); j++)
		{
			Z[i][j] = -INT_MAX;
		}
	}
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete painter;
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	setDataPtr();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img == nullptr) {
		return true;
	}

	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

bool ViewerWidget::changeSize(int width, int height)
{
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete painter;
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		update();
	}

	return true;
}

void ViewerWidget::setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a)
{
	r = r > 255 ? 255 : (r < 0 ? 0 : r);
	g = g > 255 ? 255 : (g < 0 ? 0 : g);
	b = b > 255 ? 255 : (b < 0 ? 0 : b);
	a = a > 255 ? 255 : (a < 0 ? 0 : a);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = b;
	data[startbyte + 1] = g;
	data[startbyte + 2] = r;
	data[startbyte + 3] = a;
}
void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA)
{
	valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);
	valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
	valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
	valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = static_cast<uchar>(255 * valB);
	data[startbyte + 1] = static_cast<uchar>(255 * valG);
	data[startbyte + 2] = static_cast<uchar>(255 * valR);
	data[startbyte + 3] = static_cast<uchar>(255 * valA);
}
void ViewerWidget::setPixel(int x, int y, const QColor& color, int z)
{
	if (color.isValid()) {
		size_t startbyte = y * img->bytesPerLine() + x * 4;
		if (y == getImgHeight()) { y = getImgHeight() - 1; }
		if (x == getImgWidth()) { x = getImgWidth() - 1; }
		if (Z[x][y] < z)
		{
			Z[x][y] = z;
			data[startbyte] = color.blue();
			data[startbyte + 1] = color.green();
			data[startbyte + 2] = color.red();
			data[startbyte + 3] = color.alpha();
		}
	}
}

//Draw functions
void ViewerWidget::drawLine(QPoint start, QPoint end, QColor color, int z, int algType)
{
	/*painter->setPen(QPen(color));
	painter->drawLine(start, end);*/

	if (algType == 0)
	{
		DDA(start, end, color,z);
	}
	else
	{
		Bresenham(start, end, color,z);
	}
	update();
}

void ViewerWidget::DDA(QPoint A, QPoint B, QColor color, int z)
{
	double m;

	if ((double)B.x() - A.x()==0)
	{
		m = DBL_MAX;
		DDA_Y(A, B, m, color,z);
	}
	else
	{
		m = (double)(B.y() - A.y()) / (B.x() - A.x());	//smernica
		//qDebug() << B.y() - A.y();
		//qDebug() << B.x() - A.x();
		if (m == 1)	//riadiaca lubovolna
		{
			DDA_X(A, B, m, color,z);
			//qDebug() << "riadiaca lubovolna (x)" << m;
		}
		else if (m > -1 && m < 1)	//riadiaca x
		{
			DDA_X(A, B, m, color,z);
			//qDebug() << "riadiaca x" << m;
		}
		else	//riadiaca y 
		{
			DDA_Y(A, B, m, color,z);
			//qDebug() << "riadiaca y" << m;
		}
	}
}

void ViewerWidget::DDA_X(QPoint A, QPoint B, double m, QColor color, int z)
{
	int x;	//toto budu tie kde zapisujem
	double y;
	if (A.x() <= B.x())	//A je viac vlavo
	{
		x = A.x();
		y = A.y();
		setPixel(x, (int) y,color,z);
		while (x < B.x())
		{
			x++; 
			y = y + m;
			//qDebug() << x << y;
			setPixel(x, (int)(y+0.5), color,z);
		}
	}
	else	//B je viac nalavo ako A tak ich treba zmenit
	{
		DDA_X(B, A, m, color,z);
	}
}
void ViewerWidget::DDA_Y(QPoint A, QPoint B, double m, QColor color, int z)
{
	double x;
	int y;
	if (A.y() <= B.y())	//A je viac hore
	{
		x = A.x();
		y = A.y();
		setPixel((int)x, y, color,z);
		while (y < B.y())
		{
			x = x + 1 / m;
			y++;
			setPixel((int)(x + 0.5), y, color,z);
		}
	}
	else	//B je viac hore
	{
		DDA_Y(B, A, m, color,z);
	}
}

void ViewerWidget::Bresenham(QPoint A, QPoint B, QColor color, int z)
{
	double m;
	m = (double)(B.y() - A.y()) / (B.x() - A.x());	//smernica

	if (m > -1 && m < 1)	//veduca x
	{
		Bresenham_X(A, B, m, color,z);
	}
	else	//riadiaca x
	{
		Bresenham_Y(A, B, m, color,z);
	}
}
void ViewerWidget::Bresenham_X(QPoint A, QPoint B, double m, QColor color, int z)
{
	int k1, k2, p, x, y;
	if (A.x() <= B.x())	//A je viac nalavo
	{
		if (m > 0 && m < 1)
		{
			k1 = 2 * (B.y() - A.y());	
			k2 = k1 - 2 * (B.x() - A.x()); 
			p = k1 - (B.x() - A.x());	
			x = A.x();
			y = A.y();
			setPixel(x, y, color,z);
			while (x<B.x())
			{
				x++;
				if (p > 0)
				{
					p = p + k2;
					y++;
				}
				else
				{
					p = p + k1;
				}
				setPixel(x, y, color,z);
			}
		}
		else
		{
			k1 = 2 * (B.y() - A.y());
			k2 = k1 + 2 * (B.x() - A.x());
			p = k1 + (B.x() - A.x());
			x = A.x();
			y = A.y();
			setPixel(x, y, color,z);
			while (x < B.x())
			{
				x++;
				if (p < 0)
				{
					y--;
					p = p + k2;
				}
				else
				{
					p = p + k1;
				}
				setPixel(x, y, color,z);
			}
		}

	}
	else	//B je viac nalavo
	{
		Bresenham_X(B, A, m,color,z);
	}

}
void ViewerWidget::Bresenham_Y(QPoint A, QPoint B, double m, QColor color, int z)
{
	int k1, k2, p,x,y;
	if (A.y() <= B.y())	//ak A je vyssie
	{
		if (m > 1)
		{
			k1 = 2 * (B.x() - A.x());
			k2 = k1 - 2 * (B.y() - A.y());
			p = k1 - (B.y() - A.y());
			x = A.x();
			y = A.y();
			setPixel(x, y, color,z);
			while (y < B.y())
			{
				y++;
				if (p > 0)
				{
					x++;
					p = p + k2;
				}
				else
				{
					p = p + k1;
				}
				setPixel(x, y, color,z);
			}
		}
		else
		{
			k1 = 2 * (B.x() - A.x());
			k2 = k1 + 2 * (B.y() - A.y());
			p = k1 + (B.y() - A.y());
			x = A.x();
			y = A.y();
			setPixel(x, y, color,z);
			while (y < B.y())
			{
				y++;
				if (p < 0)
				{
					x--;
					p = p + k2;
				}
				else
				{
					p = p + k1;
				}
				setPixel(x, y, color,z);
			}
		}
	}
	else
	{
		Bresenham_Y(B, A, m, color,z);
	}
}

bool ViewerWidget::isInside(QVector<QPoint>& v)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (!isInside(v[i].x(), v[i].y()))	//ak nejaky body sa najde ze nie je v poli kreslenia
		{
			//qDebug() << "nasiel sa bod vonku";
			return false;
		}
	}
	//qDebug() << "nenasiel sa bod vonku";
	return true;
}



void ViewerWidget::CyrusBeck(QVector<QPoint>& v, QVector<QPoint>& orez)		
{
	//v je v tomto pripade sice vektor bodov, ale len dvoch, lebo pracujem s useckou
	//idem najprv otestovat ci sa nachadza aspon jeden z bodov v oblasti kreslenia
	if (isInside(v[0].x(), v[0].y()) || isInside(v[1].x(), v[1].y()))
	{
		double tl = 0, tu = 1, t;
		QPoint d = v[1] - v[0], n, w;
		QVector<QPoint>E{ QPoint(0,0),QPoint(img->width(), 0), QPoint(img->width(),img->height()),QPoint(0,img->height()) };
		for (int i=0; i < E.size(); i++)
		{
			if (i == E.size() - 1)	//ak som na konci tak spajam prvy a posledny
			{
				n = E[0] - E[i];
				n= QPoint(-n.y(), n.x());
			}
			else
			{
				n = E[i+1] - E[i];
				n = QPoint(-n.y(), n.x());
			}
			

			w = v[0] - E[i];
			
			int s1 = QPoint::dotProduct(d, n);
			int s2= QPoint::dotProduct(w, n);

			
			if (s1 != 0)
			{
				t = -(double)s2 / s1;
				if (s1 > 0) 
				{
					tl = qMax(tl, t);
				}
				else 
				{
					tu = qMin(tu, t);
				}
			}
		}

		if (tl < tu)
		{
			QPoint o1, o2;
			o1.setX((int)(v[0].x() + (v[1].x() - v[0].x()) * tl + 0.5));
			o1.setY((int)(v[0].y() + (v[1].y() - v[0].y()) * tl + 0.5));

			o2.setX((int)(v[0].x() + (v[1].x() - v[0].x()) * tu + 0.5));
			o2.setY((int)(v[0].y() + (v[1].y() - v[0].y()) * tu + 0.5));
			orez.append(o1);
			orez.append(o2);
		}
	}

}

QVector<QPoint> ViewerWidget::SutherlandZlava(QVector <QPoint>& v, int xmin)
{
	if (v.isEmpty())
	{
		return v;
	}
	QPoint s = v.last(), p;
	QVector<QPoint> orez;
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i].x() >= xmin)
		{
			if (s.x() >= xmin)
			{
				orez.append(v[i]);
			}
			else
			{
				double koef = (double)(v[i].y() - s.y()) / (v[i].x() - s.x());
				p.setX(xmin);
				p.setY((int)(s.y() + (xmin - s.x()) * koef + 0.5));
				orez.append(p);
				orez.append(v[i]);
			}
		}
		else
		{
			if (s.x() >= xmin)
			{
				double koef = (double)(v[i].y() - s.y()) / (v[i].x() - s.x());
				p.setX(xmin);
				p.setY((int)(s.y() + (xmin - s.x()) * koef + 0.5));
				orez.append(p);
			}
		}
		s = v[i];
	}
	return orez;
}


QVector<QPoint> ViewerWidget::Sutherland(QVector <QPoint> v)
{
	QVector<QPoint> orezany = v;
	QVector<QPoint> E{ QPoint(0,0), QPoint(img->width() - 1,0),QPoint(img->width() - 1 , img->height() - 1), QPoint(0,img->height() - 1) };

	for (int i = 0; i < 4; i++)
	{
		orezany=SutherlandZlava(orezany, E[i].x());

		for (int j = 0; j < E.size(); j++)
		{
			E[j] = QPoint(E[j].y(), -E[j].x());
		}
		for (int j = 0; j < orezany.size(); j++)
		{
			orezany[j] = QPoint(orezany[j].y(), -orezany[j].x());
		}
		//qDebug() << E;
		//qDebug() << orezany;
	}
	return orezany;
}






void ViewerWidget::ScanLine(QVector <QPoint>& v, QColor color, int z)
{
	struct hrana
	{
		QPoint zaciatok;
		QPoint koniec;
		double m;	//smernica

		//udaje do tabulky o hranach na algoritmus
		int deltaY;		//pocet riadkov do ktorych hrana zasahuje
		double x;	//aktualna suradnica priesecniku s rozkladovym riadkom (pociatocna hodnota x = zaciatok.x())
		double w;	//prirastok x pri prechode na dalsi riadok – obratena hodnota smernice w = 1/m
	};

	QList <hrana> hrany;	//tu si poupravujem a ulozim hrany

	hrana h;
	for (int i = 0; i < v.size(); i++)
	{
		if (i == v.size() - 1)	//ak som na konci, tak chcem aby sa ulozil ako koniec hrany uplne prvy bod v polygone (vektor v)
		{
			if (v[i].y() < v[0].y())	//potrebujem si ich spravne orientovat, zhora dole
			{
				h.zaciatok = v[i];
				h.koniec = v[0];
			}
			else if (v[i].y() > v[0].y())
			{
				h.zaciatok = v[0];
				h.koniec = v[i];
			}
			else // v pripade, ze je vodorovna tak ju uplne vynecham
			{
				continue;
			}
		}
		else	//ak som hocikde inde ako na uplnom konci
		{
			if (v[i].y() < v[i + 1].y())	//potrebujem si ich spravne orientovat, zhora dole
			{
				h.zaciatok = v[i];
				h.koniec = v[i + 1];
			}
			else if (v[i].y() > v[i + 1].y())
			{
				h.zaciatok = v[i + 1];
				h.koniec = v[i];
			}
			else // v pripade, ze je vodorovna tak ju uplne vynecham
			{
				continue;
			}
		}
		h.m = (double)(h.koniec.y() - h.zaciatok.y()) / (h.koniec.x() - h.zaciatok.x());
		h.w = 1 / h.m;
		h.koniec.setY(h.koniec.y() - 1);

		h.deltaY = h.koniec.y() - h.zaciatok.y();
		h.x = h.zaciatok.x();
		

		hrany.append(h);
	}

	std::sort(hrany.begin(), hrany.end(), [](hrana a, hrana b)
		{
			return a.zaciatok.y() < b.zaciatok.y();
		});


	//teraz uz by mali byt hrany pripravene ist na samotny algoritmus
	int ymin = hrany.first().zaciatok.y();
	//int ymax = hrany.last().koniec.y();

	int ymax = ymin;
	for (int i = 0; i < hrany.size(); i++)
	{
		if (hrany[i].koniec.y() > ymax)
		{
			ymax = hrany[i].koniec.y();
		}
	}

	QList<QList<hrana>> TH(ymax - ymin + 1);	//priprava tabulky hran

	//qDebug() << "TH SIZE: " << TH.size();

	//naplnenie TH
	for (int i = 0; i < hrany.size(); i++)
	{
		int index = hrany[i].zaciatok.y() - ymin;
		TH[index].append(hrany[i]);
	}

	QList<hrana>ZAH;
	int y = ymin;

	for (int i = 0; i < TH.size(); i++)
	{
		if (!TH[i].isEmpty())	//ak je neprazdny
		{
			for (int j = 0; j < TH[i].size(); j++)
			{
				ZAH.append(TH[i][j]);	//pridavam hrany do zoznamu aktivnych hran
			}
		}
		std::sort(ZAH.begin(), ZAH.end(), [](hrana a, hrana b)
			{
				return a.x < b.x;
			});

		for (int j = 0; j < ZAH.size(); j += 2)
		{
			if ((int)(ZAH[j].x + 0.5) != (int)(ZAH[j + 1].x + 0.5))
			{
				drawLine(QPoint(ZAH[j].x, y), QPoint(ZAH[j + 1].x , y), color,z);
			}
		}

		for (int j = 0; j < ZAH.size(); j++)
		{
			if (ZAH[j].deltaY == 0)
			{
				ZAH.removeAt(j);
				j--;
			}
		}
		for (int j = 0; j < ZAH.size(); j++)
		{
			ZAH[j].deltaY --;
			ZAH[j].x = ZAH[j].x + ZAH[j].w;
		}
		y++;
	}
}

void ViewerWidget::fillTriangle(QVector<QPoint> v, QColor color, int z)
{
	std::sort(v.begin(), v.end(), [](QPoint a, QPoint b)
		{
			if (a.y() == b.y()) {
				return a.x() < b.x();
			}
	return a.y() < b.y();
		});

	struct hrana
	{
		QPoint zaciatok, koniec;
		double m;
		double w;
	};

	hrana e1, e2;
	
	if (v[0].y() == v[1].y())
	{
		//vyplnanie iba spodneho trojuholnika
		e1.zaciatok = v[0];
		e1.koniec = v[2];
		e2.zaciatok = v[1];
		e2.koniec = v[2];

		e1.m = (double)(e1.koniec.y() - e1.zaciatok.y()) / (e1.koniec.x() - e1.zaciatok.x());
		e2.m = (double)(e2.koniec.y() - e2.zaciatok.y()) / (e2.koniec.x() - e2.zaciatok.x());

		e1.w = 1.0 / e1.m;
		e2.w = 1.0 / e2.m;

		int y = e1.zaciatok.y();
		int ymax = e1.koniec.y();
		double x1 = e1.zaciatok.x();
		double x2 = e2.zaciatok.x();
	}
	else if (v[1].y() == v[2].y())
	{
		e1.zaciatok = v[0];
		e1.koniec = v[1];
		e2.zaciatok = v[0];
		e2.koniec = v[2];

		e1.m = (double)(e1.koniec.y() - e1.zaciatok.y()) / (e1.koniec.x() - e1.zaciatok.x());
		e2.m = (double)(e2.koniec.y() - e2.zaciatok.y()) / (e2.koniec.x() - e2.zaciatok.x());

		e1.w = 1.0 / e1.m;
		e2.w = 1.0 / e2.m;

		int y = e1.zaciatok.y();
		int ymax = e1.koniec.y();
		double x1 = e1.zaciatok.x();
		double x2 = e2.zaciatok.x();
	}
	else
	{
		QPoint p;
		double m = (double)(v[2].y() - v[0].y()) / (double)(v[2].x() - v[0].x());
		p.setX((double)(v[1].y() - v[0].y()) / m + v[0].x());
		p.setY(v[1].y());

		if (v[1].x() < p.x())
		{
			fillTriangle(QVector<QPoint>{v[0], v[1], p}, color,z);
			fillTriangle(QVector<QPoint>{v[1],p,v[2]}, color,z);
		}
		else
		{
			fillTriangle(QVector<QPoint>{v[0], p, v[1]}, color,z);
			fillTriangle(QVector<QPoint>{p, v[1], v[2]}, color,z);
		}
	}

	int y = e1.zaciatok.y();
	int ymax = e1.koniec.y();
	double x1 = e1.zaciatok.x();
	double x2 = e2.zaciatok.x();

	while (y < ymax)
	{
		if (x1 != x2)
		{
			drawLine(QPoint(x1 + 0.5, y), QPoint(x2 + 0.5, y), color,z);
		}
		x1 += e1.w;
		x2 += e2.w;
		y++;
	}
}

void ViewerWidget::drawPolygon(QVector<QPoint>& v, QColor color, int z, int algType)
{
	//clear();
	for (int i = 0; i < v.size(); i++)
	{
		if (i == v.size() - 1)
		{
			drawLine(v[i], v[0], color, z, algType);
		}
		else
		{
			drawLine(v[i], v[i + 1], color, z, algType);
		}
	}
}

void ViewerWidget::vypln(QVector<QPoint>& v, QColor color, int z)
{
	if (v.size() > 3)
	{
		ScanLine(v, color,z);
	}
	else if (v.size() == 3)
	{
		fillTriangle(v, color,z);
	}
	else
	{
		return;
	}
}

void ViewerWidget::Bezier(QVector<QPoint>& v, QColor color, int z)
{
	QList<QList <QPoint>> P(v.size());
	//priprava 2D listu
	for (int i = 0; i < v.size(); i++)
	{
		P[i].resize(v.size() - i);
	}
	for (int i = 0; i < P[0].size(); i++)
	{
		P[0][i] = v[i];
	}

	double delta_t = 0.025;
	double t = delta_t;

	QPoint Q0 = v[0], Q1;
	while (t < 1)
	{
		for (int i = 1; i < v.size(); i++)
		{
			for (int j = 0; j < v.size() - i; j++)
			{
				P[i][j] = (1 - t) * P[i - 1][j] + t * P[i - 1][j + 1];
			}
		}
		Q1 = P[v.size() - 1][0];
		QVector <QPoint> f = { QPoint(Q0.x() + 0.5, Q0.y() + 0.5) , QPoint(Q1.x() + 0.5, Q1.y() + 0.5) };
		checkPolygon(f, color, 0, z);
		Q0 = Q1;
		t += delta_t;
	}

	QVector <QPoint> f = { Q0, v[v.size() - 1] };
	checkPolygon(f, color, 0,z);
	//drawLine(Q0, v[v.size() - 1], color,z);
}



void ViewerWidget::checkPolygon(QVector<QPoint>& v, QColor color, bool vyplnit, int z, int algType)
{
	//clear();
	bool vnutri;
	vnutri = isInside(v);	//zistim ci su vsetky body z mojho polygonu vnutri

	if (vnutri)	//ak su vsetky vnutri tak normalne prebehnem kreslenie
	{
		drawPolygon(v, color,z, algType);
		if (vyplnit)
		{
			vypln(v, color,z);
		}
	}
	else	//ak sa najde nejaky co nie je vnutri tak musim spravit orezanie na zaklade toho ci je to usecka alebo nie
	{
		QVector<QPoint> orezany;
		if (v.size() == 2)
		{
			CyrusBeck(v, orezany);
			if (!orezany.isEmpty())	//vykresli sa len ak je neprazdny. V pripade ze oba body su vonku tak vektor bude prazdny
			{
				//drawPolygon(orezany, color, z, algType);
				drawLine(orezany[0], orezany[1], color,z, algType);
			}
			//qDebug() << orezany;
		}
		else
		{
			
			orezany=Sutherland(v);
			if (!orezany.isEmpty())
			{
				drawPolygon(orezany, color, algType);
				if (vyplnit)
				{
					vypln(orezany, color,z);
				}
			}
		}
	}
	
	update();
}

void ViewerWidget::drawKruznica(QPoint stred, QPoint bod, QColor color, int z)
{
	int r = std::round(sqrt(pow(stred.x() - bod.x(), 2) + pow(stred.y() - bod.y(), 2)));
	int p = 1 - r;
	int x = 0, y = r;
	int dvaX = 3, dvaY = 2 * r - 2;
	
	while (x <= y)
	{
		QPoint bod = QPoint(x, y);
		for (int i = 0; i < 4; i++)
		{
			if (isInside((int)(stred.x() + bod.x() + 0.5), (int)(stred.y() + bod.y() + 0.5)))
			{
				setPixel((int)(stred.x() + bod.x() + 0.5), (int)(stred.y() + bod.y() + 0.5), color,z);
			}
			bod = QPoint(bod.y(), -bod.x());
		}
		bod = QPoint(y ,x);
		for (int i = 0; i < 4; i++)
		{
			if (isInside((int)(stred.x() + bod.x() + 0.5), (int)(stred.y() + bod.y() + 0.5)))
			{
				setPixel((int)(stred.x() + bod.x() + 0.5), (int)(stred.y() + bod.y() + 0.5), color,z);
			}
			bod = QPoint(bod.y(), -bod.x());
		}

		if (p > 0)
		{
			p = p + dvaX - dvaY;
			y--;
			dvaY = dvaY - 2;
			dvaX += 2;
		}
		else
		{
			p = p + dvaX;
			dvaX += 2;
		}
		x++;
	}


	update();
}


void ViewerWidget::clear()
{
	img->fill(Qt::white);
	resetBuffer();
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

void ViewerWidget::posun(QVector<QPoint>& v, int x, int y)
{
	//qDebug() << v;
	for (int i = 0; i < v.size(); ++i) {
		v[i].setX(v[i].x() + x);
		v[i].setY(v[i].y() + y);
	}
	//qDebug() << v;
}

void ViewerWidget::otoc(QVector<QPoint> &v, int stupne)
{
	QPoint stred = v[0];
	double rad = stupne * M_PI / 180.0;
	int x, y;

	for (int i = 0; i < v.size(); i++) {
		x = v[i].x() - stred.x();
		y= v[i].y() - stred.y();
		if (rad > 0)	//otacam v smere hod ruciciek
		{
			v[i].setX(x * qCos(rad) - y * qSin(rad) + stred.x());
			v[i].setY(x * qSin(rad) + y * qCos(rad) + stred.y());
		}
		else
		{
			v[i].setX(x * qCos(-rad) + y * qSin(-rad) + stred.x());
			v[i].setY(-x * qSin(-rad) + y * qCos(-rad) + stred.y());
		}
	}
}

void ViewerWidget::skaluj(QVector<QPoint>& v, double zvislo, double vodorovne)
{
	double error = 0.001;
	if (fabs(zvislo) > error && fabs(vodorovne) > error)
	{
		QPoint bod = v[0];	//toto je prvotny bod mojho polygonu, ten zostava na mieste
		for (int i = 0; i < v.size(); i++)
		{
			v[i].setX((v[i].x() - bod.x()) * vodorovne + bod.x());
			v[i].setY((v[i].y() - bod.y()) * zvislo + bod.y());
		}
	}
	//prvy bod povazujem ako keby za nulu, size najprv musim posunut moj skalovany bod na uroven mojho prveho bodu, zoskalovat hho a potom ho posunut naspat
}

void ViewerWidget::preklop(QVector <QPoint>& v)
{
	if (v.size() == 2)	//ak to bude usecka tak budem preklapat podla osi x niekde cca v strede usecky
	{
		//najprv potrebujem najst stred mojej usecky
		QPoint stred;
		stred.setX((v[0].x() + v[1].x()) / 2);
		stred.setY((v[0].y() + v[1].y()) / 2);

		int vzdialenost;
		//kedze preklapam cez os x, tak budem menit len y-ove suradnice, x-ove zostavaju rovnake. Potrebujem zistit ako daleko su od stredu a potom ich preklopit o raz tolko
		for (int i = 0; i < v.size(); i++)
		{
			vzdialenost = v[i].y() - stred.y();
			v[i].setY(stred.y() - vzdialenost);
		}


	}
	else	//ak to bude polygon budem preklapat podla prvej hrany, resp medzi bodmi v[0]a v[1]
	{
		QPoint os;
		int a, b, c, nove_x, nove_y;
		//rovno zapisujem ako normalu teda ako x davam deltaY a ako y davam -deltaX
		os.setX(v[1].y() - v[0].y());
		os.setY(-(v[1].x() - v[0].x()));

		a = os.x();
		b = os.y();
		c = -a * v[0].x() - b * v[0].y();

		for (int i = 0; i < v.size(); i++)
		{
			nove_x = v[i].x() - 2 * a * (a * v[i].x() + b * v[i].y() + c) / (a * a + b * b);
			nove_y = v[i].y() - 2 * b * (a * v[i].x() + b * v[i].y() + c) / (a * a + b * b);
			v[i].setX(nove_x);
			v[i].setY(nove_y);
		}
	}
}

void ViewerWidget::skos(QVector<QPoint>& v, double k)
{
	QPoint prvy = v[0];  // zapamatam si moj prvy bod, ten nechcem aby sa posuval nech mi nexestuje polygon horedole

	// skosim polygon
	for (int i = 1; i < v.size(); i++) {
		v[i].setX(v[i].x() + k * (v[i].y() - prvy.y()));	//skosenie robil vzhladom na prvy bod
	}
}