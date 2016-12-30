#include "yuv_compare_wnd.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

CmpDlg::CmpDlg(QWidget *parent) :
    QWidget(parent)
  ,ui(new Ui::yuv_compare_wnd)
{
    ui->setupUi(this);


    setFixedWidth(size().width());
    setFixedHeight(size().height());

    ui->lineEdit_threshold->setValidator(new QIntValidator(0, 1000, this));
    ui->lineEdit_threshold->setPlaceholderText(QStringLiteral("width"));
    ui->lineEdit_threshold->setText("0");


    connect(ui->pushButton_file, SIGNAL(clicked()), this, SLOT(openfile()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(onStop()));
    connect(ui->pushButton_previous, SIGNAL(clicked()), this, SLOT(onPrevFrame()));
    connect(ui->pushButton_next, SIGNAL(clicked()), this, SLOT(onNextFrame()));

    connect(ui->comboBox_compareComponent, SIGNAL(currentTextChanged(QString)), this, SLOT(onCompareComponentChanged(QString)));
    connect(ui->comboBox_compareType, SIGNAL(currentTextChanged(QString)), this, SLOT(onCompareModeChanged(QString)));
    connect(ui->lineEdit_threshold, SIGNAL(textChanged(QString)), this, SLOT(onCompareModeChanged(QString)));
    connect(ui->horizontalSlider_framepos, SIGNAL(sliderReleased()), this, SLOT(onSeekFrame()));

    //connect(&play_timer, SIGNAL(timeout()), this, SLOT(OnNextFrame()));

    m_imageWnd = new ImageWnd();
    m_imageWnd->m_pRender = &m_imageRender;
    m_imageWnd->hide();
    connect(this, SIGNAL(notifyShowYUVView()), m_imageWnd, SLOT(showOneFrame()));
    connect(m_imageWnd, SIGNAL(mousePos(int,int)), this, SLOT(setBlockPos(int, int)));

    m_diffWnd = new ImageWnd();
    m_diffWnd->m_pRender = &m_diffRender;
    m_diffWnd->hide();
    connect(this, SIGNAL(notifyShowYUVView()), m_diffWnd, SLOT(showOneFrame()));
    connect(m_diffWnd, SIGNAL(mousePos(int,int)), this, SLOT(setBlockPos(int, int)));


    m_cmpMode = getCompareType();
    m_cmpComp = getCompareComponentFlag();
    m_threshold = getThreshold();
    //connect(ui->pushButton_play, SIGNAL(clicked()), this, SLOT(openfile()));
    m_ppFormaters = gbl_formater_man.getList();

    setState(STATE_NOVIDEO);

}

CmpDlg::~CmpDlg()
{
    //m_imageWnd->close();
    gbl_formater_man.destroyList(m_ppFormaters);
    delete m_imageWnd;
    delete m_diffWnd;
    delete ui;
}


void CmpDlg::closeEvent(QCloseEvent *event)
{
    // do what you need here
    // then call parent's procedure
    emit notifyMainUncheckedComporeCombox();
    m_imageWnd->close();
    m_diffWnd->close();
    QWidget::closeEvent(event);
}



void CmpDlg::setBlockPos(int x, int y)
{
    ui->label_yuv_info->clear();
    ui->label_yuv_info->setText(QString("(%1,%2)").arg(x).arg(y));
}


void CmpDlg::setSize(int w, int h)
{
    m_width = w;
    m_height = h;
    m_notifyId = 1;
}

void CmpDlg::setFormatFromIdx(int idx)
{
    IYUV_Formater *formater = m_ppFormaters[idx]->pItem;
    m_formater = formater;
}

void CmpDlg::printMsg(QString msg)
{
    if(!msg.isEmpty())
        QMessageBox::information(this, tr("Food"), msg);
}


int CmpDlg::updateConf1(int level)
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

    {
        // yuv format

        //val1 = pBox->GetItemData(index);
        //val2 = m_yuvDoc.getFormaterID();

        //if(val2 != val1){
        if(1){
            m_yuvDoc.setFormater(m_formater, val1);
        }
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

        RawImage_Info info;
        m_yuvDoc.getOutFormat(&info);


        // filters
        /*
        if(isChecked(IDC_CHECK_FILTER))
            m_render.setFilters(m_ppFilters2);
        else
            m_render.setFilters(NULL);
        */

        m_imageRender.setFilters(NULL);
        if(m_imageRender.setParams(&info) < 0){
            //err_msg = QString("Unsupport output format : id: %1(0x%2), %3x%4").arg(info.raw_format, info.raw_format, info.width,info.height);
            printMsg(err_msg);
            goto leave;
        }

        if(m_imageRender.update() < 0){
            err_msg = m_imageRender.getErrMsg();
            printMsg(m_imageRender.getErrMsg());
            goto leave;
        }

        m_imageRender.getRawInfo(&info);
        m_formatLocal = info;


        info = m_formatLocal;
        info.raw_format = RAW_FORMAT_GREY8;
        m_diffRender.setParams(&info);
        m_diffRender.update();
    }

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


void CmpDlg::setGridType(int gridXNum, int gridYNum)
{
    m_gridXNum = gridXNum;
    m_gridYNum = gridYNum;
}


void CmpDlg::setGridOnFlag(int flag)
{
    m_bGridOn = flag;
}

void CmpDlg::setShowFlag(int showYUV)
{
    m_showYUV = showYUV;
}

int CmpDlg::updateConf2(int level)
{
    int change;
    QString err_msg;
    QString buf, buf2;
    int pos;

    m_imageRender.setShowYUV(m_showYUV);

    m_bLoop = false;
    //m_bLoop = isChecked(IDC_CHECK_LOOP);

    int ret;
    m_gridColor = QString("Red");
    ret = m_imageWnd->enableGrid(m_bGridOn,m_gridXNum,m_gridYNum);
    ret = m_diffWnd->enableGrid(m_bGridOn,m_gridXNum,m_gridYNum);
    //if(ret < 0)
    //    goto leave;

    //if(m_bCompare == 1)
    //    updateCompare2();

    return 1;

leave:
    if( level == 0 && !err_msg.isEmpty()){
        printMsg(err_msg);
        ;
    }
    return -1;
}

bool CmpDlg::isReady()
{
    return m_bReady == 1;
}

int CmpDlg::updateFinal(){
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

    return 1;

leave:
    if(errMsg.isEmpty() == 0)
        printMsg(errMsg);
    return ret;
}


void CmpDlg::onShowFlagChanged(int idx)
{
    QString tmp;
    onShowFlagChanged(tmp);
}

void CmpDlg::onShowFlagChanged(QString itemName)
{
    if(updateConf2() > 0)
        displayFrame();
}


int CmpDlg::getCompareComponentFlag()
{
    int cmpComp;
    QString str = ui->comboBox_compareComponent->currentText();

    if(str == QString("All"))
        cmpComp = 0;
    else if(str == QString("Y"))
        cmpComp = 1;
    else if(str == QString("U"))
        cmpComp = 2;
    else if(str == QString("V"))
        cmpComp = 3;
    return cmpComp;
}

int CmpDlg::getThreshold()
{
    int val1;
    val1 = ui->lineEdit_threshold->text().toInt();
    return val1;
}

int CmpDlg::getCompareType()
{
    int cmpType;
    QString str = ui->comboBox_compareType->currentText();

    if(str == QString("Threshold"))
        cmpType = 0;
    else if(str == QString("Diff"))
        cmpType = 1;
    return cmpType;
}

void CmpDlg::onCompareModeChanged(QString itemName)
{
    m_cmpMode = getCompareType();
    if(m_cmpMode == 1)
    {
        //ui->lineEdit_threshold->setText("0");
        //ui->lineEdit_threshold->setReadOnly(true);
    }
    else
    {
        //ui->lineEdit_threshold->setText("0");
        //ui->lineEdit_threshold->setReadOnly(false);
    }
    m_threshold = getThreshold();
    updateDiff();
    updateDiffView();
}

void CmpDlg::onCompareComponentChanged(QString itemName)
{
    m_cmpComp = getCompareComponentFlag();
    updateDiffView();
}


void CmpDlg::applyNewConfigure()
{
    // TODO: Add your control notification handler code here
    if(m_playing == 1)		// is playing
        return;

    int ret;
    ret = updateConf1();
    if(ret >= 0)
        ret = updateConf2();

    if(ret > 0 && !m_fileName.isEmpty())
        ret = updateFinal();
}

void CmpDlg::onStop()
{
    // TODO: Add your control notification handler code here
    if(m_playing == 1)
        endPlay();

    m_playing = 0;
    if(!m_bReady)
        return;

    //if(m_pCmpDlg != NULL)	// m_bCompare ->
        //::PostMessage(m_pCmpDlg->m_hWnd,WM_COMPARE,CMP_STOP,0);
        ;

    if( m_yuvDoc.getFrameIndex() == 1 ){
        closeFile();
        m_diffWnd->hide();
        m_imageWnd->hide();
        m_bReady = 0;

        //if(m_pCmpDlg != NULL){	// m_bCompare ->	// added
        //    m_pCmpDlg->clear(2);
        //    m_bCompare = 0;
        //}

        return;
    }

    // 显示第一帧
    if(m_yuvDoc.seekFrame(0,SEEK_SET) >= 0){
        if(m_yuvDoc.getFrameIndex() == 1){
            setState(STATE_VIDEOSTART);
        }

        displayFrame();
    }
}


void CmpDlg::onPrevFrame()
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


int CmpDlg::updateDiffView()
{
    updateDiff();
    notifyView();
    return 1;
}

void CmpDlg::onNextFrame()
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

void CmpDlg::onSeekFrame()
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
      onNextFrame();
  }
  else{
      if(nNow == m_yuvDoc.getFrameNum())
          setState(STATE_VIDEOMID);

      n++;
      m_yuvDoc.seekFrame(n,SEEK_SET);
      onPrevFrame();
  }

