#include "yuv_main_wnd.h"
#include "ui_yuv_main_wnd.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <iostream>

YUVPlayerMainWidget::YUVPlayerMainWidget(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::yuv_main_wnd)
{
    ui->setupUi(this);

    ui->lineEdit_width->setValidator(new QIntValidator(0, 10000, this));
    ui->lineEdit_height->setValidator(new QIntValidator(0, 10000, this));
    ui->lineEdit_width->setPlaceholderText(QStringLiteral("width"));
    ui->lineEdit_height->setPlaceholderText(QStringLiteral("height"));
    ui->lineEdit_width->setReadOnly(true);
    ui->lineEdit_height->setReadOnly(true);
    ui->checkBox_play_sync->setChecked(true);

    setFixedWidth(size().width());
    setFixedHeight(size().height());

    connect(ui->pushButton_file, SIGNAL(clicked()), this, SLOT(openfile()));
    connect(ui->pushButton_play, SIGNAL(clicked()), this, SLOT(onPlay()));
    connect(ui->pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(onStop()));
    connect(ui->pushButton_previous, SIGNAL(clicked()), this, SLOT(onPrevFrame()));
    connect(ui->pushButton_next, SIGNAL(clicked()), this, SLOT(onNextFrame()));
    connect(ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(onApply()));


    connect(ui->comboBox_size, SIGNAL(currentTextChanged(QString)), this, SLOT(onSizeChanged(QString)));
    connect(ui->comboBox_show, SIGNAL(currentTextChanged(QString)), this, SLOT(onShowFlagChanged(QString)));
    connect(ui->comboBox_grid, SIGNAL(currentTextChanged(QString)), this, SLOT(onShowFlagChanged(QString)));
    connect(ui->checkBox_grid, SIGNAL(stateChanged(int)), this, SLOT(onShowFlagChanged(int)));

    connect(ui->checkBox_compare, SIGNAL(stateChanged(int)), this, SLOT(onCompare(int)));

    //connect(&play_timer, SIGNAL(timeout()), this, SLOT(OnNextFrame()));
    connect(&this->play_timer, SIGNAL(timeout()), this, SLOT(onPlayTimer()));

    connect(ui->horizontalSlider_framepos, SIGNAL(sliderReleased()), this, SLOT(onSeekFrame()));

    m_udpateStage = 0;
    //ui->lineEdit_width->editingFinished();
    //ui->lineEdit_height->editingFinished();

    int w, h;
    getSize(w, h);

    ui->lineEdit_width->setText(QString("%1").arg(w));
    ui->lineEdit_height->setText(QString("%1").arg(h));


    m_imageWnd = new ImageWnd();
    m_imageWnd->m_pRender = &m_render;
    m_imageWnd->hide();

    connect(this, SIGNAL(notifyShowYUVView()), m_imageWnd, SLOT(showOneFrame()));
    connect(m_imageWnd, SIGNAL(mousePos(int,int)), this, SLOT(setBlockPos(int, int)));


    m_pCmpDlg = new CmpDlg();

    m_pCmpDlg->setOrgRender(&m_render);
    m_pCmpDlg->hide();
    connect(m_pCmpDlg, SIGNAL(notifyMainUncheckedComporeCombox()), this, SLOT(unCheckedCompareBox()));


    //pushButton_filter
    //connect(ui->pushButton_play, SIGNAL(clicked()), this, SLOT(openfile()));


    m_pFilterDlg = new FilterWnd();
    m_pFilterDlg->hide();
    connect(ui->checkBox_filter, SIGNAL(stateChanged(int)), this, SLOT(onFilter(int)));
    //connect(m_pFilterDlg, SIGNAL(notifyMainWndApply()), this, SLOT(onApply()));
    connect(m_pFilterDlg, SIGNAL(notifyMainWndApply()), this, SLOT(onFilterApply()));
    connect(m_pFilterDlg, SIGNAL(notifyMainUncheckedFilterCombox()), this, SLOT(unCheckedFilterBox()));

    //connect(m_pFilterDlg, SIGNAL(notifyMainWndApply()), this, SLOT(onShowFlagChanged()));

    setState(STATE_NOVIDEO);

    QString buf1;
    QString path;

    //GetModuleFileName(NULL, buf1.GetBuffer(MAX_PATH), MAX_PATH);
    //buf1.ReleaseBuffer();
    //int pos = buf1.ReverseFind('\\');
    //path = buf1.Left(pos+1);

    m_ppFormaters = gbl_formater_man.getList();
    if(m_ppFormaters)
    {
        for(int i = 0;;i++)
        {
            if(m_ppFormaters[i] == NULL)
                break;
            const char *name = m_ppFormaters[i]->pItem->getName();

            ui->comboBox_format->addItem(QString(name));

            //int index = addComBoxItem(IDC_COMBO_FORMAT, name, i);
        }
    }
    else
    {
        printMsg("Can't find any plugins, so, can't open yuv file.");
    }

    m_ppFilters = gbl_fielter_nam.getList();
    if(m_ppFilters)
    {
        m_pFilterDlg->setFilterModule(m_ppFilters);
    }
    else
    {
        printMsg("Can't find any plugins, so, can't open yuv file.");
    }
}

