/*
 * Copyright 2009-2010 Freescale Semiconductor, Inc.
 *	Jun-jie Zhang <b18070@freescale.com>
 *	Mingkai Hu <Mingkai.hu@freescale.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <miiphy.h>
#include <phy.h>
#include <fsl_mdio.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/fsl_enet.h>

void tsec_local_mdio_write(struct tsec_mii_mng *phyregs, int port_addr,
		int dev_addr, int regnum, int value)
{
	int timeout = 1000000;

	out_be32(&phyregs->miimadd, (port_addr << 8) | (regnum & 0x1f));
	out_be32(&phyregs->miimcon, value);
	asm("sync");

	while ((in_be32(&phyregs->miimind) & MIIMIND_BUSY) && timeout--)
		;
}

int tsec_local_mdio_read(struct tsec_mii_mng *phyregs, int port_addr,
		int dev_addr, int regnum)
{
	int value;
	int timeout = 1000000;

	/* Put the address of the phy, and the register
	 * number into MIIMADD */
	out_be32(&phyregs->miimadd, (port_addr << 8) | (regnum & 0x1f));

	/* Clear the command register, and wait */
	out_be32(&phyregs->miimcom, 0);
	asm("sync");

	/* Initiate a read command, and wait */
	out_be32(&phyregs->miimcom, MIIMCOM_READ_CYCLE);
	asm("sync");

	/* Wait for the the indication that the read is done */
	while ((in_be32(&phyregs->miimind) & (MIIMIND_NOTVALID | MIIMIND_BUSY))
			&& timeout--)
		;

	/* Grab the value read from the PHY */
	value = in_be32(&phyregs->miimstat);

	return value;
}

static int fsl_pq_mdio_reset(struct mii_dev *bus)
{
	struct tsec_mii_mng *regs = bus->priv;

	/* Reset MII (due to new addresses) */
	out_be32(&regs->miimcfg, MIIMCFG_RESET_MGMT);

	out_be32(&regs->miimcfg, MIIMCFG_INIT_VALUE);

	while (in_be32(&regs->miimind) & MIIMIND_BUSY)
		;

	return 0;
}

int tsec_phy_read(struct mii_dev *bus, int addr, int dev_addr, int regnum)
{
	struct tsec_mii_mng *phyregs = bus->priv;

	return tsec_local_mdio_read(phyregs, addr, dev_addr, regnum);
}

int tsec_phy_write(struct mii_dev *bus, int addr, int dev_addr, int regnum,
			u16 value)
{
	struct tsec_mii_mng *phyregs = bus->priv;

	tsec_local_mdio_write(phyregs, addr, dev_addr, regnum, value);

	return 0;
}

#define PSEPHY_ACCESS_CTRL	16
#define PSEPHY_RDWR_CTRL	17
#define PSEPHY_ACCESS_REG1	24
#define PSEPHY_ACCESS_REG2	25
#define PSEPHY_ACCESS_REG3	26
#define PSEPHY_ACCESS_REG4	27

#define PSEDOPHY		30
#define PHY0			0
#define PHY1			1
#define PHY2			2
#define PHY3			3
#define PHY4			4

#define ACCESS_EN		1

#define STRINGLEN 100
#define OPER_RD 0x2
#define OPER_WR 0x1

int bcm53101_write(struct mii_dev *bus, unsigned char page, unsigned char addr, unsigned short *value)
{
        unsigned short mdio_val = 0;
        unsigned short ret_val = 0, s_count = 0xffff;
        int mii_id = 0;

        if(((page >= 0x10) && (page <= 0x14))||(page == 0x17))//get real port phy addr
        {
                mii_id = page - 0x10;
//		printk("mii_id=%d\n",mii_id);
                tsec_phy_write(bus, mii_id, 0, addr/2, value[0]);
        }
        else                            //get psedophy addr
        {
                mii_id = PSEDOPHY;
//		printk("mii_id=%d\n",mii_id);
		page &= 0xff;
		mdio_val |= (page << 8);
	        mdio_val |= ACCESS_EN;
	
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_ACCESS_CTRL,mdio_val);
	
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_ACCESS_REG1,value[0]);
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_ACCESS_REG2,value[1]);
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_ACCESS_REG3,value[2]);
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_ACCESS_REG4,value[3]);

		mdio_val = 0;
	        mdio_val |= addr << 8;
	        mdio_val |= OPER_WR;
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_RDWR_CTRL,mdio_val);

//	msleep(1);	
		do
		{
			mdio_val = (tsec_phy_read(bus, mii_id, 0, PSEPHY_RDWR_CTRL) &0xffff);
			if(!(mdio_val&0x11))
				break;
			s_count --;
	//		msleep(1);
		}while(s_count > 0);
	}
	return 0;
}

