
#include "stdio.h"
#include "string.h"
#include "dso/bsp_dso.h"
#include "lv_examples/lv_dso/lv_dso.h"

lv_obj_t *ac_btn;
lv_obj_t *ac_label;
lv_style_t ac_style_btn;

lv_obj_t *ht_btn;
lv_obj_t *ht_label;
lv_style_t ht_style_btn;

lv_obj_t *run_btn;
lv_obj_t *run_label;
lv_style_t run_style_btn;

lv_obj_t *vol_btn;
lv_obj_t *vol_label;
lv_style_t vol_style_btn;

lv_obj_t *rms_btn;
lv_obj_t *rms_label;
lv_style_t rms_style_btn;

lv_obj_t *fre_btn;
lv_obj_t *fre_label;
lv_style_t fre_style_btn;

void AC_Style_Btn(lv_obj_t* src)
{
    lv_style_copy(&ac_style_btn,&lv_style_plain_color);
	ac_style_btn.body.main_color = LV_COLOR_MAKE(0x1E,0x9F,0xFF); //���ô�ɫ�ı���
	ac_style_btn.body.grad_color = ac_style_btn.body.main_color;
	ac_style_btn.body.opa = LV_OPA_COVER;           //���ñ���ɫ��ȫ��͸��
	ac_style_btn.body.radius = LV_RADIUS_CIRCLE;    //����Բ�ǰ�ť
	ac_style_btn.body.shadow.color = LV_COLOR_MAKE(0x1E,0x9F,0xFF);
	ac_style_btn.body.shadow.type = LV_SHADOW_FULL; //�����ı߶�����Ӱ
	ac_style_btn.body.shadow.width = 1;             //������Ӱ�Ŀ��
	ac_style_btn.text.color = LV_COLOR_BLACK;
	ac_style_btn.body.padding.left = 1;            //�������ڱ߾�
	ac_style_btn.body.padding.right = 1;           //�������ڱ߾�
    
    ac_btn = lv_btn_create(src, NULL);
    lv_obj_set_pos(ac_btn, 5, 2);
	lv_obj_set_height(ac_btn,30);
    lv_obj_set_width(ac_btn, 110);
	lv_btn_set_style(ac_btn,LV_BTN_STYLE_REL,&ac_style_btn);
    lv_btn_set_style(ac_btn,LV_BTN_STYLE_PR, &ac_style_btn);
	ac_label = lv_label_create(ac_btn, NULL);                 
	lv_label_set_text(ac_label,"2.0V  AC");
}

void HT_Style_Btn(lv_obj_t* src)
{
    lv_style_copy(&ht_style_btn,&lv_style_plain_color);
	ht_style_btn.body.main_color = LV_COLOR_YELLOW; //���ô�ɫ�ı���
	ht_style_btn.body.grad_color = ht_style_btn.body.main_color;
	ht_style_btn.body.opa = LV_OPA_COVER;           //���ñ���ɫ��ȫ��͸��
	ht_style_btn.body.radius = LV_RADIUS_CIRCLE;    //����Բ�ǰ�ť
	ht_style_btn.body.shadow.color = LV_COLOR_MAKE(0x1E,0x9F,0xFF);
	ht_style_btn.body.shadow.type = LV_SHADOW_FULL; //�����ı߶�����Ӱ
	ht_style_btn.body.shadow.width = 1;             //������Ӱ�Ŀ��
	ht_style_btn.text.color = LV_COLOR_BLACK;
	ht_style_btn.body.padding.left = 1;            //�������ڱ߾�
	ht_style_btn.body.padding.right = 1;           //�������ڱ߾�
    
    ht_btn = lv_btn_create(src, NULL);
    lv_obj_set_pos(ht_btn, 120, 2);
	lv_obj_set_height(ht_btn,30);
    lv_obj_set_width(ht_btn, 110);
	lv_btn_set_style(ht_btn,LV_BTN_STYLE_REL,&ht_style_btn);  
    lv_btn_set_style(ht_btn,LV_BTN_STATE_PR, &ht_style_btn); 
	ht_label = lv_label_create(ht_btn, NULL);                 
	lv_label_set_text(ht_label,"H: 0.2mS");
}

void Run_Style_Btn(lv_obj_t* src)
{
    lv_style_copy(&run_style_btn,&lv_style_plain_color);
	run_style_btn.body.main_color = LV_COLOR_MAKE(248,104,97);    //���ô�ɫ�ı���
	run_style_btn.body.grad_color = run_style_btn.body.main_color;
	run_style_btn.body.opa = LV_OPA_COVER;           //���ñ���ɫ��ȫ��͸��
	run_style_btn.body.radius = LV_RADIUS_CIRCLE;    //����Բ�ǰ�ť
	run_style_btn.body.shadow.color = LV_COLOR_MAKE(240,104,97);
	run_style_btn.body.shadow.type = LV_SHADOW_FULL; //�����ı߶�����Ӱ
	run_style_btn.body.shadow.width = 1;             //������Ӱ�Ŀ��
	run_style_btn.text.color = LV_COLOR_BLACK;
	run_style_btn.body.padding.left = 1;             //�������ڱ߾�
	run_style_btn.body.padding.right = 1;            //�������ڱ߾�
    
    run_btn = lv_btn_create(src, NULL);
    lv_obj_set_pos(run_btn, 390, 2);
	lv_obj_set_height(run_btn,30);
    lv_obj_set_width(run_btn, 80);
	lv_btn_set_style(run_btn,LV_BTN_STYLE_REL,&run_style_btn);  
    lv_btn_set_style(run_btn,LV_BTN_STYLE_PR, &run_style_btn);  
	run_label = lv_label_create(run_btn, NULL);                  
	lv_label_set_text(run_label,"RUN");
}