void YUVPlayerMainWidget::closeEvent(QCloseEvent *event)
{
    // do what you need here
    // then call parent's procedure
    m_imageWnd->close();
    m_pCmpDlg->close();
    m_pFilterDlg->close();
    QWidget::closeEvent(event);
}

YUVPlayerMainWidget::~YUVPlayerMainWidget()
{
    //m_imageWnd->close();
    gbl_formater_man.destroyList(m_ppFormaters);
    delete m_imageWnd;
    delete ui;
}


void YUVPlayerMainWidget::setBlockPos(int x, int y)
{
    ui->label_yuv_info->clear();
    ui->label_yuv_info->setText(QString("(%1,%2)").arg(x).arg(y));
}

void YUVPlayerMainWidget::printMsg(QString msg)
{
    if(!msg.isEmpty())
        QMessageBox::information(this, tr("Food"), msg);
}


int YUVPlayerMainWidget::updateConf1(int level)
{
    int val1,val2;
    QString err_msg;
    QString buf;
    QString size_str;
    //CButton *pBtn;
    int n;
    QString  buf2;
    long pos;
    int index;


    //if(m_playing > 0)
    //    return 0;

    m_bReady = 0;

    int res = getSize(val1, val2);
    if(res < 0)
    {
        err_msg = QString("Size is wrong.");
        printMsg(err_msg);
        goto leave;
    }

    if(	m_width != val1 || m_height != val2){
        m_width = val1;
        m_height = val2;
        m_notifyId = 1;
    }

    {
        // yuv格式
        m_yuvDoc.setFormater(getFormat(), val1);
        m_format_id = val1;

        m_field_id = FIELD_ALL;
        YUV_Params params;
        params.width = m_width;
        params.height = m_height;
        params.field = m_field_id;
        if(m_yuvDoc.setParams(&params) < 0){
            err_msg = QString("current yuv format didn't support this parameter.");
            printMsg(err_msg);
            goto leave;
        }
        m_field_id = index;
    }

    m_udpateStage = 1;
    return 1;

leave:
    if( level == 0){
        if(err_msg.isEmpty())
            //err_msg = m_yuvDoc.getErrMsg();
            ;
        if(!err_msg.isEmpty())
            printMsg(err_msg);
            QMessageBox::information(this, tr("Test"), err_msg);
            ;
    }

    return -1;
}


int YUVPlayerMainWidget::updateConf2(int level)
{
    int val1,val2;
    QString err_msg;
    QString buf;
    QString size_str;
    //CButton *pBtn;
    int n;
    QString  buf2;
    long pos;
    int index;

    if(m_fileName.isEmpty())
        return 0;

    if(m_udpateStage < 1)
    {
        return 1;
    }

    {
        RawImage_Info info;
        m_yuvDoc.getOutFormat(&info);

        // filters
        m_ppFilters_apply = m_pFilterDlg->getEnableFilterList();
        m_render.setFilters(m_ppFilters_apply);

        if(m_render.setParams(&info) < 0){
            //err_msg = QString("Unsupport output format : id: %1(0x%2), %3x%4").arg(info.raw_format, info.raw_format, info.width,info.height);
            printMsg(err_msg);
            goto leave;
        }

        if(m_render.update() < 0){
            err_msg = m_render.getErrMsg();
            printMsg(m_render.getErrMsg());
            goto leave;
        }

        m_render.getRawInfo(&info);
        m_formatLocal = info;

        /*
        if(m_pCmpDlg){
            if(m_bCompare && m_pCmpDlg->setCmpFormat(&info) <= 0){
                printMsg("两边格式不同, 将无法比较, 请调整格式");
            }
        }
        */

    }

     m_udpateStage = 2;
    return 1;

leave:
    if( level == 0){
        if(err_msg.isEmpty())
            //err_msg = m_yuvDoc.getErrMsg();
            ;
        if(!err_msg.isEmpty())
            printMsg(err_msg);
            QMessageBox::information(this, tr("Test"), err_msg);
            ;
    }

    return -1;
}

