#include <QDebug>
#include <QPainter>
#include <QCloseEvent>
#include "imagewnd.h"
#include "ui_imagewidget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define  IDM_ONTOP              0x0300
#define  IDM_ORIGIN_SIZE        0x0400
#define  IDM_SHOW_TITLE         0x0500
#define  IDM_SWITCH_SHOWMODE    0x0600
#define  IDM_HELP               0x0700

#define IMAGE_MOVE_STEP_HORZ   20
#define IMAGE_MOVE_STEP_VERT   20


#define HELP_MSG  ""

/////////////////////////////////////////////////////////////////////////////
// ImageWnd

ImageWnd::ImageWnd(QWidget *parent) : QWidget(parent)
    ,ui(new Ui::ImageShowWidget)
{
    ui->setupUi(this);

    m_pValuesDlg = new ValWnd();
    m_pValuesDlg->hide();

    setMouseTracking(true);
    ui->label_yuv_show->setMouseTracking(true);

    m_pBmpInfo = NULL;
    m_pBmpBits = NULL;

    m_bGridOn = 0;
    //m_gridColor = RGB(255,255,255);
    m_gridXNum = 1;
    m_gridYNum = 1;

    //m_pValuesDlg = NULL;
    m_pRender = NULL;

    m_myLife1 = m_myLife2 = 0;

    //m_pPosWnd = NULL;

    m_rszFlag = 0;

    //m_hBitmap = NULL;
    //m_hMemDC = NULL;

    m_paintFlag = 0;

    //m_pValues2Dlg = NULL;

    m_bGridFlag = 0;
    mouse_press = false;

    m_oldPt.setX(0);
    m_oldPt.setY(0);

}

ImageWnd::~ImageWnd()
{
     delete ui;
    delete m_pValuesDlg;
#if 0
    if(m_pValuesDlg)
        delete m_pValuesDlg;
    if(m_pPosWnd){
        delete m_pPosWnd;
    }
    if(m_pValues2Dlg)
        delete m_pValues2Dlg;
    if(m_hMemDC)
        ::ReleaseDC(m_hWnd, m_hMemDC);
    if(m_hBitmap)
        ::DeleteObject(m_hBitmap);
#endif
}


void ImageWnd::closeEvent(QCloseEvent *event)
{
    // do what you need here
    // then call parent's procedure
    m_pValuesDlg->close();
    QWidget::closeEvent(event);
}

// ImageWnd message handlers
void ImageWnd::printMsg(QString msg){
    if(!msg.isEmpty())
        //AfxMessageBox(msg);
        ;
}

void ImageWnd::show()
{
    QWidget::show();
}

void ImageWnd::hide()
{
    m_pValuesDlg->hide();
    QWidget::hide();
}


void ImageWnd::drawGrid()//CDC *pDc,QRect &rect)
{
#if 0
    if(rect.IsRectEmpty())
        return;
    if(m_gridXNum > m_rect1.width || m_gridYNum > m_rect1.height)
        return;
    CPen pen;
    CPen *oldPen;
    LOGBRUSH logBrush;
    logBrush.lbStyle = BS_SOLID;
    logBrush.lbColor = m_gridColor;
    pen.CreatePen(PS_SOLID |PS_GEOMETRIC|PS_ENDCAP_ROUND, 1, &logBrush);
    oldPen = pDc->SelectObject(&pen);

    int x,y,cx,cy,step,extra;
    x = rect.left;
    y = rect.top;
    cx = rect.Width();
    cy = rect.Height();

    if(m_gridXNum > 0){
        y = rect.top;
        step = m_gridXNum;
        extra = 0;
        x += step;		// added on 2008/02/03
        while(x < cx){
            pDc->MoveTo(x,y);
            pDc->LineTo(x,y+cy);
            x += step;
            if(extra-- > 0)
                x += 1;
        }
    }
    if(m_gridYNum > 0){
        x = rect.left;
        step = m_gridYNum;
        extra = 0;
        y += step;
        while(y < cy){
            pDc->MoveTo(x,y);
            pDc->LineTo(x+cx,y);
            y += step;
            if(extra-- > 0)
                y += 1;
        }
    }

    pDc->SelectObject(oldPen);
    pen.DeleteObject();
#endif
}

bool ImageWnd::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease || e->type() == QEvent::MouseMove)
    {
        return true;
    }
    return QWidget::eventFilter(obj, e);
}


void ImageWnd::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint pt(event->pos().rx(), event->pos().ry());
    if(m_bGridOn == 0){
        //updateValues2(point);
        m_pValuesDlg->hide();
        ;
    }
    else
    {
        m_pValuesDlg->setWindowTitle(windowTitle());
        m_pValuesDlg->show();
        m_currentPt = pt;
        updateValues(pt);
    }

    QWidget::mouseDoubleClickEvent(event);
}