leave:
  ;
}

int CmpDlg::startPlay()
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

    return 1;
}

int CmpDlg::endPlay(){
    m_playing = 0;
    play_timer.stop();
    return 1;
}

int CmpDlg::imageEnd(int back)
{
    m_playing = 0;
    setState(STATE_VIDEOEND);

    return 1;
}


void CmpDlg::autonextFrame_timer()
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


void CmpDlg::setOrgRender(CRender *pOrg)
{
    m_orgRender = pOrg;
}

int  CmpDlg::updateDiff(){
    if(m_bReady <= 0)
        return 0;

    QString buf;
    int n;
    const unsigned char *p1, *p2;
    unsigned char *p3;
    int len1, len2;
    int bufsz;

    //m_threshold = 0;

    p1 = m_imageRender.getRaw(&len1);
    p2 = m_orgRender->getRaw(&len2);

    m_diffRender.getBuffer(&p3, &bufsz);

    if(m_cmpComp == 0)
        n = m_diffRender.yuvDiff_bi_all(&m_imageRender, p1, p2, m_threshold, (m_cmpMode != 0));
    else
        n = m_diffRender.yuvDiff_bi_comp(&m_imageRender, p1, p2, m_threshold, (m_cmpMode != 0), m_cmpComp-1);

    m_diffRender.putData(p3);

    /*
    if( n > 0){
        RawImage_Info info;
        m_render2.getRawInfo(&info);
        double db = info.width*info.height;
        float f = n/db*100;
        buf.Format("差异像素数: %d 个(占%.2f%%)",n,f);
    }
    else{
        buf = "相同: ^_^";
    }
    m_imageWnd2.setTitle(buf);
    */

    /*
    ::PostMessage(m_imageWnd2.m_hWnd,WM_VIEW,m_notifyId2,0);
    if(m_notifyId2>0){
        m_notifyId2 = 0;
    }
    */

    return 1;
}