int bcm53101_read(struct mii_dev *bus, unsigned char page, unsigned char addr, unsigned short *value)
{
        unsigned short mdio_val = 0;
        unsigned short setval, ret_val = 0, s_count = 0xffff;
	int mii_id = 0;	

//printf("page=0x%x,addr=0x%x\n",page,addr);

	if(((page >= 0x10) && (page <= 0x14))||(page == 0x17))//get real port phy addr
	{
		mii_id = page - 0x10;
//		printk("mii_id=%d\n",mii_id);
		mdio_val = tsec_phy_read(bus, mii_id, 0, addr/2)&0xffff;
		value[3] = 0; 
		value[2] = 0; 
		value[1] = 0; 
		value[0] = mdio_val; 
	}
	else				//get psedophy addr
	{
		mii_id = PSEDOPHY;
//		printk("mii_id=%d\n",mii_id);
	        page &= 0xff;
		setval = 0;
	        setval |= (page << 8);
	        setval |= ACCESS_EN;
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_ACCESS_CTRL,setval);
		
	        setval = 0;
	        setval |= addr << 8;
	        setval |= OPER_RD;
	        tsec_phy_write(bus, mii_id, 0, PSEPHY_RDWR_CTRL,setval);
	
	        do
	        {
	                setval = tsec_phy_read(bus, mii_id, 0, PSEPHY_RDWR_CTRL)&0xffff;
	                if((!(setval&0x11)))
	                        break;
			s_count --;
	//		msleep(1);
	        }while(s_count > 0);
	        setval = tsec_phy_read(bus, mii_id, 0, PSEPHY_ACCESS_REG1);
		value[0] = setval;
	        setval = tsec_phy_read(bus, mii_id, 0, PSEPHY_ACCESS_REG2);
		value[1] = setval;
	        setval = tsec_phy_read(bus, mii_id, 0, PSEPHY_ACCESS_REG3);
		value[2] = setval;
	        setval = tsec_phy_read(bus, mii_id, 0, PSEPHY_ACCESS_REG4);
		value[3] = setval;
	}

	return 0;
#if 0
//        printk("24: 0x%x\n",mdio_val);
	        ret_val = fsl_pq_local_mdio_read(preg,30,25);
	        printk("25: 0x%x\n",ret_val);
	        ret_val = fsl_pq_local_mdio_read(preg,30,26);
	        printk("26: 0x%x\n",ret_val);
	        ret_val = fsl_pq_local_mdio_read(preg,30,27);
	        printk("27: 0x%x\n",ret_val);
#endif
}


int bcm53101_init(struct mii_dev *bus)
{
	unsigned short mdio_val[4] = {0};
	
	
	//gpio_direction_output(BCM53101_GPIO_SEL, BCM53101_A);

	//set IMP port enable 
	memset(mdio_val, 0, sizeof(mdio_val));
	mdio_val[0] = 0x80;
	bcm53101_write(bus, 2, 0x0, mdio_val);

	//set IMP port receive uni/multi/broad cast enable 
	memset(mdio_val, 0, sizeof(mdio_val));
	mdio_val[0] = 0x1c;
	bcm53101_write(bus, 0, 0x08, mdio_val);

	//disable broad header for IMP port
	memset(mdio_val, 0, sizeof(mdio_val));
	mdio_val[0] = 0x0;
	bcm53101_write(bus, 2, 0x03, mdio_val);

	//set Switch Mode forwarding enable and managed mode
	memset(mdio_val, 0, sizeof(mdio_val));
	mdio_val[0] = 0x3;
	bcm53101_write(bus, 0, 0x0b, mdio_val);

	//set IMP Port State 1000M duplex and Link pass
	memset(mdio_val, 0, sizeof(mdio_val));
	mdio_val[0] = 0x8b;
	bcm53101_write(bus, 0, 0x0e, mdio_val);

	//set IMP RGMII RX/TX clock delay enable
	memset(mdio_val, 0, sizeof(mdio_val));
	mdio_val[0] = 0x3;
	bcm53101_write(bus, 0, 0x60, mdio_val);

	return 0;
}

#define DEFAULT_MII_NAME "mdio0"

int fsl_pq_mdio_init(bd_t *bis, struct fsl_pq_mdio_info *info)
{
	struct mii_dev *bus = mdio_alloc();

	if (!bus) {
		printf("Failed to allocate FSL MDIO bus\n");
		return -1;
	}

	bus->read = tsec_phy_read;
	bus->write = tsec_phy_write;
	bus->reset = fsl_pq_mdio_reset;
	sprintf(bus->name, info->name);

	bus->priv = info->regs;

//modified  by zhangjj 2015-12-7
#if 0	
	return mdio_register(bus);
#else
	int ret;
	ret = mdio_register(bus);
	if(!strcmp(info->name, DEFAULT_MII_NAME))  //bcm53101 for outband init
	{
//		printf("mii name =%s\n",info->name);
		bcm53101_init(bus);	
		printf("BCM53101 init done\n");
	}


#endif
}