int YUVPlayerMainWidget::isNeedUpdate2()
{
    int showYUV_old = getShowFlag();
    int gridFlag_old = getGridOnFlag();
    int gridXNum, gridYNum;
    getGridType(gridXNum, gridYNum);


    return (showYUV_old != m_showYUV) || (m_bGridOn != gridFlag_old) || (m_gridXNum != gridXNum || m_gridYNum != gridYNum);
}

int YUVPlayerMainWidget::getFormatIdx()
{
    return ui->comboBox_format->currentIndex();
}

IYUV_Formater *YUVPlayerMainWidget::getFormat()
{
    int idx = getFormatIdx();
    IYUV_Formater *formater = m_ppFormaters[idx]->pItem;
    return formater;
}

void YUVPlayerMainWidget::getGridType(int &gridXNum, int &gridYNum)
{
    QString buf;
    int xn,yn;

    buf = ui->comboBox_grid->currentText();
    if(buf == QString("8x8"))
    {
        xn = 8;
        yn = 8;
    }
    else if(buf == QString("16x16"))
    {
        xn = 16;
        yn = 16;
    }
    else if(buf == QString("32x32"))
    {
        xn = 32;
        yn = 32;
    }

    gridXNum = xn;
    gridYNum = yn;
}


int YUVPlayerMainWidget::getGridOnFlag()
{
    return ui->checkBox_grid->isChecked();
}

int YUVPlayerMainWidget::getShowFlag()
{
    int showYUV;
    QString show_type = ui->comboBox_show->currentText();

    if(show_type == QString("YUV"))
        showYUV = YUV_SHOW_YUV;
    else if(show_type == QString("Y"))
        showYUV = YUV_SHOW_Y;
    else if(show_type == QString("U"))
        showYUV = YUV_SHOW_U;
    else if(show_type == QString("V"))
        showYUV = YUV_SHOW_V;
    return showYUV;
}

int YUVPlayerMainWidget::getSize(int &x, int &y)
{
    int val1, val2;
    QString size_str;
    QString err_msg;
    size_str = ui->comboBox_size->currentText();
    if(size_str == QString("1080p"))
    {
        val1 = 1920;
        val2 = 1080;
    }
    else if(size_str == QString("720p"))
    {
        val1 = 1280;
        val2 = 720;
    }
    else if(size_str == QString("CIF"))
    {
        val1 = 352;
        val2 = 288;
    }
    else
    {
        val1 = ui->lineEdit_width->text().toInt();
        val2 = ui->lineEdit_height->text().toInt();
        if(val1  < 0){
            err_msg = tr("wront width");
            goto leave;
        }
        if(val2  < 0){
            err_msg = tr("wront height");
              goto leave;
        }

        if(val1 <= 0 || val2 <= 0){
            err_msg = "error size";
            goto leave;
        }
        if(val1 & 0x1){
            err_msg = "odd width";
            goto leave;
        }
        if(val2 & 0x1){
            err_msg = "odd height";
            goto leave;
        }
    }

    x = val1;
    y = val2;

    return 1;
leave:
   if(!err_msg.isEmpty())
       printMsg(err_msg);
   return -1;
}

