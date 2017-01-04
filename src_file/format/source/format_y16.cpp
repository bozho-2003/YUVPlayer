#include "format_y16.h"



#define YUV_FORMATER_ID    0x1
#define YUV_FORMATER_NAME  "Y16"

#define MODULE_VERSION    "0.0.1"
#define MODULE_AUTHOR     "netjunegg@gmail.com"
#define MODULE_COMMENT    "y16, 转换为y8"


void CFormater_Y16::_init(){
	m_buf = NULL;
	m_bufsz = 0;
	
	m_block_size = 0;
	
	m_frame_num = 0;
	m_index = -1;
	
	m_width = m_height = 0;
	m_field = FIELD_UNKNOWN;
	m_y_size = 0;

	m_bEmu = 0;
	m_outFormat = RAW_FORMAT_UNKNOWN;

	m_y16_endian_offset = 0;
}
CFormater_Y16::CFormater_Y16(){
	_init();
}
CFormater_Y16::~CFormater_Y16(){
	destroy();
}

// format name
const char * CFormater_Y16::getName(){
	return YUV_FORMATER_NAME;
}
// other information
int  CFormater_Y16::getModuleInfo(Module_Info *pInfo){
	sprintf(pInfo->author_name,MODULE_AUTHOR);
	sprintf(pInfo->comment,MODULE_COMMENT);
	sprintf(pInfo->version,MODULE_VERSION);
	return E_OK;
}

int  CFormater_Y16::simpleConfig(int val, unsigned int val2, const char *str){
	if(val == 0)
		m_y16_endian_offset = 0;
	else
		m_y16_endian_offset = 1;

	return 0;
}
int  CFormater_Y16::selfConfig(){


	return 0;
}

int  CFormater_Y16::queryRuntimeInfo(char *infoBuf, int bufsz){
	if(infoBuf == NULL || bufsz <= 0)
		return -1;

	sprintf(infoBuf,"endian模式: %s", m_y16_endian_offset ? "大端":"小端");

	return 1;
}

// 设置源数据格式, width, height, field
int  CFormater_Y16::setParams(YUV_Params *params){
	if(params == NULL)
		return E_BAD_ARG;
	if(params->width <= 0 || params->height <= 0 
		|| (params->width&1) || (params->height&1) )
		return E_UNSUPPORTTED;
	if(params->field != FIELD_ALL && params->field != FIELD_TOP_ONLY
		&& params->field != FIELD_BOTTOM_ONLY && params->field != FIELD_TOP_BOTTOM_SEQ)
		return E_UNSUPPORTTED;

	m_width = params->width;
	m_height = params->height;
	m_field = params->field;

	m_y_size = m_width*m_height;
	m_block_size = m_width*m_height*2;
	if(m_field == FIELD_TOP_BOTTOM_SEQ)
		m_frame_num = 2;
	else
		m_frame_num = 1;

	return E_OK;
}

int  CFormater_Y16::update(void *pErr){
	if(m_bufsz < m_block_size){
		if(m_buf)
			free(m_buf);
		m_bufsz = 0;
		m_buf = (unsigned char *)malloc(m_block_size);
		if(m_buf == NULL)
			return E_NO_MEMORY;
		m_bufsz = m_block_size;
	}
	
	return 1;
}

// 查询输出数据格式, format, width, height
int  CFormater_Y16::getOutInfo(RawImage_Info *out){
	if(out == NULL)
		return E_BAD_ARG;

	m_outFormat = RAW_FORMAT_GREY8;
	out->raw_format = m_outFormat;
	if(m_field == FIELD_ALL){
		out->height = m_height;
	}
	else{
		out->height = m_height/2;
	}
	out->width = m_width;
	return E_OK;
}

int  CFormater_Y16::getMinBlockSize(){
	return m_block_size;
}
int  CFormater_Y16::getMinBlockFrameNum(){
	return m_frame_num;
}

// 获取buffer, buf大小应为minBlockSize
int  CFormater_Y16::getBuffer(unsigned char **ppBuf){
	if(ppBuf == NULL)
		return E_BAD_ARG;
	*ppBuf = NULL;
	if(m_block_size <= 0 ||m_bufsz < m_block_size)
		return E_NO_INIT;
	
	*ppBuf = m_buf;
	return E_OK;
}
int  CFormater_Y16::putData(unsigned char *pBuf){
	if(pBuf != m_buf)
		return E_BAD_ARG;
	m_index = 0;
	return E_OK;
}

