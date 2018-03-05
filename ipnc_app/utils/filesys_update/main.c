#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdarg.h>
#include <mtd/mtd-user.h>

#define SUCCESS			0
#define FAIL				-1
#define ENVALID_PRM		-2
#define UNKOWN			-3
#define FILE_NAME_SIZE	80

#define MAX_OOB_SIZE	64
#define MAX_PAGE_SIZE	2048

/* seems the oobblock member has been replaced. */
#define oobblock writesize

#ifdef U_DEBUG
#define DBG(fmt, args...)		fprintf(stdout, fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

typedef struct _FileInfo{
	long nStartOffset;
	long nEndOffset;
	int fd;
	long nLength;
	char strFileName[FILE_NAME_SIZE + 1];
}FileInfo;


unsigned char oobbuf[MAX_OOB_SIZE];
unsigned char oobreadbuf[MAX_OOB_SIZE];
unsigned char writebuf[MAX_PAGE_SIZE];
int	blockalign = 1; /*default to using 16K block size */
int	forcejffs2 = 1;
int	forceyaffs = 0;
int	autoplace = 0;
int	forcelegacy = 1;
int	noecc = 0;
int	writeoob = 0;
int	pad = 1;

struct nand_oobinfo jffs2_oobinfo = {
	.useecc = MTD_NANDECC_PLACE,
	.eccbytes = 6,
	.eccpos = { 0, 1, 2, 3, 6, 7 }
};

struct nand_oobinfo yaffs_oobinfo = {
	.useecc = MTD_NANDECC_PLACE,
	.eccbytes = 6,
	.eccpos = { 8, 9, 10, 13, 14, 15}
};

static int gLastI = -1, gLastRead = 0;

//FILE* db;