int CmpDlg::displayFrame()
{
    if(m_bReady == 0)
        return 0;


    int framePos,frameNum;

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
    ret = m_imageRender.getBuffer(&pBuf, &bufSz);
    if(ret < 0){
        m_buf = QString("OOPS: 0x1001, %1. ").arg(ret);
        if(!m_imageRender.getErrMsg().isEmpty())
            m_buf += m_imageRender.getErrMsg();
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
    ret = m_imageRender.putData(pBuf);
    if(ret < 0){
        m_buf = QString("OOPS: 0x1003, %d. ").arg(ret);
        if(!m_imageRender.getErrMsg().isEmpty())
            m_buf += m_imageRender.getErrMsg();
        printMsg(m_buf);
        return -2;
    }

    updateDiff();

    notifyView();

    /*
    if(framePos == frameNum && !m_bLoop){
        //imageEnd(0);
        return 0;
    }
    */

    return 1;
}

void CmpDlg::notifyView(int id){
    emit notifyShowYUVView();
    emit notifyShowYUVDiffView();

    if(m_notifyId>0){
        m_notifyId = 0;
    }
}

void CmpDlg::openfile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open YUV file."), ".", tr("YUV Files(*.yuv *.rgb)"));
    if(!fileName.isEmpty())
    {
        openFile_inner(fileName, 1, 0);
        //Open a new show widget
        m_imageWnd->setWindowTitle(fileName);
        m_imageWnd->show();
        m_diffWnd->setWindowTitle("diff");
        m_diffWnd->show();
    }
}

