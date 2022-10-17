typedef struct{
	char event[512];
	char camera[512];
}MMD_CAMERA; 

/* プロトタイプ宣言 */
MMD_CAMERA *Getshare(void);
void Closeshare(MMD_CAMERA *);