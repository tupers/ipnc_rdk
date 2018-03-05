/*
 * In order to mount a nand partition as UBIFS, we have lots of steps to do.
 * So we create this application to implement it.
 * Author: Y.C.C. (yong-cheh_chen@appropho.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	INVALID_FS = -1,
	JFFS2,
	UBIFS
};

typedef struct _table_entry{
	int ret;
	char* str;
	int (*mount)(void);
}table_entry;

static int gFsType = INVALID_FS;
int gPartition;
char *pDest;
static char cmd[256];

int mount_jffs2(void)
{
	return 0;
}

static int GetUbiStart(void)
{
	int i;
	for(i = 0;i < 255;i++){
		sprintf(cmd, "test -e /dev/ubi%d", i);
		if(system(cmd))
			break;
	}
	return i;
}

int HasPatition(int idx)
{
	sprintf(cmd, "test -e /dev/ubi%d_0", idx);
	return !system(cmd);
}

#define UBI_RETRY_MAX	3

int mount_ubi(void)
{
	int start, ret = 0, retry_count = 0;
	/* Get the ubi start index */
	start = GetUbiStart();
	do{
		if(ret){
			/* Structure needs cleaning */
			sprintf(cmd, "ubidetach /dev/ubi_ctrl -d %d", start);
			system(cmd);
			usleep(500*1000);
			sprintf(cmd, "ubiformat /dev/mtd%d -s 512 -O 2048", gPartition);
			system(cmd);
			usleep(500*1000);
		}
		sprintf(cmd, "ubiattach /dev/ubi_ctrl -m %d -O 2048", gPartition);
		system(cmd);
		usleep(500*1000);
		if(!HasPatition(start)){
			/* try to make partition */
			sprintf(cmd, "ubimkvol /dev/ubi%d -N ubifs_volume -m", start);
			system(cmd);
			usleep(500*1000);
		}
		sprintf(cmd, "mount -t ubifs ubi%d:ubifs_volume %s", start, pDest);
		ret = system(cmd);
		usleep(500*1000);
	}while(ret && retry_count++ < UBI_RETRY_MAX);
	return ret;
}

table_entry fs_table[] =
{
	{JFFS2, "JFFS2", mount_jffs2},
	{UBIFS, "UBIFS", mount_ubi}
};

int GetFs(char *pFsName)
{
	int i, n = sizeof(fs_table)/ sizeof(fs_table[0]);
	for(i = 0;i < n;i++)
		if(!strcmp(pFsName, fs_table[i].str))
			return fs_table[i].ret;
	return INVALID_FS;
}

void show_usage(char *pName)
{
	printf("%s [JFFS2|UBIFS] [partition] [destination]\n", pName);
	printf("Ex: %s UBIFS /dev/mtd5 /mnt/nand\n", pName);
}

int do_mount(void)
{
	return fs_table[gFsType].mount();
}

int main(int argc, char *argv[])
{
	if(argc < 4){
		show_usage(argv[0]);
		exit(1);
	}
	gFsType = GetFs(argv[1]);
	pDest = argv[3];
	if(gFsType == INVALID_FS){
		printf("Filesystem [%s] is not support\n", argv[1]);
		exit(1);
	}
	/* assume argv[2] is /dev/mtdX format */
	if(sscanf(argv[2], "/dev/mtd%d", &gPartition) != 1){
		printf("unkown format for patition : %s\n", argv[2]);
		exit(1);
	}
	/* check of argv[3] exist */
	sprintf(cmd, "test -d %s", argv[3]);
	if(system(cmd)){
		printf("%s is not exist\n", argv[3]);
		exit(1);
	}
	return do_mount();
}