int CmpDlg::openFile_inner(QString fileName, int bUseFile, int bUpdateRencent)
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

    ret = updateFinal();
    if(ret < 0)
        goto leave;

    ret = updateConf2(0);
    if(ret < 0)
        goto leave;
    //m_playing = 2;
    return 1;

leave:
    return ret;
}

int  CmpDlg::closeFile(){
    setState(STATE_NOVIDEO);

    m_yuvDoc.destroy();

    return 1;
}

int  CmpDlg::setPlayingFlag(int flag)
{
    m_playing = flag;
    return 1;
}

int  CmpDlg::setState(int state)
{
    int enable;
    if(state == STATE_NOVIDEO){
        enable = true;
        ui->pushButton_file->setEnabled(enable);

        enable = false;
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);
        ui->lineEdit_framepos_info->setReadOnly(true);
    }
    else if(state == STATE_VIDEOSTART){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->lineEdit_framepos_info->setReadOnly(false);

        enable = false;
        ui->pushButton_previous->setEnabled(enable);
    }
    else if(state == STATE_VIDEOMID){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);

        enable = false;
        ui->lineEdit_framepos_info->setReadOnly(false);

    }
    else if(state == STATE_VIDEOEND){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);

        enable = false;
        ui->pushButton_next->setEnabled(enable);

        ui->lineEdit_framepos_info->setReadOnly(false);
    }
    else if(state == STATE_PLAYING){
        enable = true;
        ui->pushButton_stop->setEnabled(enable);

        enable = false;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);

        ui->lineEdit_framepos_info->setReadOnly(true);
    }
    else if(state == STATE_PAUSE){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);

        enable = false;
        ui->lineEdit_framepos_info->setReadOnly(false);

    }
    else if(state == STATE_ONEFRAME){
        enable = true;
        ui->pushButton_file->setEnabled(enable);
        ui->pushButton_stop->setEnabled(enable);

        enable = false;
        ui->pushButton_next->setEnabled(enable);
        ui->pushButton_previous->setEnabled(enable);

        ui->lineEdit_framepos_info->setReadOnly(false);
    }

    return 1;
}