/**
*@brief Skip package header
*@param[in] fd File fd.
*@param[in] pBuf Buffer to store temp date.
*@param[in] nBufSize Buffer size.
*@return Value of buffer index
*/
int SkipHeader(int fd, char *pBuf, int nBufSize){
	int i,count = 0;
	const char target[]="\r\n\r\n";
	if(gLastRead > 0){
		i = (gLastI > 0) ? gLastI : 0;
		do {
			for(; i < gLastRead;i++){
				if(count == strlen(target)){
					return i;
				} else if(pBuf[i] == target[count])
					count++;
				else
					count = 0;
			}
			i = 0;
		}while((gLastRead = read(fd, pBuf, nBufSize)) > 0);
	} else {
		while((gLastRead = read(fd, pBuf, nBufSize)) > 0){
			for(i = 0; i < gLastRead;i++){
				if(count == strlen(target)){
					return i;
				} else if(pBuf[i] == target[count])
					count++;
				else
					count = 0;
			}
		}
	}
	return -1;
}
/**
*@brief Create a file
*@param[in] pFileName File name.
*@param[in] fd File fd.
*@param[in] pBuf Buffer to store temp date.
*@param[in] nBufSize Buffer size.
*@param[out] pFile File information to new file.
*@retval ENVALID_PRM gLastRead is not set correctly.
*@retval FAIL File create fail.
*@retval SUCCESS File create success.
*/
int CreateFile(char *pFileName, int fd, char *pBuf, int nBufSize, FileInfo* pFile)
{
	const char target[]="\r\n-----------------------------";
	int i,count = 0;
	if(gLastRead <= 0)
		return ENVALID_PRM;
	i = (gLastI > 0) ? gLastI : 0;
	DBG("i = %d\n", i);
	strcpy(pFile->strFileName, pFileName);
	pFile->fd = fd;
	pFile->nStartOffset = lseek(fd, 0, SEEK_CUR) - gLastRead + i;
	do{
		DBG("gLastRead : %d\n", gLastRead);
		for(; i < gLastRead;i++){
			if(count == strlen(target)){
				gLastI = i;
				pFile->nEndOffset = lseek(fd, 0, SEEK_CUR) - gLastRead + i - count;
				pFile->nLength = pFile->nEndOffset - pFile->nStartOffset;
				DBG("File length = %ld\n", pFile->nLength);
				return SUCCESS;
			} else if(pBuf[i] == target[count]){
				count++;
			} else if(count > 0){
				count = 0;
				i--;
			} else {
//				putc(pBuf[i], fp);
			}
		}
		i = 0;
	}while((gLastRead = read(fd, pBuf, nBufSize)) > 0);
	gLastI = -1;
	return FAIL;
}
/**
*@brief Function that really write file to NAND
*@param[in] pFile File to write.
*@param[in] imglen Lengh to write.
*@param[in] offset Offset to block start.
*@param[in] meminfo Pointer to NAND info.
*@param[in] fd NAND device fd.
*@param[in] oobinfochanged Value to determind if OOB had modified outside.
*@param[in] old_oobinfo Old OOB info.
*@param[in] oob pointer to OOB .
*@retval SUCCESS Nand write complete.
*@retval FAIL Error occures.
*/
int WritingFile2Nand(FileInfo* pFile, int imglen, unsigned long offset,
					struct mtd_info_user* meminfo, int fd, int oobinfochanged,
					struct nand_oobinfo* old_oobinfo, struct mtd_oob_buf* oob)
{
	int ret, blockstart = -1, baderaseblock, readlen, cnt;
	loff_t offs;
	/* Get data from input and write to the device */
	while (imglen && (offset < meminfo->size)) {
		// new eraseblock , check for bad block(s)
		// Stay in the loop to be sure if the mtdoffset changes because
		// of a bad block, that the next block that will be written to
		// is also checked. Thus avoiding errors if the block(s) after the
		// skipped block(s) is also bad (number of blocks depending on
		// the blockalign
		while (blockstart != (offset & (~meminfo->erasesize + 1))) {
			blockstart = offset & (~meminfo->erasesize + 1);
			offs = blockstart;
		        baderaseblock = 0;
			DBG("Writing data to block %x\n", blockstart);

		        /* Check all the blocks in an erase block for bad blocks */
			do {
			   	if ((ret = ioctl(fd, MEMGETBADBLOCK, &offs)) < 0) {
					printf("ioctl(MEMGETBADBLOCK) error\r\n");
					return FAIL;
				}
				if (ret == 1) {
						baderaseblock = 1;
						DBG("Bad block at %x, %u block(s) from %x will be skipped\n", (int) offs, blockalign, blockstart);
					}

				if (baderaseblock) {
					offset = blockstart + meminfo->erasesize;
				}
			        offs +=  meminfo->erasesize / blockalign ;
			} while ( offs < blockstart + meminfo->erasesize );

		}

		readlen = meminfo->oobblock;
		if (pad && (imglen < readlen))
		{
			readlen = imglen;
			memset(writebuf + readlen, 0xff, meminfo->oobblock - readlen);
		}

		/* Read Page Data from input file */
		if ((cnt = read(pFile->fd, writebuf, readlen)) != readlen) {
			if (cnt == 0)	// EOF
				break;
			printf("File I/O error on input file\r\n");
			return FAIL;
		}

		if (writeoob) {
			/* Read OOB data from input file, exit on failure */
			if ((cnt = read(pFile->fd, oobreadbuf, meminfo->oobsize)) != meminfo->oobsize) {
				printf("File I/O error on input file");
				return FAIL;
			}
			if (!noecc) {
				int i, start, len;
				/*
				 *  We use autoplacement and have the oobinfo with the autoplacement
				 * information from the kernel available
				 *
				 * Modified to support out of order oobfree segments,
				 * such as the layout used by diskonchip.c
				 */
				if (!oobinfochanged && (old_oobinfo->useecc == MTD_NANDECC_AUTOPLACE)) {
					for (i = 0;old_oobinfo->oobfree[i][1]; i++) {
						/* Set the reserved bytes to 0xff */
						start = old_oobinfo->oobfree[i][0];
						len = old_oobinfo->oobfree[i][1];
						memcpy(oobbuf + start,
							oobreadbuf + start,
							len);
					}
				} else {
					/* Set at least the ecc byte positions to 0xff */
					start = old_oobinfo->eccbytes;
					len = meminfo->oobsize - start;
					memcpy(oobbuf + start,
						oobreadbuf + start,
						len);
				}
			}
			/* Write OOB data first, as ecc will be placed in there*/
			oob->start = offset;
			if (ioctl(fd, MEMWRITEOOB, oob) != 0) {
				printf("ioctl(MEMWRITEOOB) error\r\n");
				return FAIL;
			}
			imglen -= meminfo->oobsize;
		}

		/* Write out the Page data */
		if (pwrite(fd, writebuf, meminfo->oobblock, offset) != meminfo->oobblock) {
			printf("pwrite error\r\n");
			return FAIL;
		}
		imglen -= readlen;
		offset += meminfo->oobblock;
	}
	return SUCCESS;
}
/**
*@brief Write file to nand
*@param[in] pFile File to write.
*@param[in] pBlockName Block device name.
*@param[in] offset Offset to block start.
*@retval SUCCESS Nand write complete.
*@retval FAIL Error occures.
*/
int FlashNand(FileInfo* pFile, char *pBlockName, unsigned long offset)
{
	char cmd[100];
	int fd, imglen = 0, pagelen;
	struct mtd_info_user meminfo;
	struct mtd_oob_buf oob;
	int ret;
#if 0
	loff_t offs;
	int cnt, readlen, baderaseblock, blockstart = -1;
#endif
	int oobinfochanged = 0;
	struct nand_oobinfo old_oobinfo;
	ret = SUCCESS;
	sprintf(cmd, "flash_eraseall -j %s > /dev/null\n", pBlockName);
	if(system(cmd)){
		printf("Fail on erase block\r\n");
		ret = FAIL;
		goto EXIT_FLASHNAND;
	}
#if 0
	sprintf(cmd, "/usr/sbin/nandwrite -s %ld -f -p -j %s %s/%s > %s/%s\n", offset, pBlockName,
			TEMP_PATH, pFile->strFileName, TEMP_PATH, TEMP_FILE);
	DBG(cmd);
	if(system(cmd)){
		printf("Fail on update new firmware\r\n");
		sprintf(cmd, "rm -f %s/%s\n", TEMP_PATH, TEMP_FILE);
		system(cmd);
		return FAIL;
	}
#else
	/* mtdoffset = offset, forcelegacy = 1, pad = 1, forcejffs2 = 1,
	mtd_device = pBlockName, img = pFile */
	memset(oobbuf, 0xff, sizeof(oobbuf));
	/* Open the device */
	if ((fd = open(pBlockName, O_RDWR)) == -1) {
		printf("open flash error\r\n");
		ret = FAIL;
		goto EXIT_FLASHNAND;
	}
	/* Fill in MTD device capability structure */
	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		printf("MEMGETINFO error\r\n");
		close(fd);
		ret = FAIL;
		goto EXIT_FLASHNAND;
	}
        /* Set erasesize to specified number of blocks - to match jffs2 (virtual) block size */
        meminfo.erasesize *= blockalign;
	/* Make sure device page sizes are valid */
	if (!(meminfo.oobsize == 16 && meminfo.oobblock == 512) &&
	    !(meminfo.oobsize == 8 && meminfo.oobblock == 256) &&
	    !(meminfo.oobsize == 64 && meminfo.oobblock == 2048)) {
		printf("Unknown flash (not normal NAND)\r\n");
		close(fd);
		ret = FAIL;
		goto EXIT_FLASHNAND;
	}
	/* Read the current oob info */
	if (ioctl (fd, MEMGETOOBSEL, &old_oobinfo) != 0) {
		printf("MEMGETOOBSEL error\r\n");
		close (fd);
		ret = FAIL;
		goto EXIT_FLASHNAND;
	}
	/*
	 * force oob layout for jffs2 or yaffs ?
	 * Legacy support
	 */
	if (forcejffs2 || forceyaffs) {
//		struct nand_oobinfo *oobsel = forcejffs2 ? &jffs2_oobinfo : &yaffs_oobinfo;

		if (autoplace) {
			printf("Autoplacement is not possible for legacy -j/-y options\n");
			ret = FAIL;
			goto restoreoob;
		}
		if ((old_oobinfo.useecc == MTD_NANDECC_AUTOPLACE) && !forcelegacy) {
			printf("Use -f option to enforce legacy placement on autoplacement enabled mtd device\n");
			ret = FAIL;
			goto restoreoob;
		}
		if (meminfo.oobsize == 8) {
    			if (forceyaffs) {
				printf("YAFSS cannot operate on 256 Byte page size");
				ret = FAIL;
				goto restoreoob;
			}
			/* Adjust number of ecc bytes */
			jffs2_oobinfo.eccbytes = 3;
		}
#if 0
		if (ioctl (fd, MEMSETOOBSEL, oobsel) != 0) {
			printf("MEMSETOOBSEL error\r\n");
			ret = FAIL;
			goto restoreoob;
		}
#endif
	}
	oob.length = meminfo.oobsize;
	oob.ptr = noecc ? oobreadbuf : oobbuf;

	/* Set file to start */
	lseek(pFile->fd, pFile->nStartOffset, SEEK_SET);
	pagelen = meminfo.oobblock + ((writeoob == 1) ? meminfo.oobsize : 0);
	// Check, if file is pagealigned
	if ((!pad) && ((pFile->nLength % pagelen) != 0)) {
		printf("Input file is not page aligned\n");
		ret = FAIL;
		goto restoreoob;
	}
	// Check, if length fits into device
	if ( ((pFile->nLength / pagelen) * meminfo.oobblock) > (meminfo.size - offset)) {
		printf("Image %ld bytes, NAND page %d bytes, OOB area %u bytes, device size %u bytes\n",
				pFile->nLength, pagelen, meminfo.oobblock, meminfo.size);
		printf("Input file does not fit into device error\r\n");
		ret = FAIL;
		goto restoreoob;
	}
	/* Get data from input and write to the device */
