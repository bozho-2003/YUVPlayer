#ifndef YUV_VALUE_WND_H
#define YUV_VALUE_WND_H


#include <QObject>
#include <QWidget>
#include "ui_yuv_value_wnd.h"

#define  ITEM_NUM_MAX	3

#define  VALUE_MODE_NONE    0x0
#define  VALUE_MODE_HEX      0x1
#define  VALUE_MODE_DEC      0x2



class ValWnd : public QWidget
{
    Q_OBJECT
public:
    explicit ValWnd(QWidget *parent = 0);
    ~ValWnd();

    int m_adjustRect;

    //CFont  m_font;
    //COLORREF m_textColor;
    //COLORREF m_bgColor;
    //CBrush m_brush;

    int m_valueMode;
    int m_YUV;

    int enableWnd(int id,int enable);
    int enableCheck(int id,int check);
    int  isChecked(int id);

    int  getStringSize(QString str,int length,int &width,int &height);
    int  getStringLine(QString& str,int line, int &start,int &length);

    // interface
    int  clearItem();
    int  addItem(QString grid_pos, QString pixel_pos, QString text);

    //
    void destroy();

signals:


public slots:

// Attributes
public:

private slots:


// Operations
public:


protected:


private:
     Ui::ValWnd *ui;
};



#endif // YUV_VALUE_WND_H