int YUVPlayerMainWidget::updateConf4(int level)
{
    int change;
    QString err_msg;
    QString buf, buf2;
    int pos;

    if(m_fileName.isEmpty())
        return 0;

    m_showYUV = getShowFlag();
    m_render.setShowYUV(m_showYUV);

    m_fps = ui->comboBox_framerate->currentText().toInt();

    m_bLoop = false;
    //m_bLoop = isChecked(IDC_CHECK_LOOP);
    m_bGridOn = getGridOnFlag();

    if(m_bGridOn)
    {
        int gridXNum, gridYNum;
        getGridType(gridXNum, gridYNum);

        if(m_gridXNum != gridXNum || m_gridYNum != gridYNum)
        {
            m_gridXNum = gridXNum;
            m_gridYNum = gridYNum;
        }
    }

    int ret;
    m_gridColor = QString("Red");
    //ret = m_imageWnd.setGridColor(m_gridColor);
    ret = m_imageWnd->enableGrid(m_bGridOn,m_gridXNum,m_gridYNum);

    //if(ret < 0)
    //    goto leave;

    //if(m_bCompare == 1)
    //    updateCompare2();

     m_udpateStage = 4;
    return 1;

leave:
    if( level == 0 && !err_msg.isEmpty()){
        printMsg(err_msg);
        ;
    }
    return -1;
}


int YUVPlayerMainWidget::updateConf3(){
    int ret;

    if(m_fileName.isEmpty())
        return 0;

    QString errMsg;

    m_yuvDoc.setFileName(m_fileName.toStdString().c_str());
    ret = m_yuvDoc.update();  	// -1: 失败,0:无变化,1:更新
    if( ret  < 0){
        setState(STATE_NOVIDEO);		// 无图像
        errMsg = m_yuvDoc.getErrMsg();
        goto leave;
    }
    else if(ret == 0){  // 无变化,或无数据
        return 0;
    }
    else{		// 有变化,文件已经重启,可显示
        int n;
        m_bReady = 1;
        n = displayFrame();
        if( n >= 0){
            if(n > 0)
                setState(STATE_VIDEOSTART);
            //m_imageWnd.setTitle(m_fileName);
            //m_imageWnd.ShowWindow(SW_SHOW);
        }
    }


    // added. 2008/03/21
    if(m_yuvDoc.getFrameNum() == 1)
        setState(STATE_ONEFRAME);

     m_udpateStage = 3;
    return 1;

leave:
    if(errMsg.isEmpty() == 0)
        printMsg(errMsg);
    return ret;
}

void YUVPlayerMainWidget::onPlay()
{
    // TODO: Add your control notification handler code here
    if(!m_bReady){
        return;
    }

    startPlay();
}

void YUVPlayerMainWidget::onPause()
{
    // TODO: Add your control notification handler code here
    if(m_playing == 1){
        m_playing = 2;
        setState(STATE_PAUSE);
        m_pCmpDlg->setPlayingFlag(m_playing);
        m_pCmpDlg->setState(STATE_PAUSE);
    }
}

void YUVPlayerMainWidget::onFilter(int idx)
{
    if (idx == false)
    {
        //m_pCmpDlg->onStop();
        //m_pCmpDlg->onStop();
        //m_pCmpDlg->onStop();
        m_pFilterDlg->hide();
    }
    else
    {
         //
         int w,h ;
         int gridXNum, gridYNum;
         getSize(w, h);
         getGridType(gridXNum, gridYNum);

         //m_pCmpDlg->setSize(w, h);
         //m_pCmpDlg->setFormatFromIdx(getFormatIdx());
         //m_pCmpDlg->setGridType(gridXNum, gridYNum);
         //m_pCmpDlg->setGridOnFlag(getGridOnFlag());
         //m_pCmpDlg->setShowFlag(getShowFlag());

         m_pFilterDlg->show();

    }
}



void YUVPlayerMainWidget::onCompare(int idx)
{
    if (idx == false)
    {
        m_pCmpDlg->onStop();
        m_pCmpDlg->onStop();
        m_pCmpDlg->onStop();
        m_pCmpDlg->hide();
    }
    else
    {
         //
         int w,h ;
         int gridXNum, gridYNum;
         getSize(w, h);
         getGridType(gridXNum, gridYNum);

         m_pCmpDlg->setSize(w, h);
         m_pCmpDlg->setFormatFromIdx(getFormatIdx());
         m_pCmpDlg->setGridType(gridXNum, gridYNum);
         m_pCmpDlg->setGridOnFlag(getGridOnFlag());
         m_pCmpDlg->setShowFlag(getShowFlag());

         m_pCmpDlg->show();

    }
}

