## アイテムID(カメラ用)
0 ~ 9 数字用に割当  
10 ~ 19 回復アイテムに割当  
20 ~ 29 攻撃増加アイテムに割当  
30 ~ 39 ダメージ軽減アイテム  
40 ~ 49 フラグ用アイテム  

## MMD用マーカーID
0: 読み取りなし  
1: アイテム登録  
2: アイテム使用  

## MMDに送信するメッセージ
画面上のマーカーでモードが変更されたとき  
MMD_CAMERA_GET|mode_changed  

アイテムが登録されたとき  
MMD_CAMERA_GET|item_registerd  

アイテム使用時(%dにはアイテムidが入る)  
MMD_CAMERA_GET|item_%d  
 
アイテムの残量なし  
MMD_CAMERA_GET|emptyItem 

## 動作説明
初期状態ではカメラでのマーカー認識を行いません  
PCの画面左上をキャプチャし続けそこにマーカーがあるかを調べます  
マーカーのidに応じてモードが切り替えられ、アイテム登録かアイテム使用のモードに切り替えます  

### アイテム登録モード
カメラに写ったマーカーが使用済みか確認  
使用済みではければIDに対応したアイテムの残量を+1  
MMDにメッセージ送信  

### アイテム使用モード  
カメラに写ったマーカーの対応するアイテムの残量を確認  
残量があれば-1しMMDにメッセージ送信  


#### BGM
default  
https://dova-s.jp/bgm/play17549.html  
boss  
https://dova-s.jp/bgm/play1788.html  
battle  
https://dova-s.jp/bgm/play9573.html  

#### SE
コイン  
https://dova-s.jp/se/play404.html  
