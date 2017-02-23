#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/TS-VH401
	NAME:=TouchStar VH401
	PACKAGES:=kmod-usb-core kmod-usb2
endef
define Profile/TS-VH401/Description
	Configuration of TouchStar VH401.
endef

$(eval $(call Profile,TS-VH401))