void YUVPlayerMainWidget::onShowFlagChanged()
{
    QString tmp;
    onShowFlagChanged(tmp);
}

void YUVPlayerMainWidget::onShowFlagChanged(int idx)
{
    QString tmp;
    onShowFlagChanged(tmp);
}

void YUVPlayerMainWidget::onSizeChanged(QString itemName)
{
    QString size_str;
    size_str = ui->comboBox_size->currentText();
    if(size_str == QString("other"))
    {
        ui->lineEdit_width->clear();
        ui->lineEdit_width->setReadOnly(false);
        ui->lineEdit_height->clear();
        ui->lineEdit_height->setReadOnly(false);
    }
    else
    {
        int w, h;
        getSize(w, h);

        ui->lineEdit_width->setText(QString("%1").arg(w));
        ui->lineEdit_height->setText(QString("%1").arg(h));

        ui->lineEdit_width->setReadOnly(true);
        ui->lineEdit_height->setReadOnly(true);
    }
}

void YUVPlayerMainWidget::onShowFlagChanged(QString itemName)
{
    if(updateConf4() > 0)
        displayFrame();

    QString size_str;
    size_str = ui->comboBox_size->currentText();
    if(size_str == QString("other"))
    {
        ui->lineEdit_width->setReadOnly(false);
        ui->lineEdit_height->setReadOnly(false);
    }


    int w,h ;
    int gridXNum, gridYNum;
    getSize(w, h);
    getGridType(gridXNum, gridYNum);

    m_pCmpDlg->setSize(w, h);
    m_pCmpDlg->setFormatFromIdx(getFormatIdx());
    m_pCmpDlg->setGridType(gridXNum, gridYNum);
    m_pCmpDlg->setGridOnFlag(getGridOnFlag());
     m_pCmpDlg->setShowFlag(getShowFlag());
    m_pCmpDlg->onShowFlagChanged(0);
}

void YUVPlayerMainWidget::unCheckedCompareBox()
{
    ui->checkBox_compare->setChecked(false);
}

void YUVPlayerMainWidget::unCheckedFilterBox()
{
    ui->checkBox_filter->setChecked(false);
}

void YUVPlayerMainWidget::onStop()
{
    // TODO: Add your control notification handler code here
    if(m_playing == 1)
    {
        endPlay();
    }

    m_playing = 0;
    if(!m_bReady)
        return;

    //if(m_pCmpDlg != NULL)	// m_bCompare ->
        //::PostMessage(m_pCmpDlg->m_hWnd,WM_COMPARE,CMP_STOP,0);
        ;

    if( m_yuvDoc.getFrameIndex() == 1 )
    {
        m_pCmpDlg->onStop();
        closeFile();
        m_fileName = QString();
        m_imageWnd->hide();
        m_bReady = 0;
        m_udpateStage = 0;

        m_pCmpDlg->onStop();
        m_pCmpDlg->onStop();
        m_pCmpDlg->onStop();
        ui->checkBox_compare->setChecked(false);
        m_pCmpDlg->hide();
        return;
    }

    // 显示第一帧
    if(m_yuvDoc.seekFrame(0,SEEK_SET) >= 0){
        if(m_yuvDoc.getFrameIndex() == 1){
            setState(STATE_VIDEOSTART);
        }

        displayFrame();
        if(m_pCmpDlg->isReady())
        {
            m_pCmpDlg->updateDiffView();
        }
    }
}

void YUVPlayerMainWidget::seekPrevFrame()
{
    // TODO: Add your control notification handler code here
    int n;
    int num = m_yuvDoc.getFrameNum();

    if(m_playing == 1)
        return;

    n = m_yuvDoc.getFrameIndex();
    if(n <= 1)
        return;

    if(n == num && m_playing <= 0){
        setState(STATE_VIDEOMID);
    }

    n = m_yuvDoc.seekFrameBackward(1);
    if(n > 0)
        displayFrame();
    else{
        setState(STATE_VIDEOSTART);
    }

    //if(m_bCompare == 1)
    //    notifyCompare(CMP_PREV_FRAME);

    // added. 2008/03/21
    if(m_yuvDoc.getFrameIndex() == 1 && m_playing <= 0)
        setState(STATE_VIDEOSTART);
}

