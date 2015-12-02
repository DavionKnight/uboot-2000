#include <common.h>
#include <command.h>
#include <environment.h>
#include <search.h>
#include <errno.h>
#include <malloc.h>
#include <watchdog.h>
#include <serial.h>
#include <linux/stddef.h>
#include <asm/byteorder.h>
#include <asm/global_data.h>  //gd  defined here 
#include <config.h>
#include <linux/types.h>
#include <api_public.h>




static int env_print(char *name)
{
	char *res = NULL;
	size_t len;
	int ret;

	if (name) {		/* print a single name */
		ENTRY e, *ep;

		e.key = name;
		e.data = NULL;
		hsearch_r(e, FIND, &ep, &env_htab);
		if (ep == NULL)
			return 0;
		len = printf("%s=%s\n", ep->key, ep->data);
		return len;
	}

	//memcpy(gd->bd->device, ep->data, sizeof(gd->bd->device);
	/* print whole list */
	len = hexport_r(&env_htab, '\n', &res, 0, 0, NULL);

	if (len > 0) {
		puts(res);
		free(res);
		return len;
	}

	/* should never happen */
	return 0;
}

int do_printfac(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char v[] = "vendor";
	char d[] = "device";
	env_print(v);
	env_print(d);
	//printf("vendor = %s\n", "huahuan");
	return 0;
}

U_BOOT_CMD(
	printfac,	CONFIG_SYS_MAXARGS,	1,	do_printfac,
	"print vendor and device information",
	"\n"
	"	- print vendor and device information\n"
	"help command ...\n"
	"	- print vendor and device information"
);


