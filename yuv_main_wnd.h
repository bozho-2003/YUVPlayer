#ifndef YUVPLAYERMAINWIDGET_H
#define YUVPLAYERMAINWIDGET_H

#include <QWidget>
#include <Qstring>
#include <QMenu>
#include <QTimer>
#include "cyuvdoc.h"
#include "render.h"
#include "ui_yuv_main_wnd.h"
#include "formaterman.h"
#include "filterman.h"
#include "imagewnd.h"
#include "yuv_compare_wnd.h"
#include "filter_wnd.h"


#define		FLAG_NONE	 0x0
#define		FLAG_HIDE	 0x1

#define		STATE_NOVIDEO		0x0
#define		STATE_VIDEOSTART	0x1
#define		STATE_VIDEOMID		0x2
#define		STATE_VIDEOEND		0x3
#define		STATE_PLAYING		0x4
#define		STATE_PAUSE			0x5
#define		STATE_ONEFRAME		0x6

#define		IDM_REGIST				0x0100
#define     IDM_CENTER_WINDOW       0x0110


class YUVPlayerMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit YUVPlayerMainWidget(QWidget *parent = 0);
    ~YUVPlayerMainWidget();

private:
    QString m_fileName;
    int  m_bLoop;

    int  m_width;
    int  m_height;
    int  m_field_id;
    int  m_format_id;

    int  m_showYUV;
    int  m_fps;


    int  m_bGridOn;
    int  m_gridXNum;
    int  m_gridYNum;

    int  m_bCompare;
    int  m_playing;

    int  m_bReady;		// 至少有一帧数据可以播
    int m_udpateStage;  // update stage

    int  m_notifyId;	 // imageWnd

    QString m_lpCmdline;

    // class
    CYUVDoc m_yuvDoc;

    // msg
    QString m_buf;
    int  m_recent_size;
    int  m_favor_size;

    CFormaterItem **m_ppFormaters;
    IYUV_Formater *m_formater;
    CRender m_render;
    //CFormaterItem **m_ppFormaters;
    // Indicate all availabel filter
    CFilterItem **m_ppFilters;
    IImage_Filter **m_ppFilters_apply;

    CmpDlg *m_pCmpDlg;
    ImageWnd  *m_imageWnd;


    FilterWnd *m_pFilterDlg;

    //
    QMenu *m_pMenu;
    QMenu *m_pRecentMenu;


    //MenuList  *m_pRecentList;
    //MenuList  *m_pFavorList;

    QString   m_exePath;

    //
    int m_conf_file_flag;

    //
    QSize m_fullSize;
    QSize m_upSize;

    // cmp
    RawImage_Info m_formatLocal;

    QString m_gridColor;

    QTimer play_timer;
protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void unCheckedCompareBox();
    void unCheckedFilterBox();

private slots:
    void openfile();
    void onApply();
    void onPlay();
    void onFilterApply();
    void onPause();
    void onStop();
    void onPrevFrame();
    void onNextFrame();
    void onPlayTimer();


    void onSizeChanged(QString itemName);
    void onShowFlagChanged(QString itemName);
    void onShowFlagChanged(int idx);
    void onShowFlagChanged();
    void onCompare(int idx);
    void onFilter(int idx);


    void setBlockPos(int x, int y);

private:
    Ui::yuv_main_wnd *ui;
    int openFile_inner(QString fileName, int bUseFile, int bUpdateRencent);
    int closeFile();
    int setState(int state);
    int updateConf1(int level = 1);
    int updateConf2(int level = 1);
    int updateConf4(int level = 1);
    int updateConf3();
    void printMsg(QString msg);

    int getGridOnFlag();
    int getShowFlag();
    int getSize(int &x, int &y);
    int getFormatIdx();
    IYUV_Formater *getFormat();

    int isNeedUpdate2();
    void getGridType(int &gridXNum, int &gridYNum);

    int startPlay();
    int endPlay();
    int imageEnd(int back);
    int displayFrame();
    void autonextFrame_timer();

    void notifyView(int id = 0);

signals:
    void notifyShowYUVView(int id = 0);
};

#endif // YUVPLAYERMAINWIDGET_H
