#include "DxLib.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	//ディスプレイの幅、高さを決めておく（縮小する際に使う）
	#define DW 640
	#define DH 480
	#define kanji_size 1945

	#define mosaic_a 4
	#define mosaic_b 8000

	//元の画像
	int SrcHandle ;
	//完成後の画像(モノクロ)
	int DestHandle ;
	//一時保存している画像	（モザイク状態）
	int TmpHandle ;
	//幅、高さ
	int Width,Height;
	int W_kanji,H_kanji;
	//分割する比率を計算
	int W_Src,H_Src;
	//さまざまな計算過程で用いる変数
	int x;
	double y;
	double z;
	int i,j,n;
	//漢字データの格納場所
	int kanji[kanji_size];
	//漢字データにモザイク処理を施したものを格納する場所
	int mosaic_kanji[kanji_size];

	char ckanji[23];
	//↑（メモリの無駄遣い？）

	//漢字の色コードを格納する場所
	int iro_kanji[kanji_size];
		

	//プログラムの始まり

	//ウィンドウモードに切り替え
	ChangeWindowMode( TRUE ) ;

	// ＤＸライブラリの初期化
    if( DxLib_Init() < 0 ){
        return -1 ;
	}

	//漢字データの読み込み
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

	//漢字データの幅、高さを取得
	GetGraphSize(kanji[0],&W_kanji,&H_kanji);

	//大きい画像データの読み込み
    SrcHandle = LoadGraph("nani.bmp") ;

	//大きい画像データの幅、高さを取得
	GetGraphSize(SrcHandle,&Width,&Height);


	DrawFormatString( 600, 300 , GetColor( 55,255,55 ), "%d,%d",Width,Height) ;


	//適切な大きさに合わせるプログラム
	if((Width > DW)||(Height > DH)){

		y=Width/DW;
		z=Height/DH;
		if(y>z)x=(int)(y+0.9);
		else x=(int)(z+0.9);
		
		// 画像を縮小する
		GraphFilter( SrcHandle,  DX_GRAPH_FILTER_DOWN_SCALE, x ) ;
		DrawGraph(0,0,SrcHandle,FALSE);

		Width = Width / (x+1);
		Height = Height /(x+1);

	}


	//大きい画像と漢字データの幅、高さの比率を求める（今回は19）
	W_Src = Width / W_kanji;
	H_Src = Height / H_kanji;


	n = W_Src * H_Src;
	//一次元配列バージョン
	int * iro_Src = (int *)malloc( sizeof( double ) * n );


	//デバック用
//	DrawFormatString( 300, 300 , GetColor( 255,255,255 ), "%d",n) ;

    // モザイク漢字ようのグラフィックハンドルを作成
	for(i=0;i<kanji_size;i++){
		mosaic_kanji[i] = MakeScreen( W_kanji, H_kanji, FALSE ) ;
	}


	//モザイク処理した漢字データを配列に格納
	//その際に、中心の文字の色コードを読み取る
	for(i=0;i<kanji_size;i++){
		mosaic_kanji[i] = kanji[i];
		GraphFilter( mosaic_kanji[i],DX_GRAPH_FILTER_GAUSS,mosaic_a,mosaic_b);
		DrawGraph(0,0,mosaic_kanji[i],FALSE);
		iro_kanji[i] = GetPixel(W_kanji/2,H_kanji/2);
	}


    // 一時的に使用するグラフィックハンドルを作成
    TmpHandle = MakeScreen( 2560, 1440, FALSE ) ;
    // モノクロに使用するグラフィックハンドルを作成
    DestHandle = MakeScreen( 2560, 1440, FALSE ) ;

	//漢字データは白と黒しか表現できないので
	//モノトーンフィルターでモノクロの画像に変換
	GraphFilterBlt( SrcHandle,DestHandle,DX_GRAPH_FILTER_MONO,0,0) ;
	TmpHandle = DestHandle;

    // 処理を簡単にするため
	//モザイクフィルターでモザイクの画像に変換
	GraphFilter( TmpHandle,DX_GRAPH_FILTER_GAUSS,mosaic_a,mosaic_b);

	//モザイク処理された画像を表示
	DrawGraph(0,0,TmpHandle,FALSE);


	//大きい画像を漢字データの大きさごとに色データを取得する
	for(i=0;i<W_Src;i++){
		for(j=0;j<H_Src;j++){
			iro_Src[i*H_Src + j] = GetPixel((i*W_kanji)+(W_kanji/2),(j*H_kanji)+(H_kanji/2));
		}
	}

	//現状で、大きい画像データを漢字データの大きさごとに分割した際に得られる色コードの取得に成功したはず。
	//iro_Src←リサイズ色データ、iro_kanji←モザイク漢字の色コード



	int hikaku[kanji_size];

	n = W_Src * H_Src;
	//一次元配列バージョン
	int * iti = (int *)malloc( sizeof( double ) * n );

	for(i=0;i<n;i++){
		iti[i] = 0;
	}

	int tempmin;

	for(i=0;i<n;i++){
		for(j=0;j<kanji_size;j++){
			//元の画像の色コードと漢字の画像の色コードの引き算の絶対値を比較に代入
			hikaku[j] = abs(iro_Src[i] - iro_kanji[j]);
			tempmin = hikaku[j];
		}

		//比較配列の中で誤差の最小値はもっとも類似した色コードのはずなので、それを探す
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
			//モザイク処理された画像を表示
			DrawGraph(i*13,j*13,kanji[ iti [ i*H_Src + j] ],FALSE);
		}
	}

//	DrawFormatString(520, 100 , GetColor( 55,255,55 ), "%3d,%3d",Width,Height) ;
//	DrawFormatString(520, 200 , GetColor( 55,255,55 ), "%3d,%3d",W_Src,H_Src) ;
//	DrawFormatString(520, 300 , GetColor( 55,255,55 ), "%3d,%3d",W_kanji,H_kanji) ;
	

		free(iti);

    // キー入力待ち
    WaitKey() ;

    // ＤＸライブラリの後始末

	DxLib_End() ;

//0903一点比較完成版

    return 0 ;

	}