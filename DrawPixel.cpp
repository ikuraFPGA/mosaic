#include "DxLib.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	//�f�B�X�v���C�̕��A���������߂Ă����i�k������ۂɎg���j
	#define DW 640
	#define DH 480
	#define kanji_size 1945

	#define mosaic_a 4
	#define mosaic_b 8000

	//���̉摜
	int SrcHandle ;
	//������̉摜(���m�N��)
	int DestHandle ;
	//�ꎞ�ۑ����Ă���摜	�i���U�C�N��ԁj
	int TmpHandle ;
	//���A����
	int Width,Height;
	int W_kanji,H_kanji;
	//��������䗦���v�Z
	int W_Src,H_Src;
	//���܂��܂Ȍv�Z�ߒ��ŗp����ϐ�
	int x;
	double y;
	double z;
	int i,j,n;
	//�����f�[�^�̊i�[�ꏊ
	int kanji[kanji_size];
	//�����f�[�^�Ƀ��U�C�N�������{�������̂��i�[����ꏊ
	int mosaic_kanji[kanji_size];

	char ckanji[23];
	//���i�������̖��ʌ����H�j

	//�����̐F�R�[�h���i�[����ꏊ
	int iro_kanji[kanji_size];
		

	//�v���O�����̎n�܂�

	//�E�B���h�E���[�h�ɐ؂�ւ�
	ChangeWindowMode( TRUE ) ;

	// �c�w���C�u�����̏�����
    if( DxLib_Init() < 0 ){
        return -1 ;
	}

	//�����f�[�^�̓ǂݍ���
	for(i=0;i<kanji_size;i++){

			if(i<10){
				sprintf_s(ckanji,"M/ren00000%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
			else if(i<100){
				sprintf_s(ckanji,"M/ren0000%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
			else if(i<1000){
				sprintf_s(ckanji,"M/ren000%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
			else {
				sprintf_s(ckanji,"M/ren00%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
	}

	//�����f�[�^�̕��A�������擾
	GetGraphSize(kanji[0],&W_kanji,&H_kanji);

	//�傫���摜�f�[�^�̓ǂݍ���
    SrcHandle = LoadGraph("nani.bmp") ;

	//�傫���摜�f�[�^�̕��A�������擾
	GetGraphSize(SrcHandle,&Width,&Height);


	DrawFormatString( 600, 300 , GetColor( 55,255,55 ), "%d,%d",Width,Height) ;


	//�K�؂ȑ傫���ɍ��킹��v���O����
	if((Width > DW)||(Height > DH)){

		y=Width/DW;
		z=Height/DH;
		if(y>z)x=(int)(y+0.9);
		else x=(int)(z+0.9);
		
		// �摜���k������
		GraphFilter( SrcHandle,  DX_GRAPH_FILTER_DOWN_SCALE, x ) ;
		DrawGraph(0,0,SrcHandle,FALSE);

		Width = Width / (x+1);
		Height = Height /(x+1);

	}


	//�傫���摜�Ɗ����f�[�^�̕��A�����̔䗦�����߂�i�����19�j
	W_Src = Width / W_kanji;
	H_Src = Height / H_kanji;


	n = W_Src * H_Src;
	//�ꎟ���z��o�[�W����
	int * iro_Src = (int *)malloc( sizeof( double ) * n );


	//�f�o�b�N�p
//	DrawFormatString( 300, 300 , GetColor( 255,255,255 ), "%d",n) ;

    // ���U�C�N�����悤�̃O���t�B�b�N�n���h�����쐬
	for(i=0;i<kanji_size;i++){
		mosaic_kanji[i] = MakeScreen( W_kanji, H_kanji, FALSE ) ;
	}


	//���U�C�N�������������f�[�^��z��Ɋi�[
	//���̍ۂɁA���S�̕����̐F�R�[�h��ǂݎ��
	for(i=0;i<kanji_size;i++){
		mosaic_kanji[i] = kanji[i];
		GraphFilter( mosaic_kanji[i],DX_GRAPH_FILTER_GAUSS,mosaic_a,mosaic_b);
		DrawGraph(0,0,mosaic_kanji[i],FALSE);
		iro_kanji[i] = GetPixel(W_kanji/2,H_kanji/2);
	}


    // �ꎞ�I�Ɏg�p����O���t�B�b�N�n���h�����쐬
    TmpHandle = MakeScreen( 2560, 1440, FALSE ) ;
    // ���m�N���Ɏg�p����O���t�B�b�N�n���h�����쐬
    DestHandle = MakeScreen( 2560, 1440, FALSE ) ;

	//�����f�[�^�͔��ƍ������\���ł��Ȃ��̂�
	//���m�g�[���t�B���^�[�Ń��m�N���̉摜�ɕϊ�
	GraphFilterBlt( SrcHandle,DestHandle,DX_GRAPH_FILTER_MONO,0,0) ;
	TmpHandle = DestHandle;

    // �������ȒP�ɂ��邽��
	//���U�C�N�t�B���^�[�Ń��U�C�N�̉摜�ɕϊ�
	GraphFilter( TmpHandle,DX_GRAPH_FILTER_GAUSS,mosaic_a,mosaic_b);

	//���U�C�N�������ꂽ�摜��\��
	DrawGraph(0,0,TmpHandle,FALSE);


	//�傫���摜�������f�[�^�̑傫�����ƂɐF�f�[�^���擾����
	for(i=0;i<W_Src;i++){
		for(j=0;j<H_Src;j++){
			iro_Src[i*H_Src + j] = GetPixel((i*W_kanji)+(W_kanji/2),(j*H_kanji)+(H_kanji/2));
		}
	}

	//����ŁA�傫���摜�f�[�^�������f�[�^�̑傫�����Ƃɕ��������ۂɓ�����F�R�[�h�̎擾�ɐ��������͂��B
	//iro_Src�����T�C�Y�F�f�[�^�Airo_kanji�����U�C�N�����̐F�R�[�h



	int hikaku[kanji_size];

	n = W_Src * H_Src;
	//�ꎟ���z��o�[�W����
	int * iti = (int *)malloc( sizeof( double ) * n );

	for(i=0;i<n;i++){
		iti[i] = 0;
	}

	int tempmin;

	for(i=0;i<n;i++){
		for(j=0;j<kanji_size;j++){
			//���̉摜�̐F�R�[�h�Ɗ����̉摜�̐F�R�[�h�̈����Z�̐�Βl���r�ɑ��
			hikaku[j] = abs(iro_Src[i] - iro_kanji[j]);
			tempmin = hikaku[j];
		}

		//��r�z��̒��Ō덷�̍ŏ��l�͂����Ƃ��ގ������F�R�[�h�̂͂��Ȃ̂ŁA�����T��
		for(j=0;j<kanji_size ;j++){
			if(hikaku[j] < tempmin){
				tempmin = hikaku[j];
				iti[i] = j;
			}
		}
	}

	for(i=0;i<kanji_size;i++){

			if(i<10){
				sprintf_s(ckanji,"M/ren00000%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
			else if(i<100){
				sprintf_s(ckanji,"M/ren0000%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
			else if(i<1000){
				sprintf_s(ckanji,"M/ren000%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
			else {
				sprintf_s(ckanji,"M/ren00%d_R.jpg",i);
				kanji[i]=LoadGraph(ckanji);
			}
	}


	for(i=0;i<W_Src;i++){
		for(j=0;j<H_Src;j++){
			//���U�C�N�������ꂽ�摜��\��
			DrawGraph(i*13,j*13,kanji[ iti [ i*H_Src + j] ],FALSE);
		}
	}

//	DrawFormatString(520, 100 , GetColor( 55,255,55 ), "%3d,%3d",Width,Height) ;
//	DrawFormatString(520, 200 , GetColor( 55,255,55 ), "%3d,%3d",W_Src,H_Src) ;
//	DrawFormatString(520, 300 , GetColor( 55,255,55 ), "%3d,%3d",W_kanji,H_kanji) ;
	

		free(iti);

    // �L�[���͑҂�
    WaitKey() ;

    // �c�w���C�u�����̌�n��

	DxLib_End() ;

//0903��_��r������

    return 0 ;

	}