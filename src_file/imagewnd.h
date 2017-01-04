#ifndef IMAGEWND_H
#define IMAGEWND_H

#include <QObject>
#include <QWidget>

#include "yuv_value_wnd.h"
#include "cyuvdoc.h"
#include "Render.h"
#include "ui_imagewidget.h"

class CMyRect{
public:
    int left;
    int top;
    int width;
    int height;

    CMyRect(){
        left = top = width = height = 0;
    }
};


class ImageWnd : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWnd(QWidget *parent = 0);
    ~ImageWnd();
signals:
    void mousePos(int x, int y);

public slots:
    void show();
    void hide();

// Attributes
public:
    int m_rszFlag;

    CRender *m_pRender;

    int m_width;
    int m_height;
    BITMAPINFOHEADER *m_pBmpInfo;
    unsigned char *m_pBmpBits;

    int  m_bGridOn;
    int  m_gridXNum;
    int  m_gridYNum;

    unsigned int m_myLife1, m_myLife2;


    QWidget *m_hParent;

    ValWnd  *m_pValuesDlg;

    //PosWnd  *m_pPosWnd;
    QPoint  m_pt;

    // added
    QPoint  m_oldPt;

    // added on 2008/10/30
    int  m_topWnd;

    QString  m_title;


    // debug
    int  m_dbg;

    CMyRect m_rect1, m_rect2, m_rect3;
    int m_paintFlag;

    //CDlgValues2 *m_pValues2Dlg;

    QPoint m_lockPoint;
    int m_bGridFlag;

    QPoint move_point; //moved distance
    bool mouse_press; // press mouse left key

private slots:
    void showOneFrame();

// Operations
public:
    bool  Create(QWidget* pParentWnd, QString lpszWindowName);

    int enableGrid(int enable,int xNum = -1,int yNum = -1);

    //int getGridPos(CPoint &point,int bGrid);

    int setClientSize(int cx,int cy, int center = 0);
    void printMsg(QString msg);

    void  preDisplayImage(int rectOnly = 0);
    void  displayImage();
    void  adjustRect(int level = 0);

    void clear();
    void destroy();

    //int  setGridColor(COLORREF color);

    // added
    int  updateValues(QPoint point, int flag = 0);
    int  updateValues();
    int  updateValues2(QPoint pt, int flag = 0);

    void setTitle(QString title);

    int enableValues2(int enable);

    int updateLayerout(int cx, int cy);
    int transferRect(const CMyRect &rect1, const CMyRect &rect2, const CMyRect &rect3, CMyRect &rect4);

    int getGridPos(QPoint &point,int bGrid);
    void drawGrid();


    // Generated message map functions
protected:

    bool eventFilter(QObject *obj, QEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    //void paintEvent(QPaintEvent *);

private:
     Ui::ImageShowWidget *ui;
     QPoint m_currentPt;
};

#endif // IMAGEWND_H
