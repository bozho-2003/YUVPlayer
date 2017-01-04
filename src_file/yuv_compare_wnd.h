#ifndef YUVCOMPAREWND_H
#define YUVCOMPAREWND_H

#include <QWidget>
#include <Qstring>
#include <QMenu>
#include <QTimer>
#include "cyuvdoc.h"
#include "render.h"
#include "ui_yuv_compare_wnd.h"
#include "formaterman.h"
#include "imagewnd.h"


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


class CmpDlg : public QWidget
{
    Q_OBJECT

public:
    explicit CmpDlg(QWidget *parent = 0);
    ~CmpDlg();

    void setSize(int w, int h);
    void setFormatFromIdx(int idx);
    void setGridType(int gridXNum, int gridYNum);
    void setGridOnFlag(int flag);
    void setShowFlag(int showYUV);

    void setOrgRender(CRender *pOrg);

    bool isReady();
    int updateDiffView();
    void applyNewConfigure();

    int setState(int state);
    int setPlayingFlag(int flag);

private:
    QString m_fileName;
    int  m_bLoop;

    IYUV_Formater *m_formater;
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

    int  m_bReady;		// at least one frame is available to display.

    int  m_notifyId;	 // imageWnd
    int m_cmpComp;
    int m_cmpMode;

    QString m_lpCmdline;

    // class
    CYUVDoc m_yuvDoc;

    // msg
    QString m_buf;
    int  m_recent_size;
    int  m_favor_size;

    CFormaterItem **m_ppFormaters;

    //CFormaterItem **m_ppFormaters;
    //CFilterItem **m_ppFilters;
    IImage_Filter **m_ppFilters2;

    CRender *m_orgRender;

    CRender m_imageRender;
    ImageWnd  *m_imageWnd;

    CRender m_diffRender;
    ImageWnd  *m_diffWnd;
    int m_threshold;

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
    void openfile();
    void onStop();
    void onPrevFrame();
    void onNextFrame();
    void onSeekFrame();

    void onCompareComponentChanged(QString itemName);
    void onCompareModeChanged(QString itemName);
    void onShowFlagChanged(QString itemName);
    void onShowFlagChanged(int idx);
    int getCompareType();
    int getThreshold();

    void setBlockPos(int x, int y);

private:
    Ui::yuv_compare_wnd *ui;

    int openFile_inner(QString fileName, int bUseFile, int bUpdateRencent);
    int closeFile();

    int updateConf1(int level = 1);
    int updateConf2(int level = 1);
    int updateFinal();
    void printMsg(QString msg);

    int startPlay();
    int endPlay();
    int imageEnd(int back);
    int displayFrame();
    void autonextFrame_timer();

    void notifyView(int id = 0);
    int updateDiff();

    int getCompareComponentFlag();



signals:
    void notifyShowYUVView(int id = 0);
    void notifyShowYUVDiffView(int id = 0);
    void notifyMainUncheckedComporeCombox();

};

#endif // YUVCOMPAREWND_H