void Vol_Style_Btn(lv_obj_t* src)
{
    lv_style_copy(&vol_style_btn,&lv_style_plain_color);
	vol_style_btn.body.main_color = LV_COLOR_MAKE(90,248,251);    //���ô�ɫ�ı���
	vol_style_btn.body.grad_color = vol_style_btn.body.main_color;
	vol_style_btn.body.opa = LV_OPA_COVER;           //���ñ���ɫ��ȫ��͸��
	vol_style_btn.body.radius = LV_RADIUS_CIRCLE;    //����Բ�ǰ�ť
	vol_style_btn.body.shadow.color = LV_COLOR_MAKE(90,248,251);
	vol_style_btn.body.shadow.type = LV_SHADOW_FULL; //�����ı߶�����Ӱ
	vol_style_btn.body.shadow.width = 1;             //������Ӱ�Ŀ��
	vol_style_btn.text.color = LV_COLOR_BLACK;
	vol_style_btn.body.padding.left = 1;             //�������ڱ߾�
	vol_style_btn.body.padding.right = 1;            //�������ڱ߾�
    
    vol_btn = lv_btn_create(src, NULL);
    lv_obj_set_pos(vol_btn, 2, 285);
	lv_obj_set_height(vol_btn,30);
    lv_obj_set_width(vol_btn, 110);
	lv_btn_set_style(vol_btn,LV_BTN_STYLE_REL,&vol_style_btn);  
    lv_btn_set_style(vol_btn,LV_BTN_STATE_PR, &vol_style_btn);
	vol_label = lv_label_create(vol_btn, NULL);                  
	lv_label_set_text(vol_label,"Vpp: 10.0V");
}

void Rms_Style_Btn(lv_obj_t* src)
{
    lv_style_copy(&rms_style_btn,&lv_style_plain_color);
	rms_style_btn.body.main_color = LV_COLOR_WHITE;    //���ô�ɫ�ı���
	rms_style_btn.body.grad_color = rms_style_btn.body.main_color;
	rms_style_btn.body.opa = LV_OPA_COVER;           //���ñ���ɫ��ȫ��͸��
	rms_style_btn.body.radius = LV_RADIUS_CIRCLE;    //����Բ�ǰ�ť
	rms_style_btn.body.shadow.color = LV_COLOR_WHITE;
	rms_style_btn.body.shadow.type = LV_SHADOW_FULL; //�����ı߶�����Ӱ
	rms_style_btn.body.shadow.width = 1;             //������Ӱ�Ŀ��
	rms_style_btn.text.color = LV_COLOR_BLACK;
	rms_style_btn.body.padding.left = 1;             //�������ڱ߾�
	rms_style_btn.body.padding.right = 1;            //�������ڱ߾�
    
    rms_btn = lv_btn_create(src, NULL);
    lv_obj_set_pos(rms_btn, 120, 285);
	lv_obj_set_height(rms_btn, 30);
    lv_obj_set_width(rms_btn, 110);
	lv_btn_set_style(rms_btn,LV_BTN_STYLE_REL,&rms_style_btn); 
    lv_btn_set_style(rms_btn,LV_BTN_STYLE_PR, &rms_style_btn); 
	rms_label = lv_label_create(rms_btn, NULL);                  
	lv_label_set_text(rms_label,"Rms: 10.0V");
}

void Fre_Style_Btn(lv_obj_t* src)
{
    lv_style_copy(&fre_style_btn,&lv_style_plain_color);
	fre_style_btn.body.main_color = LV_COLOR_WHITE;    //���ô�ɫ�ı���
	fre_style_btn.body.grad_color = fre_style_btn.body.main_color;
	fre_style_btn.body.opa = LV_OPA_COVER;           //���ñ���ɫ��ȫ��͸��
	fre_style_btn.body.radius = LV_RADIUS_CIRCLE;    //����Բ�ǰ�ť
	fre_style_btn.body.shadow.color = LV_COLOR_WHITE;
	fre_style_btn.body.shadow.type = LV_SHADOW_FULL; //�����ı߶�����Ӱ
	fre_style_btn.body.shadow.width = 1;             //������Ӱ�Ŀ��
	fre_style_btn.text.color = LV_COLOR_BLACK;
	fre_style_btn.body.padding.left = 1;             //�������ڱ߾�
	fre_style_btn.body.padding.right = 1;            //�������ڱ߾�
    
    fre_btn = lv_btn_create(src, NULL);
    lv_obj_set_pos(fre_btn, 240, 285);
	lv_obj_set_height(fre_btn, 30);
    lv_obj_set_width(fre_btn, 120);
	lv_btn_set_style(fre_btn,LV_BTN_STYLE_REL,&fre_style_btn); 
    lv_btn_set_style(fre_btn,LV_BTN_STYLE_PR, &fre_style_btn);
	fre_label = lv_label_create(fre_btn, NULL);                  
	lv_label_set_text(fre_label,"Fre: 00.00Khz");
}

void task_cb(lv_task_t * task)
{
    char buf[20]={0};
    sprintf(buf, "Fre: %0.2f Khz", WaveParams.Freq/1000);
    lv_label_set_text(fre_label, buf);
}

void lv_dso_task(void)
{
	lv_obj_t* src = lv_scr_act();
    
    AC_Style_Btn(src);
    HT_Style_Btn(src);
    Run_Style_Btn(src);
    Vol_Style_Btn(src);
    Rms_Style_Btn(src);
    Fre_Style_Btn(src);
    
    lv_task_create(task_cb, 300, LV_TASK_PRIO_MID, NULL);
}