void YUVPlayerMainWidget::onPrevFrame()
{
    seekPrevFrame();
    if(m_pCmpDlg->isReady())
    {
        if(isPlayingSyncChecked())
        {
            m_pCmpDlg->onPrevFrame();
        }
        else
        {
            m_pCmpDlg->updateDiffView();
        }
    }
}

void YUVPlayerMainWidget::seekNextFrame()
{
    // TODO: Add your control notification handler code here
    int seek_flag = 1;
//	if(m_playing > 0){
        if(m_yuvDoc.getFrameIndex() == m_yuvDoc.getFrameNum()){
            if(!m_bLoop){
                imageEnd(0);
                return;
            }
            m_yuvDoc.restart();
            seek_flag = 0;
        }

//	}

    if(m_yuvDoc.getFrameIndex() == 1 && m_playing <= 0){
        setState(STATE_VIDEOMID);
    }

    if(m_yuvDoc.seekFrameForward(seek_flag) > 0){
        displayFrame();

        //if(m_bCompare == 1)
        //    notifyCompare(CMP_NEXT_FRAME);
    }

    // added. 此处不用考虑是否在播放中,前面已经检查过了
    if(m_yuvDoc.getFrameIndex() == m_yuvDoc.getFrameNum() && !m_bLoop)
        imageEnd(0);
}

bool YUVPlayerMainWidget::isPlayingSyncChecked()
{
    return ui->checkBox_play_sync->isChecked();
}

void YUVPlayerMainWidget::onNextFrame()
{
    seekNextFrame();
    if(m_pCmpDlg->isReady())
    {
        if(isPlayingSyncChecked())
        {
            m_pCmpDlg->onNextFrame();
        }
        else
        {
            m_pCmpDlg->updateDiffView();
        }
    }
}


int YUVPlayerMainWidget::startPlay()
{
    if(m_bReady == 0)
        return -1;
    if(m_fps <= 0)
    {
        printMsg("wrong framerate");
        return -2;
    }
    if(m_yuvDoc.getFrameNum() - m_yuvDoc.getFrameIndex() <= 1)
        return 0;

    if(m_playing <= 0){
        int n;
        n = 1000 / m_fps - 1;
        if(n <= 0)
            n = 39;
        play_timer.start(n);
    }
    else if(m_playing != 2){		// pause
        m_playing = 0;
        return -2;
    }

    m_playing = 1;

    setState(STATE_PLAYING);
    m_pCmpDlg->setPlayingFlag(m_playing);
    m_pCmpDlg->setState(STATE_PLAYING);

    return 1;
}

int YUVPlayerMainWidget::endPlay(){
    m_playing = 0;
    play_timer.stop();
    return 1;
}

int YUVPlayerMainWidget::imageEnd(int back)
{
    m_playing = 0;
    setState(STATE_VIDEOEND);

    return 1;
}


void YUVPlayerMainWidget::autonextFrame_timer()
{
    static int idx = 0;

    // TODO: Add your control notification handler code here
    int seek_flag = 1;
//	if(m_playing > 0){
        if(m_yuvDoc.getFrameIndex() == m_yuvDoc.getFrameNum()){
            if(!m_bLoop){
                imageEnd(0);
                return;
            }
            m_yuvDoc.restart();
            seek_flag = 0;
        }

//	}

    if(m_yuvDoc.getFrameIndex() == 1 && m_playing <= 0){
        setState(STATE_VIDEOMID);
    }

    if(m_yuvDoc.seekFrameForward(seek_flag) > 0){
        displayFrame();

        //if(m_bCompare == 1)
        //    notifyCompare(CMP_NEXT_FRAME);
    }

    // added. 此处不用考虑是否在播放中,前面已经检查过了
    if(m_yuvDoc.getFrameIndex() == m_yuvDoc.getFrameNum() && !m_bLoop)
        imageEnd(0);

}


void YUVPlayerMainWidget::onPlayTimer()
{   
    // TODO: Add your message handler code here and/or call default
    if(m_playing == 1)
    {
        autonextFrame_timer();
        if(m_pCmpDlg->isReady())
        {
            if(isPlayingSyncChecked())
            {
                m_pCmpDlg->onNextFrame();
            }
            else
            {
                m_pCmpDlg->updateDiffView();
            }
        }
    }
}