int  CFormater_Y16::getFrameIndex(){
	return m_index;
}
int  CFormater_Y16::setFrameIndex(int index){
	if(index < 0 || index > m_frame_num)
		return E_BAD_ARG;
	m_index = index;
	return E_OK;
}

// offset: < 0: 上一帧, ==0: 当前帧, >0: 下一帧
int  CFormater_Y16::getFrame(int offset, unsigned char *pBuf, int bufsz){
	int index = m_index;
	if(index < 0)
		return E_NO_INIT;
	if(offset > 0) index++;
	else if(offset < 0) index--;
	
	if(index < 0 || index > m_frame_num)
		return 0;
	// added!!!
	if(index == 0 && m_field == FIELD_BOTTOM_ONLY)
		index = 1;
	
	int n;
	if(m_field == FIELD_ALL){  // all field
		unsigned char *p1,*p2;
		int i,j;
		
		p1 = m_buf;
		p1 += m_y16_endian_offset;
		p2 = pBuf;
		i = m_y_size;
		while(i-- > 0){
			*p2 = *p1;
			p2++;
			p1 += 2;
		}
	}
	else if(index == 0){   // top field
		unsigned char *p1,*p2;
		int i,j;
		
		// copy Y
		p1 = m_buf;
		p2 = pBuf;
		for(i = 0;i < m_height/2;i++){
			for(j = 0;j < m_width;j++){
				*p2++ = *p1;
				p1 += 2;
			}
			p1 += m_width*2;
		}
	}
	else if(index == 1){
		unsigned char *p1,*p2;
		int i,j;
		
		// copy Y
		p1 = m_buf;
		p2 = pBuf;
		p1 += m_width*2;
		for(i = 0;i < m_height/2;i++){
			for(j = 0;j < m_width;j++){
				*p2++ = *p1;
				p1 += 2;
			}
			p1 += m_width*2;
		}
	}
	else
		return E_OOPS;
	
	return index;
}

// 格式转换: 
// 每次转换, 此函数可能会被调用两次
// 1. 若dst_buf为NULL, 用来让外部应用查询dst_buf的大小, 请在p_dst_bufsz中返回dst_buf的大小, 外部应用会根据此大小去申请dst_buf
// 2. 第二次调用dst_buf将为有效值, 此时可做转换, 并将转换后的有效数据字节数填入p_dst_bufsz中
// 3. 同时, 请检查src_info, 若不支持, 请返回负值
// 4. 此函数不要修改对象的其他成员, 不要影响到其他函数的行为即可, 尽量独立
int  CFormater_Y16::toYourFormat(RawImage_Info *src_info, const unsigned char *src_buf, int src_bufsz, unsigned char *dst_buf, int *p_dst_bufsz){
	if(src_info == NULL)
		return -1;
	
	if(src_info->raw_format != RAW_FORMAT_YUV444)
		return -2;
	
	int outsize = src_info->width*src_info->height*2;
	if(outsize <= 0)
		return -3;
	
	if(dst_buf == NULL){
		if(p_dst_bufsz == NULL)
			return -4;
		*p_dst_bufsz = outsize;
		return E_OK;
	}
	
	if(src_buf == NULL || dst_buf == NULL)
		return -5;
	
	int width = src_info->width;
	int height = src_info->height;
	int y_size = width*height;
	const unsigned char *p1;
	unsigned char *p2;
	int i,j;
	
	// Y
	p1 = src_buf;
	p2 = dst_buf;
	i = y_size;
	if(m_y16_endian_offset == 0){
		while(i-- > 0){
			*p2++ = *p1++;
			*p2++ = 0;
		}
	}
	else{
		while(i-- > 0){
			*p2++ = 0;
			*p2++ = *p1++;
		}
	}
	
	return 1;
}

int  CFormater_Y16::destroy(){
	if(m_buf)
		free(m_buf);
	_init();
	return 1;
}


int formater_y16_get_interface(void **ppIntf){
	if(ppIntf == NULL)
		return -1;
	*ppIntf = new CFormater_Y16;
	return (ppIntf != NULL);
}