void ImageWnd::mousePressEvent( QMouseEvent * event )
{
    // Only move and resize using mouse left key
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }

    move_point = event->globalPos() - this->pos();
    QWidget::mousePressEvent(event);
}

void ImageWnd::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    QWidget::mouseReleaseEvent(event);
}



void ImageWnd::mouseMoveEvent(QMouseEvent *event)
{
    // TODO: Add your message handler code here and/or call default
    if (Qt ::LeftButton == event->button()){
        /*
            POINT pt;
            pt.x=point.x;pt.y=point.y;
            ClientToScreen(&pt);
            SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,(LPARAM)(pt.x|(pt.y>>16)));
            */        
    }
    else if(m_bGridFlag)
    {
        QPoint pt(event->pos().rx(), event->pos().ry());
        getGridPos(pt,1);

        emit mousePos(pt.rx(), pt.ry());
    }
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }

    QWidget::mouseMoveEvent(event);
}

int ImageWnd::enableGrid(int enable,int xNum,int yNum){
    if(enable == 0){
        m_bGridOn = 0;
        m_bGridFlag = 0;
    }
    else{
        if(m_pBmpInfo == NULL){
            printMsg("请先调整到原始大小, 或者切换为原始大小模式");
            return -2;
        }

        if(xNum > 0)
            m_gridXNum = xNum;
        if(yNum > 0)
            m_gridYNum = yNum;
        m_bGridOn = 1;
        m_bGridFlag = 1;

    }

    if(m_pBmpInfo && m_pBmpBits){
        preDisplayImage();
        //Invalidate(0);
    }
    return 1;
}

int ImageWnd::getGridPos(QPoint &point,int bGrid)
{
    int x,y;
    int step;
    int left;
    QRect rect;

    x = point.rx();
    y = point.ry();
    if(m_rszFlag == 1){
        x += m_rect3.left;
        y += m_rect3.top;
    }

    x /= m_gridXNum;
    y /= m_gridYNum;
    if(!bGrid){
        x *= m_gridXNum;
        y *= m_gridYNum;
    }

    point.setX(x);
    point.setY(y);

    return 1;
}

int ImageWnd::setClientSize(int cx,int cy, int center)
{

    setFixedWidth(cx);
    setFixedHeight(cy);
    ui->label_yuv_show->setGeometry(QRect(0,0,cx,cy));
    ui->label_yuv_show->setFixedWidth(cx);
    ui->label_yuv_show->setFixedHeight(cy);


    return 1;
}

//
// level > 0: adjust forced
void  ImageWnd::adjustRect(int level)
{
    if(m_rect1.width <= 0 || m_rect1.height <= 0 || level > 0){
        int cx = 0, cy = 0;
        if(m_pBmpInfo != NULL){
            cx = m_pBmpInfo->biWidth;
            cy = m_pBmpInfo->biHeight;
        }
        if(cx <= 0 || cy <= 0){
            return;
        }

        setClientSize(cx,cy);

    }
}


//
// rectOnly: only update size, not image
void  ImageWnd::preDisplayImage(int rectOnly)
{
    if(!rectOnly){
        m_pRender->getImageBmp(&m_pBmpInfo, &m_pBmpBits);
    }

    if(m_pBmpInfo == NULL)
        return;

    QImage rgbShow(m_pBmpBits, m_pBmpInfo->biWidth, m_pBmpInfo->biHeight, QImage::Format_RGB888);

    QPixmap pixmap = QPixmap::fromImage(rgbShow);

    QPainter painter(&pixmap);
    painter.setPen(Qt::red);

    if(m_bGridOn){

        int x,y,cx,cy,step,extra;
        x = 0;
        y = 0;
        cx = m_pBmpInfo->biWidth;
        cy = m_pBmpInfo->biHeight;

        if(m_gridXNum > 0){
            y = 0;
            step = m_gridXNum;
            extra = 0;
            x += step;		// added on 2008/02/03
            while(x < cx)
            {            
                painter.drawLine(QPointF(x, y), QPointF(x, x + cy));

                x += step;
                if(extra-- > 0)
                    x += 1;
            }
        }

        if(m_gridYNum > 0){
            x = 0;
            step = m_gridYNum;
            extra = 0;
            y += step;
            while(y < cy){
                painter.drawLine(QPointF(x, y), QPointF(x + cx, y));
                y += step;
                if(extra-- > 0)
                    y += 1;
            }
        }

    }


    ui->label_yuv_show->setPixmap(pixmap);
    //setPixmap(pixmap);


    if(m_width != m_pBmpInfo->biWidth || m_height != m_pBmpInfo->biHeight){
        m_width = m_pBmpInfo->biWidth;
        m_height = m_pBmpInfo->biHeight;

        adjustRect(1);
    }
}


