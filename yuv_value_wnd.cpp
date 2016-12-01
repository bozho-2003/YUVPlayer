
#include <QDebug>
#include <QPainter>
#include <QCloseEvent>
#include "yuv_value_wnd.h"


ValWnd::ValWnd(QWidget *parent) : QWidget(parent)
    ,ui(new Ui::ValWnd)
{
    ui->setupUi(this);

    QFont font;
    font.setFamily("Courier New");

    ui->textEdit->setFont(font);

    m_adjustRect = 1;


    m_valueMode = VALUE_MODE_HEX;
    m_YUV = 7;



}
ValWnd::~ValWnd()
{
     delete ui;

}


/////////////////////////////////////////////////////////////////////////////
int  ValWnd::enableWnd(int id, int enable){
    return 1;
}
int  ValWnd::enableCheck(int id,int check){
}

int  ValWnd::isChecked(int id){
}


int  ValWnd::getStringSize(QString str,int length,int &width,int &height){
    return -1;
}

int  ValWnd::getStringLine(QString& str,int lineNO, int &start,int &length)
{
    if(str.isEmpty() || lineNO < 0)
        return -1;

    int lineStart,lineEnd;
    start = 0;
    length = 0;
    lineStart = 0;
    while(1)
    {
        lineEnd = str.toStdString().find("\r\n", lineStart);
        //lineEnd = str.Find("\r\n",lineStart);
        if(lineEnd < 0){
            if(lineNO == 0){
                //lineEnd = str.GetLength();
                lineEnd = str.length();
                break;
            }
            else
                return -2;
        }
        if(lineNO == 0)
            break;
        lineNO--;
        lineStart =  lineEnd + 1;
    }

    start = lineStart;
    length = lineEnd - lineStart;

    return 1;
}

int  ValWnd::clearItem(){
    return 1;
}

int  ValWnd::addItem(QString grid_pos, QString pixel_pos, QString text)
{
    int lineStart,lineLen;
    int width,height;

    if(text.isEmpty())
        return -1;

    //ui->textEdit->clear();
    QString show_info = QString("Grid: ") + grid_pos + QString(" Pixel: ") + pixel_pos + QString("\r\n") + text;
    ui->textEdit->setText(show_info);
    ui->textEdit->setReadOnly(true);
    return 1;
}