#if 0
	while (imglen && (offset < meminfo.size)) {
		// new eraseblock , check for bad block(s)
		// Stay in the loop to be sure if the mtdoffset changes because
		// of a bad block, that the next block that will be written to
		// is also checked. Thus avoiding errors if the block(s) after the
		// skipped block(s) is also bad (number of blocks depending on
		// the blockalign
		while (blockstart != (offset & (~meminfo.erasesize + 1))) {
			blockstart = offset & (~meminfo.erasesize + 1);
			offs = blockstart;
		        baderaseblock = 0;
			DBG("Writing data to block %x\n", blockstart);

		        /* Check all the blocks in an erase block for bad blocks */
			do {
			   	if ((ret = ioctl(fd, MEMGETBADBLOCK, &offs)) < 0) {
					printf("ioctl(MEMGETBADBLOCK) error\r\n");
					ret = FAIL;
					goto restoreoob;
				}
				if (ret == 1) {
						baderaseblock = 1;
						DBG("Bad block at %x, %u block(s) from %x will be skipped\n", (int) offs, blockalign, blockstart);
					}

				if (baderaseblock) {
					offset = blockstart + meminfo.erasesize;
				}
			        offs +=  meminfo.erasesize / blockalign ;
			} while ( offs < blockstart + meminfo.erasesize );

		}

		readlen = meminfo.oobblock;
		if (pad && (imglen < readlen))
		{
			readlen = imglen;
			memset(writebuf + readlen, 0xff, meminfo.oobblock - readlen);
		}

		/* Read Page Data from input file */
		if ((cnt = read(pFile->fd, writebuf, readlen)) != readlen) {
			if (cnt == 0)	// EOF
				break;
			printf("File I/O error on input file\r\n");
			ret =FAIL;
			goto restoreoob;
		}

		if (writeoob) {
			/* Read OOB data from input file, exit on failure */
			if ((cnt = read(pFile->fd, oobreadbuf, meminfo.oobsize)) != meminfo.oobsize) {
				printf("File I/O error on input file");
				ret = FAIL;
				goto restoreoob;
			}
			if (!noecc) {
				int i, start, len;
				/*
				 *  We use autoplacement and have the oobinfo with the autoplacement
				 * information from the kernel available
				 *
				 * Modified to support out of order oobfree segments,
				 * such as the layout used by diskonchip.c
				 */
				if (!oobinfochanged && (old_oobinfo.useecc == MTD_NANDECC_AUTOPLACE)) {
					for (i = 0;old_oobinfo.oobfree[i][1]; i++) {
						/* Set the reserved bytes to 0xff */
						start = old_oobinfo.oobfree[i][0];
						len = old_oobinfo.oobfree[i][1];
						memcpy(oobbuf + start,
							oobreadbuf + start,
							len);
					}
				} else {
					/* Set at least the ecc byte positions to 0xff */
					start = old_oobinfo.eccbytes;
					len = meminfo.oobsize - start;
					memcpy(oobbuf + start,
						oobreadbuf + start,
						len);
				}
			}
			/* Write OOB data first, as ecc will be placed in there*/
			oob.start = offset;
			if (ioctl(fd, MEMWRITEOOB, &oob) != 0) {
				printf("ioctl(MEMWRITEOOB) error\r\n");
				ret = FAIL;
				goto restoreoob;
			}
			imglen -= meminfo.oobsize;
		}

		/* Write out the Page data */
		if (pwrite(fd, writebuf, meminfo.oobblock, offset) != meminfo.oobblock) {
			printf("pwrite error\r\n");
			ret = FAIL;
			goto restoreoob;
		}
		imglen -= readlen;
		offset += meminfo.oobblock;
	}