int  ImageWnd::updateValues()
{
    updateValues(m_currentPt);
}

int  ImageWnd::updateValues(QPoint pt, int flag){


    unsigned char  buf[256];
    QString  buf2;
    QString  text;
    QString  grid_pos;
    QString  pixel_pos;
    int numPixels,nLine;
    int x,y;
    int n;
    int i;
    int nLineT = 0;

    if(m_pBmpInfo == NULL || m_bGridFlag == 0)
        return -2;
    if(m_gridXNum <= 0 || m_gridYNum <= 0)
        return -3;

    // pixel pos 1
    QPoint pt2 = pt;
    getGridPos(pt2,0);
    x = pt2.rx();
    y = pt2.ry();

    numPixels = m_gridXNum;
    nLine = m_gridYNum;
    if(m_pBmpInfo->biWidth - x < numPixels)
        numPixels = m_pBmpInfo->biWidth - x;
    if(m_pBmpInfo->biHeight - y < nLine)
        nLine = m_pBmpInfo->biHeight - y;
    if(numPixels <= 0 || nLine <= 0)
        return - 4;

    // grid pos 2
    getGridPos(pt2,1);

    grid_pos = QString("(%1,%2)").arg(pt2.rx()).arg(pt2.ry());
    pixel_pos = QString("(%1,%2)").arg(x).arg(y);

    int valueMode = m_pValuesDlg->m_valueMode;
    int yuvFlag = m_pValuesDlg->m_YUV;

    // Y component
    if(yuvFlag&1){
        for(i = 0;i < nLine;i++){
            int nBytes;
            const  unsigned char *p1;

            // return actual size
            nBytes = m_pRender->getPixels_Y(x,y+i,numPixels,(unsigned char*)buf,numPixels);
            if(nBytes <= 0)
                break;
            if(i == 0){
                buf2  = QString("==Y(%1x%2)==\r\n").arg(numPixels).arg(nLine);
                text += buf2;
                nLineT++;
            }

            p1 = buf;
            while(nBytes-- > 0){
                if(valueMode == VALUE_MODE_DEC){
                    buf2 = QString().sprintf("%.3d ",*(const unsigned char*)p1);
                }
                else{
                    buf2 = QString().sprintf("%.2X ",*(const unsigned char*)p1);
                }

                text += buf2;
                p1++;
            }
            text += "\r\n";
            nLineT++;
        }
    }

    // U component
    if(yuvFlag&2){
        for(i = 0;i < nLine;i++){
            int nBytes;
            const unsigned char *p1;

            nBytes = m_pRender->getPixels_U(x,y+i,numPixels,(unsigned char*)buf,numPixels);
            if(nBytes == 0)
                continue;
            else if(nBytes < 0)
                break;
            if(i == 0){
                buf2 = QString("==U(%1x%2)==\r\n").arg(numPixels).arg(nLine);
                text += buf2;
                nLineT++;
            }

            p1 = buf;
            while(nBytes-- > 0){
                if(valueMode == VALUE_MODE_DEC){
                    buf2 = QString("%1 ").arg(*(const unsigned char*)p1, 2, 10);
                }
                else{
                    buf2 = QString("%1 ").arg(*(const unsigned char*)p1, 2, 16);
                }

                text += buf2;
                p1++;
            }
            text += "\r\n";
            nLineT++;
        }
    }

    // V component
    if(yuvFlag&4){
        for(i = 0;i < nLine;i++){
            int nBytes;
            const unsigned char *p1;

            nBytes = m_pRender->getPixels_V(x,y+i,numPixels,(unsigned char*)buf,numPixels);
            if(nBytes == 0)
                continue;
            else if(nBytes < 0)
                break;
            if(i == 0){
                buf2 = QString("==V(%1x%2)==\r\n").arg(numPixels).arg(nLine);
                text += buf2;
                nLineT++;
            }

            p1 = buf;
            while(nBytes-- > 0){
                if(valueMode == VALUE_MODE_DEC){
                    buf2 = QString("%1 ").arg(*(const unsigned char*)p1, 2, 10);
                }
                else{
                    buf2 = QString("%1 ").arg(*(const unsigned char*)p1, 2, 16);
                }

                text += buf2;
                p1++;
            }
            text += "\r\n";
            nLineT++;
        }
    }

    m_pValuesDlg->addItem(grid_pos, pixel_pos, text);
    m_pValuesDlg->show();

    return 1;
}


void ImageWnd::showOneFrame()
{
    preDisplayImage();

    if(m_pValuesDlg->isVisible())
    {
        updateValues();
    }
}

void  ImageWnd::displayImage()
{

}