void YUVPlayerMainWidget::onApply()
{
    // TODO: Add your control notification handler code here
    if(m_playing == 1)		// is playing
        return;

    int ret;
    ret = updateConf1();

    if(ret >= 0)
        ret = updateConf2();

    if(ret > 0 && m_fileName.isEmpty() == 0)
        ret = updateConf3();

    if(ret >= 0)
        ret = updateConf4();

    int w,h ;
    int gridXNum, gridYNum;
    getSize(w, h);
    getGridType(gridXNum, gridYNum);
    m_pCmpDlg->setSize(w, h);
    m_pCmpDlg->setFormatFromIdx(getFormatIdx());
    //m_pCmpDlg->setGridType(gridXNum, gridYNum);
    //m_pCmpDlg->setGridOnFlag(getGridOnFlag());
    //m_pCmpDlg->setShowFlag(getShowFlag());
    m_pCmpDlg->applyNewConfigure();
    //m_pCmpDlg->updateDiffView();
}

void YUVPlayerMainWidget::onFilterApply()
{
    // TODO: Add your control notification handler code here
    if(m_playing == 1)		// is playing
        return;

    int ret;
    ret = updateConf2();

    if(ret >= 0)
        ret = updateConf4();

    if(ret > 0 && m_fileName.isEmpty() == 1)
        ret = updateConf3();

    displayFrame();

    int w,h ;
    int gridXNum, gridYNum;
    getSize(w, h);
    getGridType(gridXNum, gridYNum);
    m_pCmpDlg->setSize(w, h);
    m_pCmpDlg->setFormatFromIdx(getFormatIdx());
    //m_pCmpDlg->setGridType(gridXNum, gridYNum);
    //m_pCmpDlg->setGridOnFlag(getGridOnFlag());
    //m_pCmpDlg->setShowFlag(getShowFlag());
    //m_pCmpDlg->applyNewConfigure();
    m_pCmpDlg->updateDiffView();

}

void YUVPlayerMainWidget::onSeekFrame()
{
    if(m_playing == 1)
    {
        return;
    }
  int pos = ui->horizontalSlider_framepos->value();
  QString str = QString("%1").arg(pos);
  //lineEdit->setText(str);
  int n;
  int nNow;
  if(m_bReady == 0)
      goto leave;

  n = pos;
  nNow = m_yuvDoc.getFrameIndex();
  if(n == nNow)
      goto leave;

  if(n > nNow){
      if(nNow == 1)
          setState(STATE_VIDEOMID);

      n--;
      m_yuvDoc.seekFrame(n,SEEK_SET);
      seekNextFrame();
      if(m_pCmpDlg->isReady())
      {
          m_pCmpDlg->updateDiffView();
      }
  }
  else{
      if(nNow == m_yuvDoc.getFrameNum())
          setState(STATE_VIDEOMID);

      n++;
      m_yuvDoc.seekFrame(n,SEEK_SET);
      seekPrevFrame();
      if(m_pCmpDlg->isReady())
      {
          m_pCmpDlg->updateDiffView();
      }
  }

leave:
  ;
}

int YUVPlayerMainWidget::displayFrame()
{
    if(m_bReady == 0)
        return 0;


    int framePos,frameNum;

    // 帧数目信息
    framePos = m_yuvDoc.getFrameIndex();
    frameNum = m_yuvDoc.getFrameNum();
    if(framePos < 0 || frameNum <= 0)
        return -1;

    //qDebug() << "OnNextFrame :" << frameNum << " Pos : " << framePos;

    if((framePos == 2) || (framePos == 1))
    {
        ui->horizontalSlider_framepos->setRange(1, frameNum);
    }
    ui->horizontalSlider_framepos->setValue(framePos);
    m_buf = QString("%1-%2").arg(framePos).arg(frameNum);
    ui->lineEdit_framepos_info->setText(m_buf);

    unsigned char *pBuf;
    int bufSz;
    int ret;
    ret = m_render.getBuffer(&pBuf, &bufSz);
    if(ret < 0){
        m_buf = QString("OOPS: 0x1001, %1. ").arg(ret);
        if(!m_render.getErrMsg().isEmpty())
            m_buf += m_render.getErrMsg();
        printMsg(m_buf);
        return -2;
    }
    ret = m_yuvDoc.getFrame(pBuf, bufSz);
    if(ret < 0){
        m_buf = QString("OOPS: 0x1002, %1. ").arg(ret);
        if(!m_yuvDoc.getErrMsg().isEmpty())
            m_buf += m_yuvDoc.getErrMsg();
        printMsg(m_buf);
        return -2;
    }
    ret = m_render.putData(pBuf);
    if(ret < 0){
        m_buf = QString("OOPS: 0x1003, %d. ").arg(ret);
        if(!m_render.getErrMsg().isEmpty())
            m_buf += m_render.getErrMsg();
        printMsg(m_buf);
        return -2;
    }

    notifyView();

    return 1;
}