#else
	ret = WritingFile2Nand(pFile, pFile->nLength, offset, &meminfo, fd,
				oobinfochanged, &old_oobinfo, &oob);
#endif
restoreoob:
	if (oobinfochanged) {
		if (ioctl (fd, MEMSETOOBSEL, &old_oobinfo) != 0) {
			printf("MEMSETOOBSEL error\r\n");
			close (fd);
			ret = FAIL;
			goto EXIT_FLASHNAND;
		}
	}

	close(fd);

	if (imglen > 0) {
		printf("Data did not fit into device, due to bad blocks\n");
		ret = FAIL;
	}
#endif
EXIT_FLASHNAND:
//	if(ret == SUCCESS)
//		fprintf(db, "Done with success\n");
	return ret;
}
/**
*@brief Root file system update
*@param[in] pFile File information.
*@param[in] pBlockName File information.
*@retval SUCCESS Update success.
*@retval FAIL Update fail.
*@retval UNKOWN Unkown file format.
*/
int DoRootFileSysUpdate(FileInfo* pFile, char* pBlockName)
{
//	char cmd[80];
	/* We only check filename */
	DBG("Enter %s\r\n", __func__);
#if 0
	/* CRC check */
	sprintf(cmd, "gzip -t %s/%s\n", TEMP_PATH, pFileName);
	if(system(cmd)){
		DBG("Can't check\r\n");
		return FAIL;
	}
#endif
	return FlashNand(pFile, pBlockName, 0);
}

void ShowUsage(char* prog_name)
{
	printf("%s MTD_DEVICE INPUTFILE\n", prog_name);
}

int main(int argc, char *argv[])
{
	char *pFileName = NULL, *pBlockName = NULL;
	int fd;
	char buf[512];
	FileInfo tFile;
//	db = fopen("/tmp/nand/filesys_update.log", "wt");
	if(argc < 3 || argc > 3)
		ShowUsage(argv[0]);
	else{
		pBlockName = argv[1];
		pFileName = argv[2];
		fd = open(pFileName, O_RDWR);
		if(fd != -1){
			gLastI = SkipHeader(fd, buf, sizeof(buf));
			if(CreateFile(pFileName, fd, buf, sizeof(buf), &tFile)==SUCCESS){
				DoRootFileSysUpdate(&tFile, pBlockName);
			}else{
				fprintf(stderr, "Fail to create file\n");
//				fprintf(db, "Fail to create file\n");
			}
			close(fd);
		}else{
			sprintf(buf, "Can't open file %s :", pFileName);
//			fprintf(db, "Can't open file %s :", pFileName);
			perror(buf);
		}
	}
//	fclose(db);
	return 0;
}