void YUVPlayerMainWidget::notifyView(int id){

    emit notifyShowYUVView();

    if(m_notifyId>0){
        m_notifyId = 0;
    }
}

void YUVPlayerMainWidget::openfile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open YUV file."), ".", tr("Data Files(*.yuv *.rgb)"));
    if(!fileName.isEmpty())
    {
        openFile_inner(fileName, 1, 0);
        //Open a new show widget
        m_imageWnd->setWindowTitle(fileName);
        m_imageWnd->show();
    }
}

int YUVPlayerMainWidget::openFile_inner(QString fileName, int bUseFile, int bUpdateRencent)
{
    if(bUseFile)
    {
        m_fileName = fileName;
    }
    else
    {
        QString path = QFileDialog::getOpenFileName(this, tr("Open YUV file."), ".", tr("YUV Files(*.yuv)"));
        m_fileName = path;
    }

    if(m_bReady){
        m_bReady = 0;
        closeFile();
    }

    int ret;

    ret = updateConf1(0);
    if(ret < 0)
        goto leave;

    ret = updateConf2(0);
    if(ret < 0)
        goto leave;

    ret = updateConf3();
    if(ret < 0)
        goto leave;

    ret = updateConf4(0);
    if(ret < 0)
        goto leave;

    //m_playing = 2;
    return 1;

leave:
    return ret;
}

int  YUVPlayerMainWidget::closeFile(){
    setState(STATE_NOVIDEO);

    //
    //m_imageWnd.clear();

    m_yuvDoc.destroy();

    //CSliderCtrl *pCS = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_PLAY);
    //pCS->SetRange(0,0,1);

    //SetDlgItemText(IDC_STATIC_FRAMENUM,"0/0");

    return 1;
}

int  YUVPlayerMainWidget::setState(int state)
{

    m_pFilterDlg->setState(state == STATE_PLAYING);
    int enable;
    if(state == STATE_NOVIDEO){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);

        enable = false;
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_pause->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->lineEdit_framepos_info->setReadOnly(true);

        ui->checkBox_compare->setEnabled(enable);
    }
    else if(state == STATE_VIDEOSTART){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);
        ui->lineEdit_framepos_info->setReadOnly(false);
        ui->checkBox_compare->setEnabled(enable);

        enable = false;
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_pause->setEnabled(enable);
    }
    else if(state == STATE_VIDEOMID){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);

        enable = false;
        ui->lineEdit_framepos_info->setReadOnly(false);

    }
    else if(state == STATE_VIDEOEND){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);

        enable = false;
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_pause->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);

        ui->lineEdit_framepos_info->setReadOnly(false);
    }
    else if(state == STATE_PLAYING){
        enable = true;
        ui->pushButton_pause->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);

        enable = false;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);

        ui->lineEdit_framepos_info->setReadOnly(true);
    }
    else if(state == STATE_PAUSE){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->checkBox_compare->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);

        enable = false;

        ui->pushButton_pause->setEnabled(enable);


        ui->lineEdit_framepos_info->setReadOnly(false);
    }
    else if(state == STATE_ONEFRAME){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_apply->setEnabled(enable);
        ui->checkBox_compare->setEnabled(enable);

        enable = false;
        ui->pushButton_play->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_pause->setEnabled(enable);

        ui->lineEdit_framepos_info->setReadOnly(false);
    }

    return 1;
}




